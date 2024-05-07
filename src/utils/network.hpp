#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <map>
#include <string>

#define NO_PARAM std::map<std::string, std::string>()

enum class HTTPMethod {
    HEAD,
    GET,
};

struct HTTPResult {
    bool successful;
    int status;
    std::string body;
};

HTTPResult make_http_request(
        HTTPMethod method,
        std::string url,
        std::map<std::string, std::string> params={},
        bool follow_location=false);

#endif
