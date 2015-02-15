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
     * Serialise the current source file back out to its location that we already know.
     * @return A JSON object defining a returnCode which will be non-zero on error.
     */
    std::string saveSource() const;

    /**
     * Return the list of CellML components in the current source document.
     * @return A JSON object defining the current CellML components in the source model.
     */
    std::string getSourceComponents() const;

    /**
     * This method will be called in response to data being POST'ed to the annotator URL.
     * @param jsonData The JSON object POST'ed.
     * @return A JSON object with a returnCode of zero on success.
     */
    std::string handlePostData(const std::string& jsonData);

private:
    Annotator();
    std::string mRepositoryRoot;
    std::string mRepositoryLocalPath;
    std::string mSourceFile;
    XmlDoc* mSourceDocument;
};

#endif // ANNOTATOR_HPP
