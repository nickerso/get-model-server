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

    private:
        std::string mRepositoryRoot;
        std::vector<std::string> mModelList;
        std::map<std::string, Workspace*> mWorkspaces;
        RdfGraph* mRdfGraph;
    };
}
#endif // GMSDATA_HPP
