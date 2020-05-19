#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "../Support/very_eassy_curl.hpp"

#include "vk_api.hpp"
    #include "long_poll.hpp"
    #include "token_vk.hpp"

////////////////////////////////////////////////////////////////////////

unsigned int vkapi::long_poll::get_ts() const {
    return ts_int;
}

////////////////////////////////////////////////////////////////////////

void vkapi::long_poll::set_ts(const unsigned int& ts) {
    ts_int = ts;
}

////////////////////////////////////////////////////////////////////////

nlohmann::json vkapi::bots_long_poll::request_lp() const { 
    return json(json::parse(reqCURL(
        objCURL,
        server_str +
        "?act=a_check&key=" +
        key_str +
        "&ts=" +
        std::to_string(ts_int) +
        "&wait=10"
    )));
}

////////////////////////////////////////////////////////////////////////