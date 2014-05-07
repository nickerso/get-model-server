#ifndef BIOMAPS_HPP
#define BIOMAPS_HPP

class Biomaps
{
public:
    Biomaps(const std::string& repositoryRoot);
    ~Biomaps();

    /**
     * Loads the given model into this manager object. The returned response will contain the unique
     * identifer to use in future access to this instance of the model.
     * @param url The URL of the model to load, relative to the known repository root.
     * @return A JSON object defining the identifier to use when needing access to this model in future calls.
     */
    std::string loadModel(const std::string& url);

private:
    Biomaps();
    std::string mRepositoryRoot;
};

#endif // BIOMAPS_HPP
