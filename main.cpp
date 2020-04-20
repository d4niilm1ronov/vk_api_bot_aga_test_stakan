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

int main() {
    const std::string my_token = "c7364e48cab5cbd2ae3268104fb95d7b8dfa830431a664f256bf9dae36b31685efef421173ac8f784076f";
    const unsigned int group_id = 193038255;

    vkapi::token_group test_token(my_token, group_id);
    
    vkapi::bots_long_poll test_blp = test_token.groups_getLongPollServer();
    vkapi::message* testMessage;

    while(true) {
        vkapi::answer_botsLP ans = test_blp.request_lp();

        for (unsigned int i = 0; ans.count_updates > i; i++) {
            testMessage = reinterpret_cast<vkapi::message*>(ans[i].obj);
            test_token.messages_send(*testMessage);
        }
    }

    return 0;
}