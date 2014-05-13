#include <microhttpd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <curl/curl.h>

#include "gms-config.h"
#include "gmsData.hpp"
#include "libhttpd-utils.hpp"

static void printVersion()
{
    std::ostringstream versionString;
    versionString << "GMS " << GMS_VERSION_MAJOR << "." << GMS_VERSION_MINOR << "."
                  << GMS_VERSION_PATCH;
    std::cout << "This is GET Model Server version: " << versionString.str() << std::endl;
}

int main(int argc, char ** argv)
{
    printVersion();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    if (argc < 3)
    {
        printf("%s <PORT> <echo|log|GMS> [Model Root]\n", argv[0]);
        return 1;
    }
    GMS::Data* data = new GMS::Data();
    if (0 == strcmp(argv[2], "GMS"))
    {
        std::string repositoryRoot = "http://localhost:9876/models/";
        if (argc > 3) repositoryRoot = std::string(argv[3]);
        int code = data->initialiseModelDatabase(repositoryRoot);
        //int code = data->initialiseModelDatabase("file:///Users/dnic019/shared-folders/projects/kidney/GeneralEpithelialTransport/www/models/");
        if (code != 0)
        {
            std::cerr << "Error initialising database\n";
            abort();
        }
    }
    startServer(atoi(argv[1]), argv[2], data);
    delete data;
    curl_global_cleanup();
    return 0;
}
