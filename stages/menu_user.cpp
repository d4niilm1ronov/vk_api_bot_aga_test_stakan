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

#include "../date.hpp"

/* ////////////////////////////////////////////////////////////////////////

{
    "text":"Выберите пункт меню:1) Занятие сейчас\n2) Занятия сегодня\n3) Занятия завтра\n\n0) Отписаться от расписания",
    "keyboard":{
        "buttons":[
            [
                {
                    "action":{
                        "type":"text",
                        "label":"Сейчас",
                        "payload":"1"
                    },
                    "color":"primary"
                },
                {
                    "action":{
                        "type":"text",
                        "label":"Сегодня",
                        "payload":"2"
                    },
                    "color":"primary"
                },
                {
                    "action":{
                        "type":"text",
                        "label":"Завтра",
                        "payload":"3"
                    },
                    "color":"primary"
                }
            ],
            [
                {
                    "action":{
                        "type":"text",
                        "label":"Отписаться от расписания",
                        "payload":"20545"
                    },
                    "color":"secondary"
                }
            ]
        ]
    }
}

*/ ////////////////////////////////////////////////////////////////////////

void stage :: menu_user (const json& message) {
    // Удаляется
    //  1. ["menu"] (string)
    //
    // Добавляется
    //  1. ["menu"] = "user"

    const string current_stage = "menu_user";

    // Получение параметров отправителя сообщения
    uint   peer_id    = message["peer_id"];
    string next_stage = data_base::get_user_stage(peer_id);
    json   user_cache = data_base::get_user_cache(peer_id);


    // --------------------------------------------------------------------

    // Если пользователь с прошлого stage попал на этот stage
    if (next_stage != current_stage) {
        if (user_cache.count("menu")) { user_cache.erase("menu"); }

        data_base::set_user_cache(peer_id, user_cache);
        data_base::set_user_stage(peer_id, current_stage);
        easy::vkapi::messages_send(stage::message[current_stage], peer_id);
    }


    // --------------------------------------------------------------------


    else {

        // Если была нажата кнопка
        if (message.count("payload")) {
            if (message["payload"] == "1") { next_stage = "print_current_lesson__menu_user"; } else
            if (message["payload"] == "2") { next_stage = "print_today_lesson__menu_user"; } else
            if (message["payload"] == "3") { next_stage = "print_tomorrow_lesson__menu_user"; } else
            if (message["payload"] == "20545") { next_stage = "menu_guest"; }
        }
        

        // Если был введен текст
        else {
            if (message["text"] == "1") { next_stage = "print_current_lesson__menu_user"; } else
            if (message["text"] == "2") { next_stage = "print_today_lesson__menu_user"; }   else
            if (message["text"] == "3") { next_stage = "print_tomorrow_lesson__menu_user"; } else
            if (message["text"] == "0") { next_stage = "menu_guest"; }
        }


        // --------------------------------------------------------------------


        data_base::set_user_cache(peer_id, user_cache);


        if (next_stage == "print_current_lesson__menu_user") {

            vector<json> vector__lesson_user = data_base::get_lesson__user (
                peer_id,
                time_stakan::get_current_date().format_mmdd(),
                time_stakan::last_number_lesson
            );
            
            if (vector__lesson_user.size()) { easy::vkapi::messages_send(string("Занятие сейчас 👇"    ), peer_id); }
            else                            { easy::vkapi::messages_send(string("Сейчас нет занятия 🤷‍♀️"), peer_id); }

            // Цикл по всем записям 
            for (auto iter: vector__lesson_user) {
                string text;

                text += string(iter["name"]) + " ";

                if (iter["type"] == 1) { text += string("[Лекция]\n"); }  else
                if (iter["type"] == 2) { text += string("[Семинар]\n"); } else
                if (iter["type"] == 3) { text += string("[Лабораторная]\n"); }

                if (iter["time"] == 1) { text += string("Время: до 10:10"); } else
                if (iter["time"] == 2) { text += string("Время: до 12:00"); } else
                if (iter["time"] == 3) { text += string("Время: до 14:00"); } else
                if (iter["time"] == 4) { text += string("Время: до 15:50"); } else
                if (iter["time"] == 5) { text += string("Время: до 17:40"); } else
                if (iter["time"] == 6) { text += string("Время: до 19:30"); } else
                if (iter["time"] == 7) { text += string("Время: до 21:10"); }
                else                             { text += string("Время: до 22:50"); }

                if ( iter["place"] != "null")
                    { text += string("\nАудитория: ") + string(iter["place"]); }

                if ( iter["teacher"] != "null")
                    { text += string("\nПреподаватель: ") + string(iter["teacher"]); }

                if ( iter["lab_group"] != "null")
                    { text += string("\nПодгруппа: ") + string(iter["lab_group"]); }
                

                easy::vkapi::messages_send(text, peer_id);
            }

        } else

        if (next_stage == "print_today_lesson__menu_user") {

            vector<json> vector__lesson_user = data_base::get_lesson__user (
                peer_id,
                time_stakan::get_current_date().format_mmdd()
            );
            
            if (vector__lesson_user.size()) { easy::vkapi::messages_send(string("Занятия сегодня 👇"    ), peer_id); }
            else                            { easy::vkapi::messages_send(string("Сегодня больше нет занятий 🤷‍♀️"), peer_id); }

            // Цикл по всем записям 
            for (auto iter: vector__lesson_user) {
                string text;

                text += string(iter["name"]) + " ";

                if (iter["type"] == 1) { text += string("[Лекция]\n"); }  else
                if (iter["type"] == 2) { text += string("[Семинар]\n"); } else
                if (iter["type"] == 3) { text += string("[Лабораторная]\n"); }

                if (iter["time"] == 1) { text += string("Время: 08:30 - 10:10"); } else
                if (iter["time"] == 2) { text += string("Время: 10:20 - 12:00"); } else
                if (iter["time"] == 3) { text += string("Время: 12:20 - 14:00"); } else
                if (iter["time"] == 4) { text += string("Время: 14:10 - 15:50"); } else
                if (iter["time"] == 5) { text += string("Время: 16:00 - 17:40"); } else
                if (iter["time"] == 6) { text += string("Время: 18:00 - 19:30"); } else
                if (iter["time"] == 7) { text += string("Время: 19:40 - 21:10"); }
                else                             { text += string("Время: 21:20 - 22:50"); }

                if ( iter["place"] != "null")
                    { text += string("\nАудитория: ") + string(iter["place"]); }

                if ( iter["teacher"] != "null")
                    { text += string("\nПреподаватель: ") + string(iter["teacher"]); }

                if ( iter["lab_group"] != "null")
                    { text += string("\nПодгруппа: ") + string(iter["lab_group"]); }
                

                easy::vkapi::messages_send(text, peer_id);
            }

        } else

        if (next_stage == "print_tomorrow_lesson__menu_user") {

            vector<json> vector__lesson_user = data_base::get_lesson__user (
                peer_id,
                time_stakan::get_current_date() .plus_one_day() .format_mmdd()
            );
            
            if (vector__lesson_user.size()) { easy::vkapi::messages_send(string("Занятия завтра 👇"    ), peer_id); }
            else                            { easy::vkapi::messages_send(string("На завтра нет занятий 🤷‍♀️"), peer_id); }

            // Цикл по всем записям 
            for (auto iter: vector__lesson_user) {
                string text;

                text += string(iter["name"]) + " ";

                if (iter["type"] == 1) { text += string("[Лекция]\n"); }  else
                if (iter["type"] == 2) { text += string("[Семинар]\n"); } else
                if (iter["type"] == 3) { text += string("[Лабораторная]\n"); }

                if (iter["time"] == 1) { text += string("Время: 08:30 - 10:10"); } else
                if (iter["time"] == 2) { text += string("Время: 10:20 - 12:00"); } else
                if (iter["time"] == 3) { text += string("Время: 12:20 - 14:00"); } else
                if (iter["time"] == 4) { text += string("Время: 14:10 - 15:50"); } else
                if (iter["time"] == 5) { text += string("Время: 16:00 - 17:40"); } else
                if (iter["time"] == 6) { text += string("Время: 18:00 - 19:30"); } else
                if (iter["time"] == 7) { text += string("Время: 19:40 - 21:10"); }
                else                             { text += string("Время: 21:20 - 22:50"); }

                if ( iter["place"] != "null")
                    { text += string("\nАудитория: ") + string(iter["place"]); }

                if ( iter["teacher"] != "null")
                    { text += string("\nПреподаватель: ") + string(iter["teacher"]); }

                if ( iter["lab_group"] != "null")
                    { text += string("\nПодгруппа: ") + string(iter["lab_group"]); }
                

                easy::vkapi::messages_send(text, peer_id);
            }

        } else

        if (next_stage == "menu_guest") {
            data_base::remove_lesson(peer_id);
            easy::vkapi::messages_send(string("Вы отписались от расписания 😔"), peer_id);

            data_base::set_user_cache(peer_id, user_cache);
            stage::function[next_stage](message);
        } else

        if (next_stage != current_stage) {
            user_cache["menu"] = "user";

            data_base::set_user_cache(peer_id, user_cache);
            stage::function[next_stage](message);
        }

        else { easy::vkapi::messages_send(string("Нет такого варианта ответа, выберите и напишите цифру из меню 👆"), peer_id); }


    }
    

}