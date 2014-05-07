#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <json/json.h>
#include <json/json-forwards.h>
#include <CellmlSimulator.hpp>

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
    // clean up
    for (auto it = mModels.begin(); it != mModels.end(); ++it)
    {
        if (it->second) delete it->second;
    }
}

std::string Biomaps::loadModel(const std::string &url)
{
    std::string response;
    std::string modelUrl = mRepositoryRoot + url;
    std::cout << "loading model: " << modelUrl << std::endl;
    // get the identifier we will use for this model
    std::string bid = defineIdentifier();
    CellmlSimulator* csim = new CellmlSimulator();
    std::string modelString = csim->serialiseCellmlFromUrl(modelUrl);
    if (csim->loadModelString(modelString) != 0)
    {
        delete csim;
        response = "There was an error loading model: " + modelUrl;
        return response;
    }
    // create the dummy simulation description - why?
    csim->createSimulationDefinition();
    // store the model for future use.
    mModels[bid] = csim;
    Json::Value root;
    // load the model
    root["id"] = bid;
    response = Json::FastWriter().write(root);
    return response;
}
