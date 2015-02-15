#ifndef ANNOTATOR_HPP
#define ANNOTATOR_HPP

class XmlDoc;
class Annotator
{
public:
    Annotator(const std::string& repositoryRoot, const std::string& repositoryLocalPath);
    ~Annotator();

    /**
     * Loads the given XML source document for annotation. We currently only support having a single source
     * document loaded at any given time.
     * @param url The URL of the document to load, relative to the known repository root.
     * @return A JSON object defining a returnCode which will be non-zero on error.
     */
    std::string loadSource(const std::string& url);

private:
    Annotator();
    std::string mRepositoryRoot;
    std::string mRepositoryLocalPath;
    XmlDoc* mSourceDocument;
};

#endif // ANNOTATOR_HPP
