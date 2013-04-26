#include <cstdio>
#include <ctype.h>
#include <string>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <redland.h>

#include "rdfgraph.hpp"

static const int OutputRdfGraph = 0;

static void printNodeType(librdf_node* node)
{
    std::cout << "Given node is of type: ";
    if (librdf_node_is_resource(node)) std::cout << "rdf:Resource (& rdf:Property) - has a URI";
    else if (librdf_node_is_literal(node)) std::cout << "rdf:Literal - has an XML string, language, XML space";
    else if (librdf_node_is_blank(node)) std::cout << "blank node has an identifier string";
    else std::cout << "???UNKNOWN TYPE???";
    std::cout << std::endl;
}

// we use the redland container object to hide all the Redland code from the outside world.
class RedlandContainer
{
public:
    RedlandContainer()
    {
        std::cout << "Creating a new RedlandContainer and its own world" << std::endl;
        world = librdf_new_world();
        librdf_world_open(world);
        storage = librdf_new_storage(world, "hashes", NULL, "hash-type='memory'");
        model = librdf_new_model(world, storage, NULL);
    }
    ~RedlandContainer()
    {
        std::cout << "Destroying the RedlandContainer and free'ing its world" << std::endl;
        if (OutputRdfGraph)
        {
            raptor_world* raptor_world_ptr = librdf_world_get_raptor(world);
            raptor_iostream* iostr = raptor_new_iostream_to_file_handle(raptor_world_ptr, stdout);
            librdf_model_write(model, iostr);
            raptor_free_iostream(iostr);
        }
        librdf_free_model(model);
        librdf_free_storage(storage);
        librdf_free_world(world);
    }
    librdf_world* world;
    librdf_storage* storage;
    librdf_model* model;
};

RdfGraph::RdfGraph()
{
    std::cout << "Creating a new RdfGraph and its redland container" << std::endl;
    mRedlandContainer = new RedlandContainer();
}

RdfGraph::~RdfGraph()
{
    std::cout << "Destroying the RdfGraph and free'ing its world" << std::endl;
    delete mRedlandContainer;
}

int RdfGraph::parseRdfXmlUrl(const std::string &url)
{
    librdf_uri* uri = librdf_new_uri(mRedlandContainer->world, (const unsigned char*)(url.c_str()));
    librdf_parser* parser = librdf_new_parser(mRedlandContainer->world, "rdfxml", NULL, NULL);
    librdf_parser_parse_into_model(parser, uri, NULL, mRedlandContainer->model);
    librdf_free_parser(parser);
    librdf_free_uri(uri);
    return 0;
}


std::vector<std::string> RdfGraph::getModelsOfType(const std::string& typeUri)
{
    std::vector<std::string> r;
    std::string queryString = "select * where { ?s ";
    queryString += "<http://biomodels.net/biology-qualifiers/property> <";
    queryString += typeUri;
    queryString += ">}";
    std::cout << "query string: " << queryString.c_str() << std::endl;
    librdf_query* query = librdf_new_query(mRedlandContainer->world, "sparql", NULL, (const unsigned char*)(queryString.c_str()), NULL);
    librdf_query_results* results = librdf_model_query_execute(mRedlandContainer->model, query);
    if (results && librdf_query_results_is_bindings(results))
    {
        while (!librdf_query_results_finished(results))
        {
            std::cout << "bobby!" << std::endl;
            librdf_node* node = librdf_query_results_get_binding_value_by_name(results, "s");
            librdf_uri* uri = librdf_node_get_uri(node);
            librdf_free_node(node);
            r.push_back(std::string((char*)(librdf_uri_as_string(uri))));
            librdf_query_results_next(results);
        }
    }
    librdf_free_query_results(results);
    librdf_free_query(query);
    return r;
}

std::string RdfGraph::getResourceTitle(const std::string &uri)
{
    std::string title = "untitled";
    std::string queryString = "select * where { <";
    queryString += uri;
    queryString += "> <http://purl.org/dc/terms/title> ?t }";
    std::cout << "query string: " << queryString.c_str() << std::endl;
    librdf_query* query = librdf_new_query(mRedlandContainer->world, "sparql", NULL, (const unsigned char*)(queryString.c_str()), NULL);
    librdf_query_results* results = librdf_model_query_execute(mRedlandContainer->model, query);
    if (results && librdf_query_results_is_bindings(results))
    {
        // protection from there being no results.
        if (!librdf_query_results_finished(results))
        {
            librdf_node* node = librdf_query_results_get_binding_value_by_name(results, "t");
            if (0) printNodeType(node);
            char* s = librdf_node_get_literal_value_as_latin1(node);
            librdf_free_node(node);
            title = std::string(s);
        }
    }
    librdf_free_query_results(results);
    librdf_free_query(query);
    return title;
}
