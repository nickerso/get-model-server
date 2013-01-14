#ifndef GMSAPI_HPP
#define GMSAPI_HPP

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
