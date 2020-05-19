#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <curl/curl.h>
#include <nlohmann/json.hpp>


using namespace std;
using json = nlohmann::json;

#include "Support/very_eassy_curl.hpp"

#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

int main() {
    const std::string my_token = "c7364e48cab5cbd2ae3268104fb95d7b8dfa830431a664f256bf9dae36b31685efef421173ac8f784076f";
    const unsigned int group_id = 193038255;

    vkapi::token_group test_token(my_token, group_id);
    vkapi::bots_long_poll test_blp = test_token.groups_getLongPollServer();

    srand(time(NULL));



    // fstream file("arr_keyboard.json");
    // json arr_keyboard;
    // json message_json;

    // file >> arr_keyboard;
    // message_json["random_id"] = rand();
    // message_json["peer_id"]   = 509452673;
    // message_json["text"]      = "Я+изменил+клавиатуру!";
    // message_json["keyboard"]  = arr_keyboard[json_keyboards::menu_setting_schedule];

    // cout << test_token.messages_send(message_json).dump(1) << endl; 



    map<int, int> user_database;
    user_database.insert({123, 123});

    

    // // Пересылка сообщений (текстовые и медиа)
    // while (true) {
    //     json ans = test_blp.request_lp();

    //     if (ans.count("failed")) {
    //         if (ans["failed"] == 1) {
    //             test_blp.set_ts(ans["ts"]);
    //             continue;
    //         }
            
    //         else {
    //             cout << "Failed Bots Long Poll API: " << ans["failed"] << endl;
    //             break;
    //         }
    //     }

    //     for (unsigned int i = 0; ans["updates"].size() > i; i++) {
    //         cout << test_token.messages_send(ans["updates"][i]["object"]["message"]).dump(1) << endl;
    //     }

    //     test_blp.set_ts(stoi(std::string(ans["ts"])));
    // }



    return 0;
}