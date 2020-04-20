#include <string>
#include <iostream>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "vk_api.hpp"

#include "very_eassy_curl.hpp"

#include "object/objects.hpp"

#include "object/attachment.hpp"
#include "object/photo.hpp"
#include "object/message.hpp"
#include "object/answer_botsLP.hpp"

#include "long_poll.hpp"
#include "token_vk.hpp"

////////////////////////////////////////////////////////////////////////

unsigned int vkapi::long_poll::get_ts() const
    { return last_ts; }

////////////////////////////////////////////////////////////////////////

void vkapi::long_poll::set_ts(const unsigned int& ts)
    { last_ts = ts; }

////////////////////////////////////////////////////////////////////////

unsigned int vkapi::long_poll::get_error() const
    { return error; }

////////////////////////////////////////////////////////////////////////

vkapi::answer_botsLP vkapi::bots_long_poll::request_lp() { 
    vkapi::answer_botsLP answer (reqCURL(
        objCURL,
        server_str +
        "?act=a_check&key=" +
        key_str +
        "&ts=" +
        std::to_string(last_ts) +
        "&wait=10"
    ));

    // Обновление поля ts (в успешном случае)
    if (answer.failed != NULL) {
        last_ts = *(answer.ts);
        error = 0;
    }
    
    // Обновление поля error (в случае ошибки)
    else {
        error = *(answer.failed);
    }

    return answer;
}

////////////////////////////////////////////////////////////////////////