#include "network.hpp"

#include "../utils/string.hpp"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../extlib/httplib.h"
#include "../extlib/logger.hpp"

Logger* network_log = Logger::init_logging();


httplib::Headers BASIC_HTTP_HEADERS = {
    {"User-Agent", "QtMusicPlayer/1.0.0"},
    {"Accept", "application/json"}
};

HTTPResult
make_http_request(HTTPMethod method,
                  std::string url,
                  std::map<std::string, std::string> params,
                  bool follow_location) {
    std::string base_url = get_base_url(url);
    std::string route_url = replace_all(url, base_url, "");

    httplib::Client client(base_url);
    client.set_follow_location(follow_location);

    network_log->log(LogLevel::INFO, LogOutput::FILE, "Making HTTP request to: " + base_url + route_url);


    std::string params_str;
    if (!params.empty()){
        params_str += "?";
        for (std::pair<const std::string, std::string> iter : params){
            params_str += iter.first + "=" + iter.second + "&";
        }
        params_str.pop_back();
        replace_all_in_place(params_str, " ", "%20");
        replace_all_in_place(params_str, "\"", "%22");
        replace_all_in_place(params_str, ":", "%3A");
    }

    route_url = (route_url.empty()? "/" : route_url) + params_str;

    httplib::Result response;
    switch (method) {
    case HTTPMethod::HEAD:
        response = client.Head(route_url, BASIC_HTTP_HEADERS);
        break;
    case HTTPMethod::GET:
        response = client.Get(route_url, BASIC_HTTP_HEADERS);
        break;
    }

    if (!response) {
        return HTTPResult{false, 0, ""};
    }
    return HTTPResult{true, response->status, response->body};
}
