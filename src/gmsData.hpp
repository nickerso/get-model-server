#ifndef GMSDATA_HPP
#define GMSDATA_HPP

#include <vector>
#include <map>
#include <sedml/SedTypes.h>

class Workspace;
class RdfGraph;

LIBSEDML_CPP_NAMESPACE_USE

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
          * Serialise the requested model ID. DEPRECATED
          */
        std::string serialiseModel(const std::string& id);

        /**
          * Perform the specified action on the given model.
          */
        std::string performModelAction(const std::string& modelId, const std::string& action);

        /**
          * Serialise all models of the given type. This is the primary method for models to get into the web application.
          */
        std::string serialiseModelsOfType(const std::string& modelType);

        /**
          * Returns the model ID for the model with the specified URI.
          */
        std::string& mapModelUri(const std::string& uri);

        /**
          * Returns the model URI for the given model ID. Empty string if given ID is not found.
          */
        std::string mapModelId(const std::string& id);

        /**
          * Returns the Simulation Description document for the given URI, if possible. The SED-ML
          * document will be loaded if not already parsed.
          */
        SedDocument* mapUriToSed(const std::string& uri);

    private:
        std::string mRepositoryRoot;
        std::vector<std::string> mModelList;
        std::map<std::string, Workspace*> mWorkspaces;
        std::map<std::string, std::string> mModelIdMap; // ID -> URI map
        std::map<std::string, std::string> mModelUriMap; // URI -> ID map
        std::map<std::string, SedDocument*> mSimulationDescriptions;
        RdfGraph* mRdfGraph;
    };
}
#endif // GMSDATA_HPP
