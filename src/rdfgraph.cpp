#include <cstdio>
#include <ctype.h>
#include <cstring>
#include <string>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <redland.h>

#include "rdfgraph.hpp"
#include "namespaces.hpp"

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

static librdf_node* getTargetNode(librdf_world* world, librdf_model* model, librdf_node* subject,
                                  const std::string& predicate)
{
    librdf_node* arc = librdf_new_node_from_uri_string(world, (const unsigned char*)(predicate.c_str()));
    librdf_node* target = librdf_model_get_target(model, subject, arc);
    librdf_free_node(arc);
    return(target);
}

static std::string nodeGetURI(librdf_node* node)
{
    std::string uri;
    if (node)
    {
        if (librdf_node_is_resource(node))
        {
            char* u = (char*)librdf_uri_to_string(librdf_node_get_uri(node));
            uri = std::string(u);
            librdf_free_memory(u);
        }
    }
    return(uri);
}

static bool isNodeNill(librdf_node* node)
{
    bool code = false;
    if (node)
    {
        if (librdf_node_is_resource(node))
        {
            char* uri = (char*)librdf_uri_to_string(librdf_node_get_uri(node));
            if (uri)
            {
                if (strcmp(uri, RDF_NS "nill") == 0) code = true;
                free(uri);
            }
        }
    }
    return(code);
}

// we create objects of this type on demand to avoid thread issues ??
class RedlandGraph
{
public:
    RedlandGraph(librdf_world* world, const std::string& data)
    {
        std::cout << "Creating a new RDF graph" << std::endl;
        storage = librdf_new_storage(world, "hashes", NULL, "hash-type='memory'");
        model = librdf_new_model(world, storage, NULL);

        librdf_parser* parser = librdf_new_parser(world, "rdfxml", NULL, NULL);
        std::string dummyUri = "http://localhost:12345/bob/fred/";
        librdf_uri* uri = librdf_new_uri(world, (const unsigned char*)(dummyUri.c_str()));
        librdf_parser_parse_string_into_model(parser, (const unsigned char*)(data.c_str()), uri, model);
        librdf_free_parser(parser);
        librdf_free_uri(uri);

        /*std::cout << "Loading graph back into Redland:" << std::endl;
        raptor_world* raptor_world_ptr = librdf_world_get_raptor(world);
        raptor_iostream* iostr = raptor_new_iostream_to_file_handle(raptor_world_ptr, stdout);
        librdf_model_write(model, iostr);
        raptor_free_iostream(iostr);*/
    }

    ~RedlandGraph()
    {
        librdf_free_model(model);
        librdf_free_storage(storage);
    }

    librdf_model* model;
    librdf_storage* storage;
};

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
        if (OutputRdfGraph) outputGraph();
        if (model) librdf_free_model(model);
        if (storage) librdf_free_storage(storage);
        librdf_free_world(world);
    }

    void outputGraph()
    {
        raptor_world* raptor_world_ptr = librdf_world_get_raptor(world);
        raptor_iostream* iostr = raptor_new_iostream_to_file_handle(raptor_world_ptr, stdout);
        librdf_model_write(model, iostr);
        raptor_free_iostream(iostr);
    }

    std::string dumpGraph()
    {
        size_t n;
        unsigned char* s = librdf_model_to_counted_string(model, NULL, "rdfxml", NULL, NULL, &n);
        std::string graphString((char*)s, n);
        librdf_free_memory(s);
        return graphString;
    }

    librdf_world* world;
    // we can't use these as a global singleton as they are not thread safe?
    // FIXME: use them as an initial loader when first loading the workspaces, but thereafter we recreate the graph as needed.
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

void RdfGraph::cacheGraph()
{
    mRedlandContainer->outputGraph();
    // save the current graph into a RDF/XML string and free the model
    mGraphCache = mRedlandContainer->dumpGraph();
    std::cout << "Cached RDF graph: **@@" << mGraphCache.c_str() << "@@**" << std::endl;
}

