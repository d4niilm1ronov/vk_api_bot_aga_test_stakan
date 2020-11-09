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
    "text":"Выберите подгруппу по лабораторным работам 👩‍🔬\n",
    "keyboard":{
        "buttons":[
            [
                {
                    "action":{
                        "type":"text",
                        "label":"Подгруппа",
                        "payload":"1"
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
            ],
            [
                {
                    "action":{
                        "type":"text",
                        "label":"Все подгруппы",
                        "payload":"411"
                    },
                    "color":"secondary"
                }
            ]
        ]
    }
}

*/ ////////////////////////////////////////////////////////////////////////

void stage :: setting_timetable_lab_group (const json& message) {
    // Сейчас в кэше:
    //  1. ["institute"] = uint
    //  2. ["menu"] = string
    //  3. ["year"] = uint
    //  4. ["group"] = uint

    const string current_stage = "setting_timetable_lab_group";

    // Получение параметров отправителя сообщения
    uint   peer_id    = message["peer_id"];
    string next_stage = data_base::get_user_stage(peer_id);
    json   user_cache = data_base::get_user_cache(peer_id);


    // --------------------------------------------------------------------
        //  Добавляется в кэш:
        //   1. ["lab_group"] = uint

    if (next_stage != current_stage) {

        if (user_cache.count("lab_group")) { user_cache.erase("lab_group"); }
        

        json final_message = stage::message[current_stage];
        json button = final_message["keyboard"]["buttons"][0];

        uint institute = user_cache["institute"];
        uint year      = user_cache["year"];
        uint group     = user_cache["group"];


        json arr_lab_group = data_base::get_group_labs(institute, year, group);

        if (!arr_lab_group.size()) {

            data_base::add_lesson(peer_id, institute * 10000 + year * 100 + group);

            user_cache.erase("institute");
            user_cache.erase("year");
            user_cache.erase("group");

            user_cache["menu"] = "user";
            next_stage = "menu_user";

            data_base::set_user_cache(peer_id, user_cache);

            easy::vkapi::messages_send(string("Расписание установлено! 🥳"), peer_id);


            stage::function[next_stage](message);
            
            return;
        }

        user_cache["lab_group"] = arr_lab_group;

        for (int i = 0; arr_lab_group.size() > i; i++) {
            // Генерация и Вставка кнопки
            button[0]["action"]["label"]   = string(arr_lab_group[i]);
            button[0]["action"]["payload"] = to_string(i + 1);
            final_message["keyboard"]["buttons"].push_back(button);

            // Генерация и Вставка текста (строчки)
            final_message["text"] = string(final_message["text"])
                    + "\n" + to_string(i + 1) + ") " + string(arr_lab_group[i]);
        }


        // Добавляем пункт "Все подгруппы"
        final_message["text"] = string(final_message["text"]) + "\n9) Все подгруппы";

        // Делаем правильное положение кнопки "Все подгруппы"
        final_message["keyboard"]["buttons"].push_back(final_message["keyboard"]["buttons"][2]);


        // Добавляем пункт назад
        final_message["text"] = string(final_message["text"]) + "\n0) Назад";

        // Делаем правильное положение кнопки "Назад"
        final_message["keyboard"]["buttons"].push_back(final_message["keyboard"]["buttons"][1]);


        // Удаляем шаблонные кнопки
        final_message["keyboard"]["buttons"].erase(0);
        final_message["keyboard"]["buttons"].erase(0);
        final_message["keyboard"]["buttons"].erase(0);

        // Отправка сообщения-клавиатуры
        easy::vkapi::messages_send(final_message, peer_id);
        data_base::set_user_cache(peer_id, user_cache);
        data_base::set_user_stage(peer_id, current_stage);
    }


    // --------------------------------------------------------------------
        

    else {
        uint lab_group;

        // Если была нажата кнопка
        if (message.count("payload")) {
            if (message["payload"] == "411") {
                lab_group = 0;
                next_stage = "menu_user";
            } else

            if (message["payload"] != "20545") {
                lab_group = stoi(string(message["payload"]));
                next_stage = "menu_user";
            }
            
            else {
                next_stage = "setting_timetable_group";
            }
        }
        

        // Если был введен текст
        else {

            if (message["text"] == "9") {
                lab_group = 0;
                next_stage = "menu_user";
            } else

            if (message["text"] != "0") {

                for (int i = 0; i < user_cache["lab_group"].size(); i++) {
                    if (message["text"] == to_string(i + 1)) {
                        lab_group = i + 1;
                        next_stage = "menu_user";
                        break;
                    }
                }

            }
            
            else {
                next_stage = "setting_timetable_group";
            }
        }


        if (next_stage == "menu_user") {
            if (lab_group) {
                data_base::add_lesson(peer_id, uint(user_cache["institute"]) * 10000 + uint(user_cache["year"]) * 100 + uint(user_cache["group"]), lab_group);
            } else {
                data_base::add_lesson(peer_id, uint(user_cache["institute"]) * 10000 + uint(user_cache["year"]) * 100 + uint(user_cache["group"]));
            }

            user_cache.erase("institute");
            user_cache.erase("year");
            user_cache.erase("group");

            user_cache["menu"] = "user";
            next_stage = "menu_user";

            easy::vkapi::messages_send(string("Расписание установлено! 🥳"), peer_id);
        }


        // --------------------------------------------------------------------




        data_base::set_user_cache(peer_id, user_cache);

        if (next_stage != current_stage) { stage::function[next_stage](message); }

        else { easy::vkapi::messages_send(string("Нет такого варианта ответа, выберите и напишите цифру из меню 👆"), peer_id); }


    }
    

}