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
    "text":"Выберите год группы 📅",
    "keyboard":{
        "buttons":[
            [
                {
                    "action":{
                        "type":"text",
                        "label":"Институт",
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
            ]
        ]
    }
}

*/ ////////////////////////////////////////////////////////////////////////

void stage :: setting_timetable_year (const json& message) {
    // Сейчас в кэше:
    //  1. ["institute"] = uint
    //  2. ["menu"] = string

    const string current_stage = "setting_timetable_year";

    // Получение параметров отправителя сообщения
    uint   peer_id    = message["peer_id"];
    string next_stage = data_base::get_user_stage(peer_id);
    json   user_cache = data_base::get_user_cache(peer_id);


    // --------------------------------------------------------------------
        //  Добавляется в кэш:
        //   1. year = [uint]

    if (next_stage != current_stage) {

        
        if (user_cache.count("year")) { user_cache.erase("year"); }
        if (user_cache.count("group")) { user_cache.erase("group"); }
        

        json final_message = stage::message[current_stage];
        json button = final_message["keyboard"]["buttons"][0];

        uint institute = user_cache["institute"];


        // Пробигаем по всем доступным институтам, добавляем их в сообщение
        {
            data_base::db
            << "SELECT DISTINCT year FROM group_stankin WHERE institute = ?;" << institute
            >> [&final_message, &button, &user_cache, institute] (int year) {
                // Генерируем институт+год_начала_обучения
                string str__institute_year;
                {
                    switch (institute) {
                        case 10: str__institute_year = "ИДБ-"; break;
                        case 20: str__institute_year = "АДБ-"; break;
                        case 30: str__institute_year = "МДБ-"; break;
                        case 40: str__institute_year = "ЭДБ-"; break;
                    }

                    str__institute_year += to_string(year);
                }

                // Вставляем в кэш
                user_cache["year"].push_back(year);

                // Генерация и Вставка кнопки
                button[0]["action"]["label"]   = str__institute_year;
                button[0]["action"]["payload"] = to_string(year);
                final_message["keyboard"]["buttons"].push_back(button);

                // Генерация и Вставка текста (строчки)
                final_message["text"] = string(final_message["text"])
                        + "\n" + to_string(user_cache["year"].size()) + ") " + str__institute_year;
            };

            // Добавляем кнопку назад
            final_message["keyboard"]["buttons"].push_back(final_message["keyboard"]["buttons"][1]);

            // Добавляем пункт назад
            final_message["text"] = string(final_message["text"]) + "\n0) Назад";

            // Удаляем шаблонные кнопки
            final_message["keyboard"]["buttons"].erase(0);
            final_message["keyboard"]["buttons"].erase(0);
        }


        data_base::set_user_cache(peer_id, user_cache);

        data_base::set_user_stage(peer_id, current_stage);
        
        // Отправка сообщения-клавиатуры
        easy::vkapi::messages_send(final_message, peer_id);
    }


    // --------------------------------------------------------------------
        //  Изменяется в кэше:
        //   1. ( year = [uint] )  =>  ( year = uint )

    else {

        // Если была нажата кнопка
        if (message.count("payload")) {
            if (message["payload"] != "20545") {
                user_cache.erase("year");
                user_cache["year"] = stoi(string(message["payload"]));
                next_stage = "setting_timetable_group";
            }
            
            else {
                next_stage = "setting_timetable_institute";
            }
        }
        

        // Если был введен текст
        else {
            if (message["text"] != "0") {

                for (int i = 0; i < user_cache["year"].size(); i++) {
                    if (to_string(i + 1) == message["text"]) {
                        user_cache.erase("year");
                        user_cache["year"] = user_cache["year"][i];
                        next_stage = "setting_timetable_group";
                    }
                }
            }
            
            else {
                next_stage = "setting_timetable_institute";
            }
        }


        // --------------------------------------------------------------------


        data_base::set_user_cache(peer_id, user_cache);

        if (next_stage != current_stage) { stage::function[next_stage](message); }

        else { easy::vkapi::messages_send(string("Нет такого варианта ответа, выберите и напишите цифру из меню 👆"), peer_id); }


    }
    

}