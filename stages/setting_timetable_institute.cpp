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
    "text":"Выберите институт группы 👩‍🎓\n1) ИДБ\n2) АДБ\n3) МДБ\n4) ЭДБ\n0) Назад",
    "keyboard":{
        "buttons":[
            [
                {
                    "action":{
                        "type":"text",
                        "label":"ИДБ",
                        "payload":"10"
                    },
                    "color":"primary"
                }
            ],
            [
                {
                    "action":{
                        "type":"text",
                        "label":"АДБ",
                        "payload":"20"
                    },
                    "color":"primary"
                }
            ],
            [
                {
                    "action":{
                        "type":"text",
                        "label":"МДБ",
                        "payload":"30"
                    },
                    "color":"primary"
                }
            ],
            [
                {
                    "action":{
                        "type":"text",
                        "label":"ЭДБ",
                        "payload":"40"
                    },
                    "color":"primary"
                }
            ],
            [
                {
                    "action":{
                        "type":"text",
                        "label":"Назад",
                        "payload":"20545"
                    },
                    "color":"secondary"
                }
            ]
        ]
    }
}

*/ ////////////////////////////////////////////////////////////////////////

void stage :: setting_timetable_institute (const json& message) {
    // Добавляется
    //  1. ["institute"] = uint

    const string current_stage = "setting_timetable_institute";

    // Получение параметров отправителя сообщения
    uint   peer_id    = message["peer_id"];
    string next_stage = data_base::get_user_stage(peer_id);
    json   user_cache = data_base::get_user_cache(peer_id);


    // --------------------------------------------------------------------


    if (next_stage != current_stage) {
        if (user_cache.count("institute")) { user_cache.erase("institute"); }
        if (user_cache.count("year")) { user_cache.erase("year"); }

        data_base::set_user_stage(peer_id, current_stage);
        data_base::set_user_cache(peer_id, user_cache);
        easy::vkapi::messages_send(stage::message[current_stage], peer_id);
    }


    // --------------------------------------------------------------------


    else {


        // Если была нажата кнопка
        if (message.count("payload")) {
            if (message["payload"] == "10") { user_cache["institute"] = 10; next_stage = "setting_timetable_year"; }
            if (message["payload"] == "20") { user_cache["institute"] = 20; next_stage = "setting_timetable_year"; }
            if (message["payload"] == "30") { user_cache["institute"] = 30; next_stage = "setting_timetable_year"; }
            if (message["payload"] == "40") { user_cache["institute"] = 40; next_stage = "setting_timetable_year"; }

            if (message["payload"] == "20545") {
                next_stage = string("menu_") + string(user_cache["menu"]);
            }
        }
        

        // Если был введен текст
        else {
            if (message["text"] == "1") { user_cache["institute"] = 10; next_stage = "setting_timetable_year"; }
            if (message["text"] == "2") { user_cache["institute"] = 20; next_stage = "setting_timetable_year"; }
            if (message["text"] == "3") { user_cache["institute"] = 30; next_stage = "setting_timetable_year"; }
            if (message["text"] == "4") { user_cache["institute"] = 40; next_stage = "setting_timetable_year"; }

            if (message["text"] == "0") {
                next_stage = string("menu_") + string(user_cache["menu"]);
            }


        }


        // --------------------------------------------------------------------


        data_base::set_user_cache(peer_id, user_cache);

        if (next_stage != current_stage) { stage::function[next_stage](message); }

        else { easy::vkapi::messages_send(string("Нет такого варианта ответа, выберите и напишите цифру из меню 👆"), peer_id); }


    }
    

}