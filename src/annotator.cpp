#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <json/json.h>
#include <json/json-forwards.h>

#include "annotator.hpp"
#include "xmlDoc.hpp"
#include "utils.hpp"

Annotator::Annotator(const std::string& repositoryRoot, const std::string& repositoryLocalPath) :
    mRepositoryRoot(repositoryRoot), mRepositoryLocalPath(repositoryLocalPath), mSourceDocument(0)
{

}

Annotator::~Annotator()
{
    if (mSourceDocument) delete mSourceDocument;
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
    mSourceFile = url;
    Json::Value root;
    root["returnCode"] = 0;
    response = Json::FastWriter().write(root);
    return response;
}