std::vector<std::string> RdfGraph::getModelsOfType(const std::string& typeUri)
{
    RedlandGraph rdf(mRedlandContainer->world, mGraphCache);
    std::vector<std::string> r;
    std::string queryString = "select * where { ?s ";
    queryString += "<" BQBIO_NS "property> <";
    queryString += typeUri;
    queryString += ">}";
    std::cout << "query string: " << queryString.c_str() << std::endl;
    librdf_query* query = librdf_new_query(mRedlandContainer->world, "sparql", NULL, (const unsigned char*)(queryString.c_str()), NULL);
    librdf_query_results* results = librdf_model_query_execute(rdf.model, query);
    if (results)
    {
        if (librdf_query_results_is_bindings(results))
        {
            while (!librdf_query_results_finished(results))
            {
                //std::cout << "bobby!" << std::endl;
                librdf_node* node = librdf_query_results_get_binding_value_by_name(results, "s");
                librdf_uri* uri = librdf_node_get_uri(node);
                librdf_free_node(node);
                r.push_back(std::string((char*)(librdf_uri_as_string(uri))));
                librdf_query_results_next(results);
            }
        }
        librdf_free_query_results(results);
    }
    librdf_free_query(query);
    return r;
}

std::string RdfGraph::getResourceTitle(const std::string &uri)
{
    RedlandGraph rdf(mRedlandContainer->world, mGraphCache);
    std::string title = "untitled";
    std::string queryString = "select * where { <";
    queryString += uri;
    queryString += "> <" DCTERMS_NS "title> ?t }";
    std::cout << "query string: " << queryString.c_str() << std::endl;
    librdf_query* query = librdf_new_query(mRedlandContainer->world, "sparql", NULL, (const unsigned char*)(queryString.c_str()), NULL);
    librdf_query_results* results = librdf_model_query_execute(rdf.model, query);
    if (results)
    {
        if (librdf_query_results_is_bindings(results))
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
    }
    librdf_free_query(query);
    return title;
}

std::string RdfGraph::getResourceImageUrl(const std::string &uri)
{
    RedlandGraph rdf(mRedlandContainer->world, mGraphCache);
    std::string imageUrl = "";
    std::string queryString = "select * where { <";
    queryString += uri;
    queryString += "> <" CSIM_NS "image> ?image }";
    std::cout << "query string: " << queryString.c_str() << std::endl;
    librdf_query* query = librdf_new_query(mRedlandContainer->world, "sparql", NULL, (const unsigned char*)(queryString.c_str()), NULL);
    librdf_query_results* results = librdf_model_query_execute(rdf.model, query);
    if (results)
    {
        if (librdf_query_results_is_bindings(results))
        {
            // protection from there being no results.
            if (!librdf_query_results_finished(results))
            {
                librdf_node* node = librdf_query_results_get_binding_value_by_name(results, "image");
                if (0) printNodeType(node);
                librdf_uri* u = librdf_node_get_uri(node);
                librdf_free_node(node);
                imageUrl = std::string((char*)(librdf_uri_as_string(u)));
                std::cout << "image URL: " << imageUrl.c_str() << std::endl;
            }
        }
        librdf_free_query_results(results);
    }
    librdf_free_query(query);
    return imageUrl;
}

