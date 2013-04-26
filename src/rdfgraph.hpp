#ifndef RDFGRAPH_HPP
#define RDFGRAPH_HPP

class RedlandContainer;

class RdfGraph
{
public:
    RdfGraph();
    ~RdfGraph();

    // parse the given RDF/XML URL into this graph
    int parseRdfXmlUrl(const std::string& url);

    std::vector<std::string> getModelsOfType(const std::string& typeUri);

    /**
      * Looks for a dcterms:title for the give resource and returns it. If a dcterms:title
      * is not found, returns the string "untitled".
      */
    std::string getResourceTitle(const std::string& uri);

private:
    RedlandContainer* mRedlandContainer;
};

#endif // RDFGRAPH_HPP
