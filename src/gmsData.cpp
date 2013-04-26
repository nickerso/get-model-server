#include <iostream>
#include <vector>
#include <algorithm>
#include <sedml/reader.h>
#include <json/json.h>
#include <json/json-forwards.h>

#include "gmsData.hpp"
#include "workspace.hpp"
#include "utils.hpp"
#include "rdfgraph.hpp"

#define MODEL_TYPE_SMALL_MOLECULE "http://cellml.sourceforge.net/ns/model-type/small-molecule"
#define MODEL_TYPE_CELL           "http://cellml.sourceforge.net/ns/model-type/cell"

using namespace GMS;

class WorkspaceLoader
{
public:
    WorkspaceLoader(Data* dataHandle, const std::string& repositoryRoot) : data(dataHandle), root(repositoryRoot)
    {
    }
    // used with for_each
    void operator()(const std::string& workspaceName)
    {
        std::string url = root + workspaceName;
        std::cout << "Loading workspace: \'" << url.c_str() << "\'\n";
        Workspace* workspace = new Workspace(url, workspaceName);
        data->addWorkspace(workspace);
    }

    Data* data;
    const std::string& root;
};

class WorkspaceToJson
{
public:
    WorkspaceToJson(Json::Value& json) : jsonRoot(json)
    {

    }
    // used with for_each
    void operator()(const Workspace* workspace)
    {
        Json::Value w;
        w["id"] = workspace->getId();
        w["url"] = workspace->getUrl();
        jsonRoot["workspaces"].append(w);
    }
    Json::Value& jsonRoot;
};

Data::Data()
{
    std::cout << "Creating new GMS::Data for use in the GET model server." << std::endl;
    mRdfGraph = new RdfGraph();
}

Data::~Data()
{
    std::cout << "Destroying the GMS::Data from the GET model server." << std::endl;
    if (mRdfGraph) delete mRdfGraph;
}

int Data::initialiseModelDatabase(const std::string &repositoryRoot)
{
    int code = 0;
    mRepositoryRoot = repositoryRoot;
    std::string workspacesUrl = repositoryRoot + "workspaces";
    std::string data = getUrlContent(workspacesUrl);
    mModelList = splitString(data, '\n', mModelList);
    WorkspaceLoader loader(this, mRepositoryRoot);
    for_each(mModelList.begin(), mModelList.end(), loader);

    struct sedml_document* doc = sedml_create_document();
    int r = sedml_read_file("http://models.cellml.org/w/andre/sine/rawfile/293afb20feb51d1739b6645eaf2cd18b1a4f3bcb/sin_approximations_sedml.xml", NULL, doc);
    if (!r)
    {
        std::cout << "r is zero!\n";
        if (doc->sedml)
        {
            std::cout << "Found a SED-ML document with level: " << doc->sedml->level << "; version: "
                      << doc->sedml->version << std::endl;
        }
        else
        {
            std::cout << "no sedml document?\n";
        }
    }
    else
    {
        std::cout << "r is not zero! " << r << std::endl;
    }
    /*std::cout << "Model Listing:\n";
    for (int i = 0; i < mModelList.size(); ++i)
    {
        std::cout << "**" << mModelList[i] << "**\n";
    }*/
    return code;
}

int Data::addWorkspace(Workspace *workspace)
{
    mWorkspaces[workspace->getId()] = workspace;
    // add any metadata from this workspace to our RDF graph
    workspace->loadMetadata(this->mRdfGraph);
    return 0;
}

std::string Data::serialiseWorkspaceListing() const
{
    std::string listing;
    Json::Value root;
    std::map<std::string, Workspace*>::const_iterator it;
    for (it = mWorkspaces.begin(); it != mWorkspaces.end(); ++it)
    {
        Json::Value w;
        w["id"] = it->second->getId();
        w["url"] = it->second->getUrl();
        root["workspaces"].append(w);
    }
    listing = Json::FastWriter().write(root);
    return listing;
}

std::string Data::serialiseModelListing() const
{
    std::string listing;
    Json::Value root;
    root["id"] = "root";
    root["type"] = "root";
    root["name"] = "Model Listing";
    std::vector<std::string> models = mRdfGraph->getModelsOfType(MODEL_TYPE_SMALL_MOLECULE);
    std::vector<std::string>::const_iterator it;
    for (it = models.begin(); it != models.end(); ++it)
    {
        Json::Value m;
        m["uri"] = *it;
        m["type"] = MODEL_TYPE_SMALL_MOLECULE;
        root["models"].append(m);
    }
    models = mRdfGraph->getModelsOfType(MODEL_TYPE_CELL);
    for (it = models.begin(); it != models.end(); ++it)
    {
        Json::Value m;
        m["uri"] = *it;
        m["type"] = MODEL_TYPE_CELL;
        root["models"].append(m);
    }
    listing = Json::FastWriter().write(root);
    return listing;
}

std::string Data::serialiseModel(const std::string& modelID) const
{
    std::cout << "serialising model: " << modelID.c_str() << std::endl;
    std::string listing;
    Json::Value root;
    if (modelID == "/root")
    {
        root["id"] = "root";
        root["name"] = "Model Listing";
        root["hasChildren"] = true;
        Json::Value c;
        c["id"] = "MODEL_TYPE_SMALL_MOLECULE";
        c["name"] = "small molecules";
        std::vector<std::string> models = mRdfGraph->getModelsOfType(MODEL_TYPE_SMALL_MOLECULE);
        if (models.size() > 0) c["hasChildren"] = true;
        root["children"].append(c);
        c["id"] = "MODEL_TYPE_CELL";
        c["name"] = "whole cell";
        models = mRdfGraph->getModelsOfType(MODEL_TYPE_CELL);
        if (models.size() > 0) c["hasChildren"] = true;
        root["children"].append(c);
    }
    else if (modelID.find("/MODEL_TYPE_") == 0)
    {
        std::string modelType = "";
        if (modelID == "/MODEL_TYPE_SMALL_MOLECULE") modelType = MODEL_TYPE_SMALL_MOLECULE;
        else if (modelID == "/MODEL_TYPE_CELL") modelType = MODEL_TYPE_CELL;
        if (modelType != "")
        {
            std::vector<std::string> models = mRdfGraph->getModelsOfType(modelType);
            std::vector<std::string>::const_iterator it;
            for (it = models.begin(); it != models.end(); ++it)
            {
                std::string s = mRdfGraph->getResourceTitle(*it);
                Json::Value m;
                m["uri"] = *it;
                m["id"] = *it;
                m["name"] = s;
                m["type"] = MODEL_TYPE_SMALL_MOLECULE;
                m["hasChildren"] = false;
                root["children"].append(m);
            }
        }
    }
    else
    {
        std::cout << "\n\nGetting model: " << modelID.c_str() << std::endl;
    }
    listing = Json::FastWriter().write(root);
    return listing;
}
