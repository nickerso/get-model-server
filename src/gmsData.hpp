#ifndef GMSDATA_HPP
#define GMSDATA_HPP

#include <vector>

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
    private:
        std::string mRepositoryRoot;
        std::vector<std::string> mModelList;
        // want a persistent connection to re-use
        void* mCurl;
    };
}
#endif // GMSDATA_HPP
