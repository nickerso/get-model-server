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
        // these are the URL's that we support
        static const char URL_MODEL[];
        static const char URL_EXECUTE[];
    };
}
#endif // GMSAPI_HPP
