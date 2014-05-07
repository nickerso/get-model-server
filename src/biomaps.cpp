#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <json/json.h>
#include <json/json-forwards.h>

#include "biomaps.hpp"

static std::string defineIdentifier()
{
    static int idCounter = 1024;
    std::ostringstream oss;
    oss << idCounter++;
    std::string id = "b" + oss.str();
    return id;
}

Biomaps::Biomaps(const std::string& repositoryRoot) :
    mRepositoryRoot(repositoryRoot)
{

}

Biomaps::~Biomaps()
{
    // nothing to do yet?
}

std::string Biomaps::loadModel(const std::string &url)
{
    std::string modelUrl = mRepositoryRoot + url;
    std::cout << "loading model: " << modelUrl << std::endl;
    std::string response;
    Json::Value root;
    // load the model
    root["id"] = defineIdentifier();
    response = Json::FastWriter().write(root);
    return response;
}
