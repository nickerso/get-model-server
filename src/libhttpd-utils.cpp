#include <microhttpd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <map>

#include "gmsData.hpp"
#include "gmsApi.hpp"

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo</body></html>"

static int ahc_echo(void * cls, struct MHD_Connection * connection,
        const char * url, const char * method, const char * version,
        const char * upload_data, size_t * upload_data_size, void ** ptr)
{
    static int dummy;
    const char * page = (char*) cls;
    struct MHD_Response * response;
    int ret;

    if (0 != strcmp(method, "GET"))
        return MHD_NO; /* unexpected method */
    if (&dummy != *ptr)
    {
        /* The first time only the headers are valid,
         do not respond in the first round... */
        *ptr = &dummy;
        return MHD_YES;
    }
    if (0 != *upload_data_size)
        return MHD_NO; /* upload data in a GET!? */
    *ptr = NULL; /* clear context pointer */
    printf("url: '%s'\n", url);
    response = MHD_create_response_from_data(strlen(page), (void*) page, MHD_NO,
            MHD_NO);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

int print_out_key(void *cls, enum MHD_ValueKind kind, const char *key,
        const char *value)
{
    printf("%s: %s\n", key, value);
    return MHD_YES;
}

static int answer_to_connection(void *cls, struct MHD_Connection *connection,
        const char *url, const char *method, const char *version,
        const char *upload_data, size_t *upload_data_size, void **con_cls)
{
    printf("New %s request for %s using version %s\n", method, url, version);
    printf("\nMHD_RESPONSE_HEADER_KIND:\n");
    MHD_get_connection_values(connection, MHD_RESPONSE_HEADER_KIND,
            &print_out_key, NULL);
    printf("\nMHD_HEADER_KIND:\n");
    MHD_get_connection_values(connection, MHD_HEADER_KIND, &print_out_key,
            NULL);
    printf("\nMHD_COOKIE_KIND:\n");
    MHD_get_connection_values(connection, MHD_COOKIE_KIND, &print_out_key,
            NULL);
    printf("\nMHD_POSTDATA_KIND:\n");
    MHD_get_connection_values(connection, MHD_POSTDATA_KIND, &print_out_key,
            NULL);
    printf("\nMHD_GET_ARGUMENT_KIND:\n");
    MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, &print_out_key,
            NULL);
    printf("\nMHD_FOOTER_KIND:\n");
    MHD_get_connection_values(connection, MHD_FOOTER_KIND, &print_out_key,
            NULL);
    return MHD_NO;
}

#define ERROR_PAGE "<html><head><title>Error</title></head><body>Bad data</body></html>"

