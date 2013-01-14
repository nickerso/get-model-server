#include <iostream>

#include <map>
#include <string>

#include <CellmlSimulator.hpp>

#include <json/json.h>
#include <json/json-forwards.h>

#include "gmsApi.hpp"
#include "gmsData.hpp"

using namespace GMS;

API::API()
{
    std::cout << "Creating new GMS::API for use by the GET model server" << std::endl;
}

API::~API()
{
    std::cout << "Destroying a GMS::API object from the GET model server" << std::endl;
}

const char API::URL_MODEL[] = "/model/";
const char API::URL_EXECUTE[] = "/execute/";

bool API::executeAPI(const std::string& url, const std::map<std::string, std::string>& argvals,
                     std::string& response)
{
    std::cout << "executeAPI for URL: \"" << url.c_str() << "\"" << std::endl;
    std::string modelUrl = url.substr(0, strlen(URL_MODEL));
    std::string executeUrl = url.substr(0, strlen(URL_EXECUTE));
    bool ret = false;
    // http://localhost/model, http://localhost/model/model_id, http://localhost/model/, http://localhost/model?q=bob all valid
    if ((modelUrl == "/model/" ) || (modelUrl == "/model"))
    {
        // request for all models
        if (modelUrl.length() == url.length())
        {
            response = "[";
            response += "{\"name\":\"Bubble Gum\", \"quantity\": 33, \"category\": \"Food\"},";
            response += "{\"name\":\"Trouble Gum\", \"quantity\": 21, \"category\": \"Food\"},";
            response += "{\"name\":\"Truck\", \"quantity\": 13, \"category\": \"Vehicle\"},";
            response += "{\"name\":\"Car\", \"quantity\": 213, \"category\": \"Vehicle\"},";
            response += "{\"name\":\"Oak Tree\", \"quantity\": 2, \"category\": \"Plant\"}";
            response += "]";
        }
        else if (argvals.size() > 0)
        {
            std::cout << "some queries to handle?" << std::endl;
        }
        else
        {
            std::string id = url.substr(modelUrl.length(), url.find("/", modelUrl.length()+1));
            std::cout << "getting model ID: \"" << id.c_str() << "\"" << std::endl;
            response += "{'ID': 'Fred', 'data': [0,1,2,3,4,5,6]}";
        }
    }
    // http://localhost/execute/sed_id is the only URL that makes sense?
    else if (executeUrl == URL_EXECUTE)
    {
        std::string id = url.substr(executeUrl.length(), url.find('/', executeUrl.length()+1));
        std::cout << "executing SED: \"" << id.c_str() << "\"" << std::endl;
        CellmlSimulator csim;
        std::string mackenzieModel = "/Users/dnic019/shared-folders/projects/kidney/general-epithelial-transport/gui/version-3/server/models/mackenzie_loo_panayotova-heiermann_wright_1996/experiments/figure-03.xml";
        mackenzieModel = "http://models.cellml.org/w/andre/sine/rawfile/293afb20feb51d1739b6645eaf2cd18b1a4f3bcb/sin_approximations_import.xml";
        std::string modelString = csim.serialiseCellmlFromUrl(mackenzieModel.c_str());
        //std::cout << modelString.c_str();
        int flag = csim.loadModelString(modelString);
        if (flag)
        {
            std::cerr << "loadModelString fail!\n";
            abort();
        }
        flag = csim.createSimulationDefinition();
        if (flag)
        {
            std::cerr << "createSimulationDefinition fail!\n";
            abort();
        }
        flag = csim.compileModel();
        if (flag)
        {
            std::cerr << "compileModel fail!\n";
            abort();
        }
        flag = csim.checkpointModelValues();
        if (flag)
        {
            std::cerr << "initial checkpointModelValues fail!\n";
            abort();
        }
        Json::Value charts;
        Json::Value curve, data;
        int dCounter = 0;
        charts["id"] = "sine_approximations_import";
        std::vector<double> dataValues = csim.getModelOutputs();
        // 0u to distinguish from [char*] operator with NULL string key
        charts["curves"][0u]["data"][dCounter]["x"] = dataValues[0];
        charts["curves"][0u]["data"][dCounter]["y"] = dataValues[1];
        charts["curves"][1]["data"][dCounter]["x"] = dataValues[0];
        charts["curves"][1]["data"][dCounter]["y"] = dataValues[2];
        charts["curves"][2]["data"][dCounter]["x"] = dataValues[0];
        charts["curves"][2]["data"][dCounter]["y"] = dataValues[4];
        ++dCounter;
        double t = 0, dt = 0.7, tEnd = 7.0;
        while (t <= tEnd)
        {
            flag = csim.simulateModelOneStep(dt);
            if (flag)
            {
                std::cerr << "simulateOneStep fail!\n";
                abort();
            }
            dataValues = csim.getModelOutputs();
            charts["curves"][0u]["data"][dCounter]["x"] = dataValues[0];
            charts["curves"][0u]["data"][dCounter]["y"] = dataValues[1];
            charts["curves"][1]["data"][dCounter]["x"] = dataValues[0];
            charts["curves"][1]["data"][dCounter]["y"] = dataValues[2];
            charts["curves"][2]["data"][dCounter]["x"] = dataValues[0];
            charts["curves"][2]["data"][dCounter]["y"] = dataValues[4];
            ++dCounter;
            t += dt;
        }
        /*
        series["id"] = "Membrane Voltage";
        data[0u]["x"] = 0.0;
        data[0u]["y"] = 0.0;
        data[1]["x"] = 1.5;
        data[1]["y"] = 5.0;
        data[2]["x"] = 3.8;
        data[2]["y"] = 4.1;
        data[3]["x"] = 8.7;
        data[3]["y"] = 7.0;
        series["data"] = data;
        root["series"][0u] = series;
        series["id"] = "SomethingElse";
        data[0u]["x"] = -1.0;
        data[0u]["y"] = -2.0;
        data[1]["x"] = 1.4;
        data[1]["y"] = 5.0;
        data[2]["x"] = 3.8;
        data[2]["y"] = -4.1;
        data[3]["x"] = 8.7;
        data[3]["y"] = 7.0;
        series["data"] = data;
        root["series"][1] = series;*/
        //std::cout << root;
        response = Json::FastWriter().write(charts);
        //sleep(2);
    }
    else
    {
        getInvalidResponse(response);
    }
    return ret;
}
void API::getInvalidResponse(std::string& response)
{
    response = "Some error in your data ";
}
