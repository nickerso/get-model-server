#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <json/json.h>
#include <json/json-forwards.h>

#include "annotator.hpp"
#include "xmlDoc.hpp"
#include "utils.hpp"
#include "rdfgraph.hpp"
#include "namespaces.hpp"

static std::pair<std::string, std::string> parseUri(const std::string& uri)
{
    std::pair<std::string, std::string> p;
    std::string child = urlChildOf(uri, ID_ORG_UNIPROT);
    if (child.size() > 0)
    {
        p.first = UNIPROT_NAME;
        p.second = child;
        return p;
    }
    child = urlChildOf(uri, ID_ORG_FMA);
    if (child.size() > 0)
    {
        p.first = FMA_NAME;
        p.second = child;
        return p;
    }
    child = urlChildOf(uri, ID_ORG_DOI);
    if (child.size() > 0)
    {
        p.first = DOI_NAME;
        p.second = child;
        return p;
    }
    child = urlChildOf(uri, ID_ORG_PUBMED);
    if (child.size() > 0)
    {
        p.first = PUBMED_NAME;
        p.second = child;
        return p;
    }
    p.first = "UNKNOWN";
    p.second = "UNKNOWN_ID";
    return p;
}

Annotator::Annotator(const std::string& repositoryRoot, const std::string& repositoryLocalPath) :
    mRepositoryRoot(repositoryRoot), mRepositoryLocalPath(repositoryLocalPath), mSourceDocument(0),
    mRdfGraph(0)
{

}

Annotator::~Annotator()
{
    if (mSourceDocument) delete mSourceDocument;
    if (mRdfGraph) delete mRdfGraph;
}

std::string Annotator::loadSource(const std::string &url)
{
    std::string response;
    std::string sourceUrl = mRepositoryRoot + url;
    std::cout << "loading model: " << sourceUrl << std::endl;
    std::string sourceContent = getUrlContent(sourceUrl);
    if (sourceContent.size() == 0)
    {
        Json::Value root;
        root["returnCode"] = 1;
        response = Json::FastWriter().write(root);
        return response;
    }
    mSourceDocument = new XmlDoc();
    if (mSourceDocument->parseString(sourceContent) != 0)
    {
        delete mSourceDocument;
        mSourceDocument = NULL;
        Json::Value root;
        root["returnCode"] = 2;
        response = Json::FastWriter().write(root);
        return response;
    }
    // success
    mSourceUrl = sourceUrl;
    mSourceFile = mRepositoryLocalPath + url;
    Json::Value root;
    root["returnCode"] = 0;
    response = Json::FastWriter().write(root);
    return response;
}

std::string Annotator::loadAnnotations(const std::string &url)
{
    std::string response;
    std::string annotationsUrl = mRepositoryRoot + url;
    std::cout << "creating RDF graph from: " << annotationsUrl << std::endl;
    mRdfGraph = new RdfGraph();
    mRdfGraph->parseRdfXmlUrl(annotationsUrl);
    mRdfGraph->cacheGraph();
    // success
    mAnnotationsFile = mRepositoryLocalPath + url;
    Json::Value root;
    root["returnCode"] = 0;
    response = Json::FastWriter().write(root);
    return response;
}

std::string Annotator::getSourceComponents() const
{
    std::string response;
    std::vector<std::pair<std::string, std::string> > components = mSourceDocument->getCellmlComponentList();
    Json::Value root;
    int j = 0;
    for (const auto& i: components)
    {
        Json::Value c;
        c["name"] = i.first;
        c["id"] = i.second;
        root["components"][j++] = c;
    }
    root["returnCode"] = 0;
    response = Json::FastWriter().write(root);
    return response;
}

std::string Annotator::handlePostData(const std::string& jsonData)
{
    Json::Value returnCode;
    std::string response;
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonData, root);
    if (!parsingSuccessful)
    {
        // report to the user the failure and their locations in the document.
        std::cerr  << "Failed to parse JSON data in annotator post"
                   << reader.getFormattedErrorMessages();
        returnCode["returnCode"] = 1;
        response = Json::FastWriter().write(returnCode);
        return response;
    }
    std::string dataType = root["type"].asString();
    if (dataType.size() == 0)
    {
        std::cerr  << "Failed to get a data type from the JSON data in annotator post\n"
                   << std::endl;
        returnCode["returnCode"] = 2;
        response = Json::FastWriter().write(returnCode);
        return response;
    }
    if (dataType == "cellmlComponent")
    {
        // create a new component in the source document
        if (mSourceDocument->createCellmlComponent(root["name"].asString(), root["id"].asString()) != 0)
        {
            std::cerr  << "ERROR creating CellML Component in source document\n"
                       << std::endl;
            returnCode["returnCode"] = 3;
            response = Json::FastWriter().write(returnCode);
            return response;
        }
    }
    else if (dataType == "annotation")
    {
        // create a new annotation
        std::string sourceUri = mSourceUrl + "#";
        sourceUri += root["sourceId"].asString();
        std::string qualifier = root["qualifier"].asString();
        std::string object = root["uri"].asString();
        if (mRdfGraph->createTriple(sourceUri, qualifier, object) != 0)
        {
            std::cerr  << "ERROR creating annotation in RDF Graph\n"
                       << std::endl;
            returnCode["returnCode"] = 4;
            response = Json::FastWriter().write(returnCode);
            return response;
        }
        Json::Value evidence = root["evidence"];
        if (evidence.isArray())
        {
            if (!evidence.empty())
            {
                // FIXME: just assume one entry for now.
                std::string evidenceQualifier = root["evidence"][0]["qualifier"].asString();
                std::string evidenceUri = root["evidence"][0]["ontology"].asString();
                evidenceUri += root["evidence"][0]["identifier"].asString();
                if (mRdfGraph->addEvidenceToGraph(sourceUri, qualifier, object, evidenceQualifier, evidenceUri) != 0)
                {
                    std::cerr  << "ERROR creating evidence annotations in RDF Graph\n"
                               << std::endl;
                    returnCode["returnCode"] = 5;
                    response = Json::FastWriter().write(returnCode);
                    return response;
                }
            }
        }
    }
    returnCode["returnCode"] = 0;
    response = Json::FastWriter().write(returnCode);
    return response;
}

