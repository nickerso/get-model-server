#ifndef RDFGRAPH_HPP
#define RDFGRAPH_HPP

struct librdf_world_s;
typedef librdf_world_s librdf_world;

class RdfGraph
{
public:
    RdfGraph();
    ~RdfGraph();

private:
    librdf_world* mWorld;
};

#endif // RDFGRAPH_HPP
