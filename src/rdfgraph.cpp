#include <cstdio>
#include <ctype.h>
#include <string>
#include <cstdarg>
#include <cstdlib>
#include <iostream>

#include <redland.h>

#include "rdfgraph.hpp"

RdfGraph::RdfGraph()
{
    std::cout << "Creating a new RdfGraph and its own world" << std::endl;
    mWorld = static_cast<void*>(librdf_new_world());
    librdf_world_open(static_cast<librdf_world*>(mWorld));
}

RdfGraph::~RdfGraph()
{
    std::cout << "Destroying the RdfGraph and free'ing its world" << std::endl;
    librdf_free_world(static_cast<librdf_world*>(mWorld));
}
