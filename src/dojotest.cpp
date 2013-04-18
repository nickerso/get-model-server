#include <iostream>
#include <vector>
#include <algorithm>
#include <sedml/reader.h>
#include <json/json.h>
#include <json/json-forwards.h>

#include "dojotest.hpp"

DojoTest::DojoTest()
{
}


std::string DojoTest::handleRequest(const std::string &url, const std::map<std::string, std::string>& argvals)
{
    std::cout << "Handling Dojo test request" << std::endl;
    std::string response = "BAD RESPONSE";
    if (url.find("/root") != std::string::npos)
    {
        Json::Value root;
        root["name"] = "US Gov";
        root["id"] = "root";
        Json::Value c;
        c["name"] = "Cong";
        c["id"] = "congress";
        c["children"] = true;
        root["children"].append(c);
        c["name"] = "Exec";
        c["id"] = "exec";
        c["children"] = true;
        root["children"].append(c);
        c["name"] = "Judicial";
        c["id"] = "judicial";
        c.removeMember("children");
        root["children"].append(c);
        response = Json::FastWriter().write(root);
    }
    else if (url.find("/congress") != std::string::npos)
    {
        Json::Value root;
        root["name"] = "Congress";
        root["id"] = "congress";
        Json::Value c;
        c["name"] = "House of Reps";
        c["id"] = "houseOfReps";
        root["children"].append(c);
        c["name"] = "Senate";
        c["id"] = "senate";
        root["children"].append(c);
        response = Json::FastWriter().write(root);
    }
    else if (url.find("/exec") != std::string::npos)
    {
        Json::Value root;
        root["name"] = "Executive";
        root["id"] = "exec";
        Json::Value c;
        c["name"] = "President";
        c["id"] = "pres";
        root["children"].append(c);
        c["name"] = "Vice President";
        c["id"] = "vice-pres";
        root["children"].append(c);
        c["name"] = "Secretary of State";
        c["id"] = "state";
        root["children"].append(c);
        c["name"] = "Cabinet";
        c["id"] = "cabinet";
        c["children"] = true;
        root["children"].append(c);
        response = Json::FastWriter().write(root);
    }
    else if (url.find("/cabinet") != std::string::npos)
    {
        Json::Value root;
        root["name"] = "Cabinet";
        root["id"] = "cabinet";
        Json::Value c;
        c["name"] = "Bob";
        c["id"] = "bob";
        root["children"].append(c);
        c["name"] = "Fred";
        c["id"] = "fred";
        root["children"].append(c);
        response = Json::FastWriter().write(root);
    }
    else
    {
        std::cout << "DojoTest doh! url: " << url.c_str() << std::endl;
    }
    return response;
}
