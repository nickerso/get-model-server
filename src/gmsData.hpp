#ifndef GMSDATA_HPP
#define GMSDATA_HPP

#include <vector>
#include <map>

class Workspace;
class RdfGraph;

namespace GMS
{
    class Data
    {
    public:
        Data();
        ~Data();
        /**
          * Initialise the database of all models we know about.
          */
        int initialiseModelDatabase(const std::string& repositoryRoot);

        int addWorkspace(Workspace* workspace);

        /**
          * Serialise the list of workspaces to JSON.
          * FIXME: might want to have a different method/arg for a summary listing
          *        versus a complete listing?
          */
        std::string serialiseWorkspaceListing() const;

        /**
          * Serialise the list of all models in the RDF graph for this data. This will serialise
          * models for all the types that we know about.
          */
        std::string serialiseModelListing() const;

        /**
          * Serialise the requested model ID.
          */
        std::string serialiseModel(const std::string& modelID) const;

    private:
        std::string mRepositoryRoot;
        std::vector<std::string> mModelList;
        std::map<std::string, Workspace*> mWorkspaces;
        RdfGraph* mRdfGraph;
    };
}
#endif // GMSDATA_HPP
