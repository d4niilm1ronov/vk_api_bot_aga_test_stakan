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

{
    "text":"Выберите пункт меню 💁‍♀️\n\n1. Установить расписание",
    "keyboard":{
        "buttons":[
            [
                {
                    "action":{
                        "type":"text",
                        "label":"Установить расписание",
                        "payload":"1"
                    },
                    "color":"primary"
                }
            ]
        ]
    }
}

*/ ////////////////////////////////////////////////////////////////////////

void stage :: menu_guest (const json& message) {
    // Удаляется
    //  1. ["menu"] (string)
    //
    // Добавляется
    //  1. ["menu"] = "guest"

    const string this_stage = "menu_guest";

    // Получение параметров отправителя сообщения
    uint   peer_id    = message["peer_id"];
    string user_stage = data_base::get_user_stage(peer_id);
    json   user_cache = data_base::get_user_cache(peer_id);


    // --------------------------------------------------------------------

    // Если пользователь с прошлого stage попал на этот stage
    if (user_stage != this_stage) {
        if (user_cache.count("menu"))      { user_cache.erase("menu"); }
        if (user_cache.count("institute")) { user_cache.erase("institute"); }

        data_base::set_user_cache(peer_id, user_cache);
        data_base::set_user_stage(peer_id, this_stage);
        easy::vkapi::messages_send(stage::message[this_stage], peer_id);
    }

    // --------------------------------------------------------------------

    // Если пользователь сделать действие (на этом stage)
    else {

        // Обработка нажатия кнопки (через payload)
        if (message.count("payload")) {
            if (message["payload"] == "1") { user_stage = "setting_timetable_institute"; }
        }
        

        // Обработка выбора пункта меню (через text сообщения)
        else {
            if (message["text"] == "1") { user_stage = "setting_timetable_institute"; }
        }

        // --------------------------------------------------------------------

        // Если в результате действия пользователя было выполненно действие
        if (user_stage != this_stage) {

            user_cache["menu"] = "guest";

            data_base::set_user_cache(peer_id, user_cache);
            stage::function[user_stage](message);
        }

        else { easy::vkapi::messages_send(string("Нет такого варианта ответа, выберите и напишите цифру из меню 👆"), peer_id); }
    }
    

}