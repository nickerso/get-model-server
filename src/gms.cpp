#include <microhttpd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <map>

#include "gmsData.hpp"
#include "libhttpd-utils.hpp"

int main(int argc, char ** argv)
{
	if (argc != 3)
	{
        printf("%s <PORT> <echo|log|GMS>\n", argv[0]);
		return 1;
	}
    GMS::Data* data = new GMS::Data();
    if (0 == strcmp(argv[2], "url"))
	{
		data->dataString = "Fred was here, I think!";
	}
    startServer(atoi(argv[1]), argv[2], data);
    delete data;
    return 0;
}