std::string RdfGraph::getResourceType(const std::string &uri)
{
    RedlandGraph rdf(mRedlandContainer->world, mGraphCache);
    std::string typeUri = "";
    std::string queryString = "select * where { <";
    queryString += uri;
    queryString += "> <" BQBIO_NS "property> ?type }";
    std::cout << "query string: " << queryString.c_str() << std::endl;
    librdf_query* query = librdf_new_query(mRedlandContainer->world, "sparql", NULL, (const unsigned char*)(queryString.c_str()), NULL);
    librdf_query_results* results = librdf_model_query_execute(rdf.model, query);
    if (results)
    {
        if (librdf_query_results_is_bindings(results))
        {
            // protection from there being no results.
            if (!librdf_query_results_finished(results))
            {
                librdf_node* node = librdf_query_results_get_binding_value_by_name(results, "type");
                if (0) printNodeType(node);
                librdf_uri* u = librdf_node_get_uri(node);
                librdf_free_node(node);
                typeUri = std::string((char*)(librdf_uri_as_string(u)));
                std::cout << "type URI: " << typeUri.c_str() << std::endl;
            }
        }
        librdf_free_query_results(results);
    }
    librdf_free_query(query);
    return typeUri;
}

std::string RdfGraph::getResourceSedUrl(const std::string &uri)
{
    RedlandGraph rdf(mRedlandContainer->world, mGraphCache);
    std::string sedUrl = "";
    std::string queryString = "select * where { <";
    queryString += uri;
    queryString += "> <" CSIM_NS "sed> ?sedml }";
    std::cout << "query string: " << queryString.c_str() << std::endl;
    librdf_query* query = librdf_new_query(mRedlandContainer->world, "sparql", NULL, (const unsigned char*)(queryString.c_str()), NULL);
    librdf_query_results* results = librdf_model_query_execute(rdf.model, query);
    if (results)
    {
        if (librdf_query_results_is_bindings(results))
        {
            // protection from there being no results.
            if (!librdf_query_results_finished(results))
            {
                librdf_node* node = librdf_query_results_get_binding_value_by_name(results, "sedml");
                if (0) printNodeType(node);
                librdf_uri* u = librdf_node_get_uri(node);
                librdf_free_node(node);
                sedUrl = std::string((char*)(librdf_uri_as_string(u)));
                std::cout << "SED-ML URL: " << sedUrl.c_str() << std::endl;
            }
        }
        librdf_free_query_results(results);
    }
    librdf_free_query(query);
    return sedUrl;
}

std::vector<std::string> RdfGraph::getResourceProtocolUris(const std::string &uri)
{
    RedlandGraph rdf(mRedlandContainer->world, mGraphCache);
    std::vector<std::string> uris;
    std::string queryString = "select * where { <";
    queryString += uri;
    queryString += "> <" CSIM_NS "protocols> ?protocols }";
    std::cout << "query string: " << queryString.c_str() << std::endl;
    librdf_query* query = librdf_new_query(mRedlandContainer->world, "sparql", NULL, (const unsigned char*)(queryString.c_str()), NULL);
    librdf_query_results* results = librdf_model_query_execute(rdf.model, query);
    if (results)
    {
        if (librdf_query_results_is_bindings(results))
        {
            // protection from there being no results.
            if (!librdf_query_results_finished(results))
            {
                librdf_node* collection = librdf_query_results_get_binding_value_by_name(results, "protocols");
                if (0) printNodeType(collection);
                librdf_node* theRest = collection;
                // iterate through the collection
                while (true)
                {
                    if (isNodeNill(theRest))
                    {
                        librdf_free_node(theRest);
                        break;
                    }
                    librdf_node* first = getTargetNode(mRedlandContainer->world, rdf.model,
                                                       theRest, RDF_NS "first");
                    if (first)
                    {
                        std::string urip = nodeGetURI(first);
                        librdf_free_node(first);
                        std::cout << "protocol URI: " << urip.c_str() << std::endl;
                        uris.push_back(urip);

                        librdf_node* rest = getTargetNode(mRedlandContainer->world, rdf.model,
                                                          theRest, RDF_NS "rest");
                        librdf_free_node(theRest);
                        if (rest) theRest = rest;
                        else break;
                    }
                    else
                    {
                        librdf_free_node(theRest);
                        break;
                    }
                }
            }
        }
        librdf_free_query_results(results);
    }
    librdf_free_query(query);
    return uris;
}
