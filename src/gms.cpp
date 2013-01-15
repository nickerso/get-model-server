#include <microhttpd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <map>
#include <curl/curl.h>

#include "gmsData.hpp"
#include "libhttpd-utils.hpp"

int main(int argc, char ** argv)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
	if (argc != 3)
	{
        printf("%s <PORT> <echo|log|GMS>\n", argv[0]);
		return 1;
	}
    GMS::Data* data = new GMS::Data();
    if (0 == strcmp(argv[2], "GMS"))
	{
        int code = data->initialiseModelDatabase("http://localhost:9876/models/");
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
