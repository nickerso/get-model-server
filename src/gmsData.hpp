#ifndef GMSDATA_HPP
#define GMSDATA_HPP

#include <vector>

class Workspace;

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
    private:
        std::string mRepositoryRoot;
        std::vector<std::string> mModelList;
        std::vector<Workspace*> mWorkspaces;
    };
}
#endif // GMSDATA_HPP
