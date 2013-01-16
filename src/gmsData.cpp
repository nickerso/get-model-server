#include <iostream>
#include <vector>
#include <algorithm>
#include <sedml/reader.h>

#include "gmsData.hpp"
#include "workspace.hpp"
#include "utils.hpp"

using namespace GMS;

class WorkspaceLoader
{
public:
    WorkspaceLoader(Data* dataHandle, const std::string& repositoryRoot) : data(dataHandle), root(repositoryRoot)
    {
    }
    // used with for_each
    void operator()(const std::string& workspaceName)
    {
        std::string url = root + workspaceName;
        std::cout << "Loading workspace: \'" << url.c_str() << "\'\n";
        Workspace* workspace = new Workspace(url);
        data->addWorkspace(workspace);
    }

    Data* data;
    const std::string& root;
};

Data::Data()
{
    std::cout << "Creating new GMS::Data for use in the GET model server." << std::endl;
}

Data::~Data()
{
    std::cout << "Destroying the GMS::Data from the GET model server." << std::endl;
}

int Data::initialiseModelDatabase(const std::string &repositoryRoot)
{
    int code = 0;
    mRepositoryRoot = repositoryRoot;
    std::string workspacesUrl = repositoryRoot + "workspaces";
    std::string data = getUrlContent(workspacesUrl);
    mModelList = splitString(data, '\n', mModelList);
    WorkspaceLoader loader(this, mRepositoryRoot);
    for_each(mModelList.begin(), mModelList.end(), loader);

    struct sedml_document* doc = sedml_create_document();
    int r = sedml_read_file("http://models.cellml.org/w/andre/sine/rawfile/293afb20feb51d1739b6645eaf2cd18b1a4f3bcb/sin_approximations_sedml.xml", NULL, doc);
    if (!r)
    {
        std::cout << "r is zero!\n";
        if (doc->sedml)
        {
            std::cout << "Found a SED-ML document with level: " << doc->sedml->level << "; version: "
                      << doc->sedml->version << std::endl;
        }
        else
        {
            std::cout << "no sedml document?\n";
        }
    }
    else
    {
        std::cout << "r is not zero! " << r << std::endl;
    }
    /*std::cout << "Model Listing:\n";
    for (int i = 0; i < mModelList.size(); ++i)
    {
        std::cout << "**" << mModelList[i] << "**\n";
    }*/
    return code;
}

int Data::addWorkspace(Workspace *workspace)
{
    mWorkspaces.push_back(workspace);
    return 0;
}
