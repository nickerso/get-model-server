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
    // and set all variables as outputs, so that we get the variable mappings (see:
    // https://code.google.com/p/cellml-simulator/issues/detail?id=6
    csim->setAllVariablesOutput();
    // store the model for future use.
    mModels[bid] = csim;
    // and initialise the output map
    mOutputMaps[bid] = std::vector<int>();
    Json::Value root;
    // load the model
    root["id"] = bid;
    response = Json::FastWriter().write(root);
    return response;
}

std::string Biomaps::flagOutput(const std::string &modelId, const std::string &componentName,
                                const std::string &variableName, int columnIndex)
{
    // due to https://code.google.com/p/cellml-simulator/issues/detail?id=6 we need to map desired outputs
    // to the actual outputs.
    std::string response;
    if (mModels.count(modelId) != 1)
    {
        response = "The requested model does not exist: " + modelId;
        return response;
    }
    CellmlSimulator* model = mModels[modelId];
    std::string variableId = componentName + ".";
    variableId += variableName;
    std::vector<std::string> variables = model->getModelVariables();
    if (mOutputMaps[modelId].size() < columnIndex+1) mOutputMaps[modelId].resize(columnIndex+1);
    int i = 0;
    for (auto v=variables.begin(); v!=variables.end(); ++i, ++v)
    {
        if (*v == variableId)
        {
            mOutputMaps[modelId].at(columnIndex) = i;
            break;
        }
    }
    Json::Value root;
    // this doesn't work, see above
    // root["returnCode"] = model->addOutputVariable(variableId, columnIndex);
    root["returnCode"] = 0;
    if (root["returnCode"] != 0)
    {
        std::cerr << "Error setting " << componentName << "/" << variableName << ", from model: "
                  << modelId << "; to be an output variable" << std::endl;
    }
    response = Json::FastWriter().write(root);
    return response;
}

std::string Biomaps::compileModel(const std::string& modelId)
{
    std::string response;
    Json::Value root;
    if (mModels.count(modelId) != 1)
    {
        response = "The requested model does not exist: " + modelId;
        return response;
    }
    CellmlSimulator* model = mModels[modelId];
    root["returnCode"] = model->compileModel();
    response = Json::FastWriter().write(root);
    return response;
}

std::string Biomaps::saveModelCheckpoint(const std::string& modelId)
{
    std::string response;
    Json::Value root;
    if (mModels.count(modelId) != 1)
    {
        response = "The requested model does not exist: " + modelId;
        return response;
    }
    CellmlSimulator* model = mModels[modelId];
    root["returnCode"] = model->checkpointModelValues();
    response = Json::FastWriter().write(root);
    return response;
}

std::string Biomaps::loadModelCheckpoint(const std::string& modelId)
{
    std::string response;
    Json::Value root;
    if (mModels.count(modelId) != 1)
    {
        response = "The requested model does not exist: " + modelId;
        return response;
    }
    CellmlSimulator* model = mModels[modelId];
    root["returnCode"] = model->updateModelFromCheckpoint();
    if (root["returnCode"] == 0) model->resetIntegrator(); // is this necessary?
    response = Json::FastWriter().write(root);
    return response;
}

std::string Biomaps::setVariableValue(const std::string &modelId, const std::string &componentName,
                                      const std::string &variableName, double value)
{
    std::string response;
    if (mModels.count(modelId) != 1)
    {
        response = "The requested model does not exist: " + modelId;
        return response;
    }
    CellmlSimulator* model = mModels[modelId];
    std::string variableId = componentName + ".";
    variableId += variableName;
    Json::Value root;
    root["returnCode"] = model->setVariableValue(variableId, value);
    if (root["returnCode"] != 0)
    {
        std::cerr << "Error setting value for " << componentName << "/" << variableName << ", from model: "
                  << modelId << "; id: " << variableId << std::endl;
    }
    response = Json::FastWriter().write(root);
    return response;
}

std::string Biomaps::execute(const std::string &modelId, double startTime, double endTime, double outputInterval)
{
    std::string response;
    if (mModels.count(modelId) != 1)
    {
        response = "The requested model does not exist: " + modelId;
        return response;
    }
    Json::Value root; // to store the results
    CellmlSimulator* model = mModels[modelId];
    model->resetIntegrator();
    // make sure we are at the start time
    if (startTime > outputInterval) model->simulateModelOneStep(startTime);
    std::vector<double> data = model->getModelOutputs();
    int col = 0;
    int row = 0;
    for (const auto& i: mOutputMaps[modelId]) root["data"][col++][row] = data[i];
    for (double time = startTime; time <= endTime; time += outputInterval)
    {
        model->simulateModelOneStep(outputInterval);
        data = model->getModelOutputs();
        col = 0; row++;
        for (const auto& i: mOutputMaps[modelId]) root["data"][col++][row] = data[i];
    }
    response = Json::FastWriter().write(root);
    return response;
}

