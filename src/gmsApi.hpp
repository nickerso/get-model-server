#ifndef GMSAPI_HPP
#define GMSAPI_HPP

namespace GMS
{
    class API
    {
    public:
        API();
        ~API();
        bool executeAPI(const std::string& url, const std::map<std::string, std::string>& argvals,
                std::string& response);
    private:
        void getInvalidResponse(std::string& response);
        // these are the URL's that we support
        static const char URL_MODEL[];
        static const char URL_EXECUTE[];
    };
}
#endif // GMSAPI_HPP
