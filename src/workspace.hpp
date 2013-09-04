#ifndef WORKSPACE_HPP
#define WORKSPACE_HPP

class ContentItem;
class RdfGraph;

class Workspace
{
public:
    Workspace(const std::string& url, const std::string& id);
    ~Workspace();

    // load any metadata from this workspace into the given RDF graph
    int loadMetadata(RdfGraph* rdfGraph) const;

    const char* getUrl() const;
    const char* getId() const;
private:
    std::string mUrl;
    std::string mId;
    std::vector<ContentItem*> mContentItems;
};

#endif // WORKSPACE_HPP
