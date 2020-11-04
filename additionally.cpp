#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <iterator>
#include <ctime>
#include <vector>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Библиотека для работы с SQLite 3
#include "sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

// VK API
#include "vkAPI/support/very_eassy_curl.hpp"
#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

#include "data_base.hpp"

#include "additionally.hpp"


////////////////////////////////////////////////////////////////////////


// токен и id сообщества
vkapi::token_group stankin_bot(string("c7364e48cab5cbd2ae3268104fb95d7b8dfa830431a664f256bf9dae36b31685efef421173ac8f784076f"), 193038255);


////////////////////////////////////////////////////////////////////////

bool its_text_message(const json& message) {
    if (!message.count("text")) { return false; }
    if (!static_cast<string>(message["text"]).size()) { return false; }

    if (message.count("attachments")) {
        if (message["attachments"].size()) {
            return false;
        }
    }

    if (message.count("fwd_messages")) {
        if (message["fwd_messages"].size()) {
            return false;
        }
    }
    
    if (message.count("reply_message")) {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////

string kill_rus_e(string str) {
    for (unsigned int i = 0; i < str.size(); i++) {
        if ((str[i] == -48) | (str[i] == -47)) {

            if (str[i] == -48) {
                i++;

                if (str[i] == -127) {
                    str[i] = -107;
                }
            }
            
            else {
                i++;

                if (str[i] == -111) {
                    str[i] = -75;
                    str[i - 1] = -48;
                }
            }

        }
    }

    return str;
}

////////////////////////////////////////////////////////////////////////

string to_upper_rus(string str) {
    for (unsigned int i = 0; i < str.size(); i++) {
        if ((str[i] == -48) | (str[i] == -47)) {

            if (str[i] == -48) {
                i++;

                if ((str[i] >= -80) & (str[i] <= -65)) {
                    str[i] = str[i] - 32;
                }
            }
            
            else {
                i++;

                if ((str[i] >= -128) & (str[i] <= -113)) {
                    str[i] = str[i] + 32;
                    i--;
                    str[i] = -48;
                    i++;
                }
            }

        }
    }

    return str;
}

////////////////////////////////////////////////////////////////////////

bool its_rus_name(string surname) {
    string caps_surname = to_upper_rus(kill_rus_e(surname));

    for (unsigned int i = 0; i < caps_surname.size(); i++) {
        if (caps_surname[i] == '-') { continue; }
        if (caps_surname[i] != -48) { return false; } else { i++; }
        if (!((caps_surname[i] >= -112 ) & (caps_surname[i] <= -81))) { return false; }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////

json easy::vkapi::messages_send(json message) {
    uint random_id;
    uint peer_id = message["peer_id"];

    data_base::db << "SELECT random_id FROM user WHERE id = ? ;"
    << peer_id
    >> random_id;

    random_id++;

    data_base::db << "UPDATE user SET random_id = ? WHERE id = ? ;"
    << random_id
    << peer_id;

    message["random_id"] = random_id;


    return stankin_bot.messages_send(message);
}

////////////////////////////////////////////////////////////////////////

json easy::vkapi::messages_send(json message, const uint& peer_id) {
    message["peer_id"] = peer_id;

    return easy::vkapi::messages_send(message);
}

////////////////////////////////////////////////////////////////////////

json easy::vkapi::messages_send(const string& text, const uint& peer_id) {
    json message;
    message["text"] = text;

    return easy::vkapi::messages_send(message, peer_id);
}

////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////