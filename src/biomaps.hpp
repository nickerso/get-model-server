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
     * Once the outputs for a model are specified we can compile it into an executable simulation. This needs
     * to be done prior to setting any variable values.
     * @param modelId The identifier for the model to compile.
     * @return A JSON object containing a return code of 0 on success, non-zero on failure.
     */
    std::string compileModel(const std::string& modelId);

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
     * Perform the actual simulation over the specified time interval.
     * @param modelId The indentifier for the model to simulate.
     * @param startTime The start time for the integration.
     * @param endTime The end time of the integration.
     * @param outputInterval The time step resolution at which output is desired.
     * @return A JSON structure containing the simluation results for the previously defined output variables.
     */
    std::string execute(const std::string& modelId, double startTime, double endTime, double outputInterval);
private:
    Biomaps();
    std::string mRepositoryRoot;
    std::map<std::string, CellmlSimulator*> mModels;
    std::map<std::string, std::vector<int> > mOutputMaps;
};

#endif // BIOMAPS_HPP
