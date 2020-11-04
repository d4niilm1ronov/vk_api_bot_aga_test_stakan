#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <map>

#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Библиотека для работы с SQLite 3 & C++
#include "../sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

// VK API
#include "../vkAPI/support/very_eassy_curl.hpp"
#include "../vkAPI/vk_api.hpp"
#include "../vkAPI/long_poll.hpp"
#include "../vkAPI/token_vk.hpp"

#include "../data_base.hpp"

#include "../additionally.hpp"

#include "../stage.hpp"

/* ////////////////////////////////////////////////////////////////////////



*/ ////////////////////////////////////////////////////////////////////////

void stage :: menu_user (const json& message) {
    // Удаляется
    //  1. ["menu"] (string)
    //
    // Добавляется
    //  1. ["menu"] = "guest"

    const string current_stage = "menu_user";

    // Получение параметров отправителя сообщения
    uint   peer_id    = message["peer_id"];
    string next_stage = data_base::get_user_stage(peer_id);
    json   user_cache = data_base::get_user_cache(peer_id);


    // --------------------------------------------------------------------


    if (next_stage != current_stage) {
        if (user_cache.count("menu")) { user_cache.erase("menu"); }
        
        user_cache["menu"] = "user";

        data_base::set_user_cache(peer_id, user_cache);
        data_base::set_user_stage(peer_id, current_stage);
        easy::vkapi::messages_send(stage::message[current_stage], peer_id);
    }


    // --------------------------------------------------------------------


    else {

        // Если была нажата кнопка
        if (message.count("payload")) {
            if (message["payload"] == "1") { next_stage = "menu_guest"; }
        }
        

        // Если был введен текст
        else {
            if (message["text"] == "1") { next_stage = "menu_guest"; }
        }


        if (next_stage == "menu_guest") {
            data_base::remove_lesson(peer_id);
            easy::vkapi::messages_send(string("Вы отписались от расписания 😔"), peer_id);
        }


        // --------------------------------------------------------------------


        data_base::set_user_cache(peer_id, user_cache);

        if (next_stage != current_stage) { stage::function[next_stage](message); }

        else { easy::vkapi::messages_send(string("Нет такого варианта ответа, выберите и напишите цифру из меню 👆"), peer_id); }


    }
    

}