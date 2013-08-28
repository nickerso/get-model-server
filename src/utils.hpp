#ifndef UTILS_HPP
#define UTILS_HPP

#include <json/json-forwards.h>
#include <sedml/SedTypes.h>

LIBSEDML_CPP_NAMESPACE_USE

std::string getUrlContent(const std::string& url);

std::vector<std::string>& splitString(const std::string &s, char delim, std::vector<std::string>& elems);

/**
  * Check if the given URL falls under the given base; if so then return the 'relative' portion of the URL.
  */
std::string urlChildOf(const std::string& url, const std::string& base);

/**
  * Serialise the outputs for the given Sed-ML document in JSON format.
  */
Json::Value getSedOutputsJson(SedDocument* doc);

#endif // UTILS_HPP