std::string Annotator::saveSource() const
{
    Json::Value returnCode;
    std::string response;
    std::cout << "New model to save to:" << mSourceFile << std::endl;
    std::cout << mSourceDocument->asString() << std::endl;
    std::ofstream output;
    output.open(mSourceFile);
    output << mSourceDocument->asString();
    returnCode["returnCode"] = 0;
    response = Json::FastWriter().write(returnCode);
    return response;
}

std::string Annotator::saveAnnotations() const
{
    Json::Value returnCode;
    std::string response;
    std::cout << "New annotations to save to:" << mAnnotationsFile << std::endl;
    std::cout << mRdfGraph->getGraphCache() << std::endl;
    std::ofstream output;
    output.open(mAnnotationsFile);
    output << mRdfGraph->getGraphCache();
    returnCode["returnCode"] = 0;
    response = Json::FastWriter().write(returnCode);
    return response;
}

std::string Annotator::fetchAnnotations(const std::string& sourceId)
{
    Json::Value results;
    std::string response;
    std::string sourceUri = mSourceUrl + "#";
    sourceUri += sourceId;
    // first the bqmodel:isInstanceOf annotations
    std::string qualifier(BQMODEL_NS "isInstanceOf");
    std::vector<std::string> objects = mRdfGraph->getAnnotationsForResource(sourceUri, qualifier);
    int i = 0;
    if (objects.size() > 0)
    {
        std::cout << "Found some instanceOf anntations for: " << sourceUri << std::endl;
        for (const auto& u: objects)
        {
            Json::Value object;
            object["qualifier"] = qualifier;
            object["uri"] = u;
            std::pair<std::string, std::string> ontology = parseUri(u);
            object["ontology"] = ontology.first;
            object["identifier"] = ontology.second;
            // can we also find any evidence for this annotation
            std::vector<std::pair<std::string, std::string> > evidence =
                    mRdfGraph->getEvidenceForStatement(sourceUri, qualifier, u);
            if (evidence.size() > 0)
            {
                int j = 0;
                for (const auto& p: evidence)
                {
                    Json::Value e;
                    e["qualifier"] = p.first;
                    ontology = parseUri(p.second);
                    e["ontology"] = ontology.first;
                    e["identifier"] = ontology.second;
                    object["evidence"][j++] = e;
                }
            }
            // add this annotation to the collection
            results["annotations"][i++] = object;
        }
    }
    // then the ro:located_in annotations
    qualifier = std::string(OBO_RO_NS "located_in");
    objects = mRdfGraph->getAnnotationsForResource(sourceUri, qualifier);
    if (objects.size() > 0)
    {
        std::cout << "Found some ro:located_in anntations for: " << sourceUri << std::endl;
        for (const auto& u: objects)
        {
            Json::Value object;
            object["qualifier"] = qualifier;
            object["uri"] = u;
            std::pair<std::string, std::string> ontology = parseUri(u);
            object["ontology"] = ontology.first;
            object["identifier"] = ontology.second;
            // can we also find any evidence for this annotation
            std::vector<std::pair<std::string, std::string> > evidence =
                    mRdfGraph->getEvidenceForStatement(sourceUri, qualifier, u);
            if (evidence.size() > 0)
            {
                int j = 0;
                for (const auto& p: evidence)
                {
                    Json::Value e;
                    e["qualifier"] = p.first;
                    ontology = parseUri(p.second);
                    e["ontology"] = ontology.first;
                    e["identifier"] = ontology.second;
                    object["evidence"][j++] = e;
                }
            }
            // add this annotation to the collection
            results["annotations"][i++] = object;
        }
    }
    // then the bqmodel:isDescribedBy annotations
    qualifier = std::string(BQMODEL_NS "isDescribedBy");
    objects = mRdfGraph->getAnnotationsForResource(sourceUri, qualifier);
    if (objects.size() > 0)
    {
        std::cout << "Found some bqmodel:isDescribedBy annotations for: " << sourceUri << std::endl;
        for (const auto& u: objects)
        {
            Json::Value object;
            object["qualifier"] = qualifier;
            object["uri"] = u;
            std::pair<std::string, std::string> ontology = parseUri(u);
            object["ontology"] = ontology.first;
            object["identifier"] = ontology.second;
            // can we also find any evidence for this annotation
            std::vector<std::pair<std::string, std::string> > evidence =
                    mRdfGraph->getEvidenceForStatement(sourceUri, qualifier, u);
            if (evidence.size() > 0)
            {
                int j = 0;
                for (const auto& p: evidence)
                {
                    Json::Value e;
                    e["qualifier"] = p.first;
                    ontology = parseUri(p.second);
                    e["ontology"] = ontology.first;
                    e["identifier"] = ontology.second;
                    object["evidence"][j++] = e;
                }
            }
            // add this annotation to the collection
            results["annotations"][i++] = object;
        }
    }
    results["returnCode"] = 0;
    response = Json::FastWriter().write(results);
    return response;
}
