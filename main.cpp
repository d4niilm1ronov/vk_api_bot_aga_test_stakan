#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "very_eassy_curl.hpp"

#include "vk_api.hpp"
    #include "long_poll.hpp"
    #include "token_vk.hpp"

////////////////////////////////////////////////////////////////////////

int main() {
    const std::string my_token = "c7364e48cab5cbd2ae3268104fb95d7b8dfa830431a664f256bf9dae36b31685efef421173ac8f784076f";
    const unsigned int group_id = 193038255;

    vkapi::token_group test_token(my_token, group_id);

    vkapi::bots_long_poll test_blp = test_token.groups_getLongPollServer();    

    // Пересылка сообщений (текстовые и медиа)
    while (true) {
        json ans = test_blp.request_lp();

        if (ans.count("failed")) {
            if (ans["failed"] == 1) {
                test_blp.set_ts(ans["ts"]);
                continue;
            }
            
            else {
                cout << "Failed Bots Long Poll API: " << ans["failed"] << endl;
                break;
            }
        }

        for (unsigned int i = 0; ans["updates"].size() > i; i++) {
            cout << test_token.messages_send(ans["updates"][i]["object"]["message"]).dump(1) << endl;
        }

        test_blp.set_ts(stoi(std::string(ans["ts"])));
    }



    return 0;
}