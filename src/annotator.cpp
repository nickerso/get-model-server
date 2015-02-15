#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <json/json.h>
#include <json/json-forwards.h>

#include "annotator.hpp"

Annotator::Annotator(const std::string& repositoryRoot, const std::string& repositoryLocalPath) :
    mRepositoryRoot(repositoryRoot), mRepositoryLocalPath(repositoryLocalPath)
{

}

Annotator::~Annotator()
{

}

std::string Annotator::loadSource(const std::string &url)
{
    std::string response;
    std::string sourceUrl = mRepositoryRoot + url;
    std::cout << "loading model: " << sourceUrl << std::endl;
    Json::Value root;
    // load the model
    root["id"] = mRepositoryLocalPath + url;
    response = Json::FastWriter().write(root);
    return response;
}

