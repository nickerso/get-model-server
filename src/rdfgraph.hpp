#ifndef RDFGRAPH_HPP
#define RDFGRAPH_HPP

class RedlandContainer;
struct librdf_model_s;

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
     * Create a new triple in the cached graph.
     * @param source The URI of the source for the triple.
     * @param predicate The URI of the predicate for the triple.
     * @param object The URI of the object of the triple.
     * @return zero on success.
     */
    int createTriple(const std::string& source, const std::string& predicate, const std::string& object);

    /**
     * Create and add the evidence annotations to our RDF graph in support of the defined statement.
     * @param source The URI of the source in the statement we are supporting.
     * @param predicate The URI of the predicate in the statement we are supporting.
     * @param object The URI of the object in the statement we are supporting.
     * @param evidenceQualifier The URI of the predicate to use in defining the evidence annotation.
     * @param evidenceUri The URI of the evidence object.
     * @return zero on success.
     */
    int addEvidenceToGraph(const std::string& source, const std::string& predicate, const std::string& object,
                           const std::string& evidenceQualifier, const std::string& evidenceUri);

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
    void cacheGraphFromModel(librdf_model_s* model);
};

#endif // RDFGRAPH_HPP
