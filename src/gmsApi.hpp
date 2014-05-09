#ifndef GMSAPI_HPP
#define GMSAPI_HPP

/**
  * GMS webservice ideas.
  *
  * /model/ - returns listing of all available models (model == workspace)
  * /model/id - returns basic information about the model
  * /model/id/math - maybe return a presentation of the model math?
  *
  * /model/id/sed - return a listing of all SED's for the model (maybe part of the basic information above?)
  * /model/id/sed/output/id - return the specified output (chart, report, etc)
  *
  *
  * /query | /search - perform requested search on the database and return results as above?
  * e.g., /query?species=rat
  *       /query?species=rat&organ=kidney
  */

namespace GMS
{
    class Data;

    class API
    {
    public:
        API();
        ~API();
        std::string executeAPI(const std::string& url, const std::map<std::string, std::string>& argvals,
                GMS::Data* data);
    private:
        void getInvalidResponse(std::string& response);
        std::string handleModelRequest(const std::string& url, const std::map<std::string, std::string>& argvals,
                                       GMS::Data* data);
        std::string handleBiomapsRequest(const std::string& url, const std::map<std::string, std::string>& argvals,
                                         GMS::Data* data);
        std::string handleQueryRequest(const std::string& url, const std::map<std::string, std::string>& argvals,
                                       GMS::Data* data);
        std::string handleDojoTestRequest(const std::string& url, const std::map<std::string, std::string>& argvals);

        // these are the root URL's that we support
        static const std::string URL_MODEL;
        static const std::string URL_SEARCH;
        static const std::string URL_QUERY;
        static const std::string URL_DOJO_TEST; // provides some test JSON data from Dojo examples
        static const std::string URL_BIOMAPS;
    };
}
#endif // GMSAPI_HPP
