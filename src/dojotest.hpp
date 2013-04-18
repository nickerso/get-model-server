#ifndef DOJOTEST_HPP
#define DOJOTEST_HPP

class DojoTest
{
public:
    DojoTest();

    std::string handleRequest(const std::string &url, const std::map<std::string, std::string>& argvals);
};

#endif // DOJOTEST_HPP
