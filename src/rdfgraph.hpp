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
      * Looks for a dcterms:title for the given resource and returns it. If a dcterms:title
      * is not found, returns the string "untitled".
      */
    std::string getResourceTitle(const std::string& uri);

    /**
      * Looks for a bqbio:property for the given resource and returns it. If a bqbio:property
      * is not found, returns the string "unknown".
      */
    std::string getResourceType(const std::string& uri);

    /**
      * Looks for a csim:image for the given resource and returns it. Expected to be a URL to
      * the image.
      */
    std::string getResourceImageUrl(const std::string &uri);

    /**
      * Looks for a csim:sed for the given resource and returns it. Expected to be a URL to
      * the SED-ML document.
      */
    std::string getResourceSedUrl(const std::string &uri);

    /**
      * Trying to work around thread-safety in Redland, need to re-create RDF graph each time we want to use it?
      */
    void cacheGraph();

    /**
      * Returns a list of all the protocols for the given resource.
      */
    std::vector<std::string> getResourceProtocolUris(const std::string& uri);

    /**
     * Return the current cached graph as a string.
     * @return The RDF/XML string representation of the current cached graph.
     */
    inline const std::string& getGraphCache() const
    {
        return mGraphCache;
    }

    /**
     * Return the object URI for each annotation consisting of the given resource and qulaifier.
     * @param resourceUri The resource URI to query for.
     * @param qualifierUri The predicate URI.
     * @return A vector of URIs as strings.
     */
    std::vector<std::string> getAnnotationsForResource(const std::string &resourceUri,
                                                       const std::string &qualifierUri);

private:
    RedlandContainer* mRedlandContainer;
    std::string mGraphCache;
};

#endif // RDFGRAPH_HPP
