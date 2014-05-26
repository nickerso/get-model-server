#ifndef BIOMAPS_HPP
#define BIOMAPS_HPP

#include <map>

class CellmlSimulator;

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

    /**
     * Flag the given variable in the specified model as a desired output. Only variables exposed at the
     * top level of a CellML model can be flagged.
     * @param modelId The identifier of the model to be used.
     * @param componentName The CellML component name containing the variable to be flagged.
     * @param variableName The name of the CellML variable to be flagged.
     * @param columnIndex The index column to store this variable's value in the output array. First column is 0.
     * @return A JSON object containing a return code of 0 (always succeeds even if it shouldn't).
     */
    std::string flagOutput(const std::string& modelId, const std::string& componentName,
                           const std::string& variableName, int columnIndex);

    /**
     * Save the current state of the model as a checkpoint that we can come back to.
     * @param modelId The identifier of the model to checkpoint.
     * @return A JSON object containing a return code of 0 on success.
     */
    std::string saveModelCheckpoint(const std::string& modelId);

    /**
     * Reset the current state of the model back to the state previously saved as a checkpoint.
     * @param modelId The identifier of the model to reset.
     * @return A JSON object containing a return code of 0 on success.
     */
    std::string loadModelCheckpoint(const std::string& modelId);

    /**
     * Set the value of the given variable in the specified model. Only variables exposed at the
     * top level of a CellML model can be set in this manner.
     * @param modelId The identifier of the model to be used.
     * @param componentName The CellML component name containing the variable to be flagged.
     * @param variableName The name of the CellML variable to be flagged.
     * @param value The value to set this variable to.
     * @return A JSON object containing a return code of 0 on success.
     */
    std::string setVariableValue(const std::string& modelId, const std::string& componentName,
                                 const std::string& variableName, double value);

    /**
     * Set the value of the given variable in the specified model. Only variables exposed at the
     * top level of a CellML model can be set in this manner.
     * @param modelId The identifier of the model to be used.
     * @param componentName The CellML component name containing the variable to be flagged.
     * @param variableName The name of the CellML variable to be flagged.
     * @param datasetId The ID of the dataset to use to control the value of this variable.
     * @param offsetDataTime If true, the dataset times will be offset by the start time when
     *   executing a simulation.
     * @return A JSON object containing a return code of 0 on success.
     */
    std::string setVariableValue(const std::string& modelId, const std::string& componentName,
                                 const std::string& variableName, const std::string& datasetId,
                                 bool offsetDataTime);

    /**
     * Perform the actual simulation over the specified time interval. No resetting is done, the simulation
     * will simply continue from its current state.
     * @param modelId The indentifier for the model to simulate.
     * @param startTime The start time for the integration.
     * @param endTime The end time of the integration.
     * @param outputInterval The time step resolution at which output is desired.
     * @return A JSON structure containing the simluation results for the previously defined output variables.
     */
    std::string execute(const std::string& modelId, double startTime, double endTime, double outputInterval);

    /**
     * Create a new unique identifier for a dataset that will be used as a parameter value when running
     * a simulation.
     * @return The new identifier, the calling code is expected to free the memory returned.
     */
    char* createDatasetId() const;

    /**
     * Populate the content of the given dataset from a JSON string.
     * @param id The identity of the dataset to set the content of.
     * @param jsonData A string representation of the JSON data defining this dataset.
     * @return zero on success.
     */
    int setDatasetContent(const std::string& id, const std::string& jsonData);
private:
    Biomaps();
    /**
     * Set the values for the variables controlled via a dataset.
     * @param modelId The identifier for the model of interest.
     * @param time The current time to set the value for.
     */
    void setDataDrivenValues(const std::string &modelId, double time);
    /**
     * Get a value from the given dataset at the specified time.
     * @todo Need to allow for different types of interpolation and offsets.
     * @param datasetId The identity of the dataset to obtain the value from.
     * @param time The time at which the value of the dataset is desired.
     * @return The requested value.
     */
    double getDataValue(const std::string& datasetId, double time) const;
    std::string mRepositoryRoot;
    std::map<std::string, CellmlSimulator*> mModels;
    std::map<std::string, std::vector<int> > mOutputMaps;
    std::map<std::string, std::vector<std::pair<double, double> > > mDatasets;
    // MAP[ <modelID> ] ==> vector( <variableID> <datasetID> )
    std::map<std::string, std::vector<std::pair<std::string, std::string> > > mDatasetVariableMap;
    // MAP[ <modelID> ] ==> vector( <variableID> <offsetDataTime> )
    std::map<std::string, std::vector<std::pair<std::string, bool> > > mDatasetTimeOffset;
};

#endif // BIOMAPS_HPP
