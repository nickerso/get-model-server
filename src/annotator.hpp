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

    /**
     * Return the list of CellML components in the current source document.
     * @return A JSON object defining the current CellML components in the source model.
     */
    std::string getSourceComponents() const;

private:
    Annotator();
    std::string mRepositoryRoot;
    std::string mRepositoryLocalPath;
    std::string mSourceFile;
    XmlDoc* mSourceDocument;
};

#endif // ANNOTATOR_HPP
