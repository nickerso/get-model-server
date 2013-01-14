#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <sstream>

#include "gmsData.hpp"

using namespace GMS;

static std::vector<std::string>&
splitString(const std::string &s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

static size_t retrieveContent(char* buffer, size_t size, size_t nmemb, void* string)
{
    std::string* s = static_cast<std::string*>(string);
    size_t bytes = size * nmemb;
    s->append(buffer, bytes);
    return bytes;
}

Data::Data()
{
    std::cout << "Creating new GMS::Data for use in the GET model server." << std::endl;
    CURL* curl = curl_easy_init();
    mCurl = static_cast<void*>(curl);
}

Data::~Data()
{
    std::cout << "Destroying the GMS::Data from the GET model server." << std::endl;
    if (mCurl)
    {
        CURL* curl = static_cast<CURL*>(mCurl);
        curl_easy_cleanup(curl);
    }
}

int Data::initialiseModelDatabase(const std::string &repositoryRoot)
{
    int code = 0;
    if (!mCurl)
    {
        std::cerr << "Missing CURL handle?\n";
        return -2;
    }
    mRepositoryRoot = repositoryRoot;
    std::string workspacesUrl = repositoryRoot + "workspaces";
    std::string data;
    CURL* curl = static_cast<CURL*>(mCurl);
    curl_easy_setopt(curl, CURLOPT_URL, workspacesUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, retrieveContent);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*>(&data));
    CURLcode res = curl_easy_perform(curl);
    if(CURLE_OK != res)
    {
        /* we failed */
        fprintf(stderr, "curl told us %d\n", res);
        return -1;
    }
    mModelList = splitString(data, '\n', mModelList);
    /*std::cout << "Model Listing:\n";
    for (int i = 0; i < mModelList.size(); ++i)
    {
        std::cout << "**" << mModelList[i] << "**\n";
    }*/
    return code;
}
