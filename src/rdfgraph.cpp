#include <cstdio>
#include <ctype.h>
#include <string>
#include <cstdarg>
#include <cstdlib>
#include <iostream>

#include <redland.h>

#include "rdfgraph.hpp"

static const int OutputRdfGraph = 0;

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