static int send_bad_response(struct MHD_Connection *connection)
{
    static char *bad_response = (char *) ERROR_PAGE;
    int bad_response_len = strlen(bad_response);
    int ret;
    struct MHD_Response *response;

    response = MHD_create_response_from_buffer(bad_response_len, bad_response,
            MHD_RESPMEM_PERSISTENT);
    if (response == 0)
    {
        return MHD_NO;
    }
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

/*
 *The Cross-Origin Resource Sharing standard works by adding new HTTP headers
 *that allow servers to describe the set of origins that are permitted to read
 *that information using a web browser. Additionally, for HTTP request methods
 *that can cause side-effects on user data (in particular, for HTTP methods
 *other than GET, or for POST usage with certain MIME types), the specification
 *mandates that browsers "preflight" the request, soliciting supported methods
 *from the server with an HTTP OPTIONS request header, and then, upon
 *"approval" from the server, sending the actual request with the actual HTTP
 *request method. Servers can also notify clients whether "credentials"
 *(including Cookies and HTTP Authentication data) should be sent with
 *requests.
 */
static int sendAccessControl(struct MHD_Connection *connection, const char *url,
        const char *method, const char *version)
{
    int ret;
    struct MHD_Response *response;

    std::cout << "Sending CORS accept header for the request: " << std::endl;

    /*answer_to_connection(NULL, connection, url, method, version, NULL, NULL,
            NULL);*/

    response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    if (response == 0)
    {
        return MHD_NO;
    }
    // not too fussed with who is trying to use us :)
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    // only allow GET (and OPTIONS) requests, no need for PUSH yet...
    MHD_add_response_header(response, "Access-Control-Allow-Methods",
            "GET, OPTIONS");
    // we simply 'allow' all requested headers
    const char* val = MHD_lookup_connection_value(connection, MHD_HEADER_KIND,
            "Access-Control-Request-Headers");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", val);
    // these seem to be needed?
    MHD_add_response_header(response, "Access-Control-Expose-Headers",
            "Content-Range");

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

static int get_url_args(void *cls, MHD_ValueKind kind, const char *key,
        const char* value)
{
    std::map<std::string, std::string> * url_args = static_cast<std::map<std::string, std::string> *>(cls);
    if (url_args->find(key) == url_args->end())
    {
        if (!value)
            (*url_args)[key] = "";
        else
            (*url_args)[key] = value;
    }
    else
    {
        std::cerr << "Duplicate URL argument?\n";
    }
    return MHD_YES;
}

static int url_handler(void *cls, struct MHD_Connection *connection,
        const char *url, const char *method, const char *version,
        const char *upload_data, size_t *upload_data_size, void **ptr)
{
    static int aptr;
    GMS::Data* data = static_cast<GMS::Data*>(cls);
    char *me;
    struct MHD_Response *response;
    int ret;
    std::map<std::string, std::string> url_args;
    GMS::API api;
    std::string respdata;

    // HTTP access control (CORS)
    // https://developer.mozilla.org/en-US/docs/HTTP/Access_control_CORS?redirectlocale=en-US&redirectslug=HTTP_access_control
    // some times a preflight check is required which uses the OPTIONS HTTP method to check for permission to
    // call cross-domain requests
    if (0 == strcmp(method, MHD_HTTP_METHOD_OPTIONS))
        return sendAccessControl(connection, url, method, version);

    // we only need to deal with GET requests
    if (0 != strcmp(method, MHD_HTTP_METHOD_GET))
        return MHD_NO;

    if (&aptr != *ptr)
    {
        *ptr = &aptr;
        return MHD_YES;
    }

    if (MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND,
            get_url_args, &url_args) < 0)
    {
        return send_bad_response(connection);
    }

    std::cout << "data->dataString: " << data->dataString.c_str() << std::endl;
    api.executeAPI(url, url_args, respdata);

    *ptr = 0; /* reset when done */
    //val = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "q");
    me = (char *) malloc(respdata.size() + 1);
    if (me == 0)
        return MHD_NO;
    strncpy(me, respdata.c_str(), respdata.size() + 1);
    response = MHD_create_response_from_buffer(strlen(me), me,
            MHD_RESPMEM_MUST_FREE);
    if (response == 0)
    {
        free(me);
        return MHD_NO;
    }

    /*it = url_args.find("type");
     if (it != url_args.end() && strcasecmp(it->second.c_str(), "xml") == 0)
     type = typexml;*/

    MHD_add_response_header(response, "Content-Type", "application/json");
    MHD_add_response_header(response, "Content-Range", "items 0-5/5");
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    // need to make sure we always expose the (non-simple) headers that we use
    MHD_add_response_header(response, "Access-Control-Expose-Headers",
            "Content-Range");
    //MHD_add_response_header(response, "OurHeader", type);

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

int startServer(int port, const char* serverType, GMS::Data* data)
{
    struct MHD_Daemon * d;

    if (0 == strcmp(serverType, "echo"))
    {
        d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, port, NULL,
                NULL, &ahc_echo, (void*) PAGE, MHD_OPTION_END);
    }
    else if (0 == strcmp(serverType, "log"))
    {
        d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, port, NULL,
                NULL, &answer_to_connection, (void*) PAGE, MHD_OPTION_END);
    }
    else if (0 == strcmp(serverType, "GMS"))
    {
        d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, port, NULL,
                NULL, &url_handler, static_cast<void*>(data), MHD_OPTION_END);
    }
    if (d == NULL)
        return 1;
    (void) getchar();
    MHD_stop_daemon(d);
    return 0;
}

