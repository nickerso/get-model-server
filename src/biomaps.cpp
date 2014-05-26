#include <string>
#include <sstream>
#include <cstring>
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
    // and since we have all variables as outputs we can even compile the model here
    csim->compileModel();
    // and save the initial checkpoint
    csim->checkpointModelValues();
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
    if ((int)(mOutputMaps[modelId].size()) < columnIndex+1) mOutputMaps[modelId].resize(columnIndex+1);
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

std::string Biomaps::setVariableValue(const std::string &modelId, const std::string &componentName,
                                      const std::string &variableName, const std::string& datasetId,
                                      bool offsetDataTime)
{
    std::string response;
    if (mModels.count(modelId) != 1)
    {
        response = "The requested model does not exist: " + modelId;
        return response;
    }
    if (mDatasets.count(datasetId) != 1)
    {
        response = "The requested dataset does not exist: " + datasetId;
        return response;
    }
    std::string variableId = componentName + ".";
    variableId += variableName;
    // check we can set the variable value
    auto& dataset = mDatasets[datasetId];
    double value = dataset[0].second;
    CellmlSimulator* model = mModels[modelId];
    Json::Value root;
    root["returnCode"] = model->setVariableValue(variableId, value);
    if (root["returnCode"] != 0)
    {
        std::cerr << "Error setting value from dataset for " << componentName << "/" << variableName
                  << ", from model: " << modelId << "; id: " << variableId << std::endl;
    }
    else
    {
        mDatasetVariableMap[modelId].push_back(std::pair<std::string, std::string>(variableId, datasetId));
        mDatasetTimeOffset[modelId].push_back(std::pair<std::string, bool>(variableId, offsetDataTime));
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
    setDataDrivenValues(modelId, startTime);
    std::vector<double> data = model->getModelOutputs();
    int col = 0;
    int row = 0;
    for (const auto& i: mOutputMaps[modelId]) root["data"][col++][row] = data[i];
    for (double time = startTime; time <= endTime; time += outputInterval)
    {
        setDataDrivenValues(modelId, time);
        model->simulateModelOneStep(outputInterval);
        data = model->getModelOutputs();
        col = 0; row++;
        for (const auto& i: mOutputMaps[modelId]) root["data"][col++][row] = data[i];
    }
    response = Json::FastWriter().write(root);
    return response;
}

char* Biomaps::createDatasetId() const
{
    std::string id = defineIdentifier();
    char* s = (char*)malloc(id.size() + 1);
    strcpy(s, id.c_str());
    return s;
}

int Biomaps::setDatasetContent(const std::string& id, const std::string& jsonData)
{
    // clear any existing values
    (mDatasets[id]).clear();
    //std::cout << "Setting the dataset: " << id << "to the content: " << jsonData << std::endl;
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonData, root);
    if (!parsingSuccessful)
    {
        // report to the user the failure and their locations in the document.
        std::cerr  << "Failed to parse configuration\n"
                   << reader.getFormattedErrorMessages();
        return -1;
    }
    std::string bob = Json::FastWriter().write(root);
    std::cout << "Parsed data: " << bob << std::endl;
    const Json::Value time = root["time"];
    const Json::Value value = root["value"];
    (mDatasets[id]).resize(time.size());
    for ( unsigned int index = 0; index < time.size(); ++index )  // Iterates over the sequence elements.
    {
        (mDatasets[id])[index] = std::pair<double, double>(time[index].asDouble(), value[index].asDouble());
        std::cout << index << "time: " << time[index].asDouble() << "; value: " << value[index].asDouble()
                  << std::endl;
    }
    return 0;
}

void Biomaps::setDataDrivenValues(const std::string& modelId, double time)
{
    // MAP[ <modelID> ] ==> vector( <variableID> <datasetID> )
    //std::map<std::string, std::vector<std::pair<std::string, std::string> > > mDatasetVariableMap;
    // MAP[ <modelID> ] ==> vector( <variableID> <offsetDataTime> )
    //std::map<std::string, std::vector<std::pair<std::string, bool> > > mDatasetTimeOffset;
    if (mDatasetVariableMap.count(modelId) > 0)
    {
        CellmlSimulator* model = mModels.at(modelId);
        const auto& variables = mDatasetVariableMap.at(modelId);
        for (const auto& v: variables)
        {
            //std::cout << "Model ID: " << modelId << "; Variable ID: " << v.first << "; dataset: "
            //          << v.second << std::endl;
            double value = getDataValue(v.second, time);
            model->setVariableValue(v.first, value);
        }
    }
}

double Biomaps::getDataValue(const std::string &datasetId, double time) const
{
    double value = 0; // resonable default?
    //std::map<std::string, std::vector<std::pair<double, double> > > mDatasets;
    if (mDatasets.count(datasetId) == 1)
    {
        const auto& values = mDatasets.at(datasetId);
        int index = 0;
        for (const auto& v: values)
        {
            if (v.first < time) ++index;
            else break;
        }
        double t1 = values[index-1].first;
        double t2 = values[index].first;
        double v1 = values[index-1].second;
        double v2 = values[index].second;
        // linear interpolation
        double xi = (time - t1) / (t2 - t1);
        value = (1.0 - xi) * v1 + xi * v2;
        std::cout << "index: " << index << "; t1: " << t1 << "; t2: " << t2
                  << "; v1: " << v1 << "; v2: " << v2 << std::endl;
        std::cout << "xi: " << xi << "; value: " << value << std::endl;
    }
    return value;
}
