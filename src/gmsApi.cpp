#include <iostream>

#include <map>
#include <string>

#include <CellmlSimulator.hpp>

#include <json/json.h>
#include <json/json-forwards.h>

#include "gmsApi.hpp"
#include "gmsData.hpp"
#include "dojotest.hpp"
#include "utils.hpp"

using namespace GMS;

API::API()
{
    std::cout << "Creating new GMS::API for use by the GET model server" << std::endl;
}

API::~API()
{
    std::cout << "Destroying a GMS::API object from the GET model server" << std::endl;
}

const std::string API::URL_MODEL = "/models/";
const std::string API::URL_QUERY = "/query";
const std::string API::URL_SEARCH = "/search";
const std::string API::URL_DOJO_TEST = "/dojo-test";
const std::string API::URL_BIOMAPS = "/biomaps";

std::string API::executeAPI(const std::string& url, const std::map<std::string, std::string>& argvals,
                            GMS::Data *data)
{
    std::string response;
    std::cout << "executeAPI for URL: \"" << url.c_str() << "\"" << std::endl;

    // test data from Dojo examples
    std::string child = urlChildOf(url, URL_DOJO_TEST);
    if (child.size() > 0)
    {
        return handleDojoTestRequest(url, argvals);
    }

    // check for a model-type URL (/model, /model/, /model/id, etc)
    child = urlChildOf(url, URL_MODEL);
    if ((child.size() > 0) || (url == URL_MODEL))
    {
        return handleModelRequest(url, argvals, data);
    }
    // FIXME: will this just be done as queries on the model URL?
    // else check for a search/query-type URL (/search?species=rat&membrane=basal, /query/?author=andre, etc.)
    std::string urlTest = url.substr(0, URL_SEARCH.size() > URL_QUERY.size() ? URL_SEARCH.size() : URL_QUERY.size());
    if (urlTest.find('/', 1) == std::string::npos) urlTest.push_back('/');
    if ((urlTest == URL_SEARCH) || (urlTest == URL_QUERY))
    {
        return handleQueryRequest(url, argvals, data);
    }

    // a hack to get something working like biomaps needs
    child = urlChildOf(url, URL_BIOMAPS);
    if (child.size() > 0)
    {
        return handleBiomapsRequest(url, argvals, data);
    }

#if 0
    std::string modelUrl = url.substr(0, strlen(URL_MODEL));
    std::string executeUrl = url.substr(0, strlen(URL_EXECUTE));
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
        //std::cout << charts;
        response = Json::FastWriter().write(charts);
    }
    else
    {
        getInvalidResponse(response);
    }
#endif
    // unhandled API method called
    std::cerr << "Unhandled API method for GMS: " << url.c_str() << std::endl;
    getInvalidResponse(response);
    return response;
}

void API::getInvalidResponse(std::string& response)
{
    response = "Some error in your data ";
}

std::string API::handleModelRequest(const std::string& url, const std::map<std::string, std::string>& argvals,
                               GMS::Data* data)
{
    std::cout << "handleModelRequest: " << url.c_str() << std::endl;
    std::string response;
    std::string trailer = urlChildOf(url, URL_MODEL);
    std::cout << "argvals: ";
    for(auto iter=argvals.begin(); iter!=argvals.end(); ++iter)
    {
        std::cout << "args:: key(" << iter->first << ") -> value(" << iter->second << ")" << std::endl;
    }
    if ((trailer.size() != 0) && argvals.empty())
    {
        // request for information about a specific model
        std::vector<std::string> strings;
        strings = splitString(trailer, '/', strings);
        if (strings.size() == 2)
        {
            std::string modelId = strings[0];
            std::string action = strings[1];
            //response = data->serialiseWorkspaceListing();
            response = data->performModelAction(modelId, action);
        }
        else
        {
            std::cerr << "expecting model ID with exactly one action to perform: " << url.c_str() << std::endl;
            getInvalidResponse(response);
        }
    }
    else if (!argvals.empty())
    {
        // FIXME: type is the only argument we currently handle.
        if (argvals.count("type")) response = data->serialiseModelsOfType(argvals.at("type"));
        else getInvalidResponse(response);
    }
    else
    {
        // unhandled model request
        std::cerr << "Unable to handle model request with args: " << url.c_str() << std::endl;
        getInvalidResponse(response);
    }
    return response;
}

std::string API::handleBiomapsRequest(const std::string& url, const std::map<std::string, std::string>& argvals,
                                      GMS::Data* data)
{
    std::cout << "\n\nhandleBiomapsRequest: " << url.c_str() << std::endl;
    std::string response;
    std::string trailer = urlChildOf(url, URL_BIOMAPS);
    Biomaps* biomaps = data->getBiomaps();
    if (biomaps) std::cout << "Managed to get a biomaps manager object" << std::endl;

    for(auto iter=argvals.begin(); iter!=argvals.end(); ++iter)
    {
        std::cout << "args:: key(" << iter->first << ") -> value(" << iter->second << ")" << std::endl;
    }
    if (trailer.size() != 0)
    {
        std::vector<std::string> strings;
        strings = splitString(trailer, '/', strings);
        for (auto s=strings.begin(); s!=strings.end(); ++s) std::cout << "URL part: " << *s << std::endl;
    }
    else if (!argvals.empty())
    {
        // FIXME: type is the only argument we currently handle.
        if (argvals.count("type")) response = data->serialiseModelsOfType(argvals.at("type"));
        else getInvalidResponse(response);
    }
    else
    {
        // unhandled model request
        std::cerr << "Unable to handle model request with args: " << url.c_str() << std::endl;
        getInvalidResponse(response);
    }
    return response;
}

std::string API::handleDojoTestRequest(const std::string &url, const std::map<std::string, std::string>& argvals)
{
    std::string response;
    DojoTest dojo;
    response = dojo.handleRequest(url, argvals);
    return response;
}

std::string API::handleQueryRequest(const std::string& url, const std::map<std::string, std::string>& argvals,
                               GMS::Data* data)
{
    std::string response;
    return response;
}
