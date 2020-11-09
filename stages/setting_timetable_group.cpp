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
    "text":"Выберите номер группы 🔢",
    "keyboard":{
        "buttons":[
            [
                {
                    "action":{
                        "type":"text",
                        "label":"Группа",
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

void stage :: setting_timetable_group (const json& message) {
    // Сейчас в кэше:
    //  1. ["institute"] = uint
    //  2. ["menu"] = string
    //  3. ["year"] = uint

    const string current_stage = "setting_timetable_group";

    // Получение параметров отправителя сообщения
    uint   peer_id    = message["peer_id"];
    string next_stage = data_base::get_user_stage(peer_id);
    json   user_cache = data_base::get_user_cache(peer_id);


    // --------------------------------------------------------------------
        //  Добавляется в кэш:
        //   1. ["group"] = [uint]

    if (next_stage != current_stage) {

        if (user_cache.count("group")) { user_cache.erase("group"); }
        if (user_cache.count("lab_group")) { user_cache.erase("lab_group"); }
        

        json final_message = stage::message[current_stage];
        json button = final_message["keyboard"]["buttons"][0];

        uint institute = user_cache["institute"];
        uint year      = user_cache["year"];


        // Пробигаем по всем доступным институтам, добавляем их в сообщение
        {
            uint count = 0; // счётчик для кол-во кнопок (чтобы поместились)

            data_base::db
            << "SELECT num FROM group_stankin WHERE (institute = ?) AND (year = ?) ;"
            << institute << year
            >> [&final_message, &button, &user_cache, &count, institute, year] (int group) {
                // Генерируем институт+год_начала_обучения
                string str__institute_year_number;

                switch (institute) {
                    case 10: str__institute_year_number = "ИДБ-"; break;
                    case 20: str__institute_year_number = "АДБ-"; break;
                    case 30: str__institute_year_number = "МДБ-"; break;
                    case 40: str__institute_year_number = "ЭДБ-"; break;
                }

                str__institute_year_number += to_string(year);

                // Добавляем нуль в название группы, чей порядковый номер меньше 10
                // Например: из ИДБ-19-9 в ИДБ-19-09
                if (group < 10) { str__institute_year_number += "-0"; }
                else            { str__institute_year_number += "-";  }

                str__institute_year_number += to_string(group);
            

                // Вставляем в кэш
                user_cache["group"].push_back(group);

                // Генерация кнопки
                button[0]["action"]["label"]   = str__institute_year_number;
                button[0]["action"]["payload"] = to_string(group);

                // Вставка кнопки
                if (count % 2) {
                    final_message["keyboard"]["buttons"][(count / 2) + 2].push_back(button[0]);
                } else {
                    final_message["keyboard"]["buttons"].push_back(button);
                }

                // Генерация и Вставка текста (строчки)
                final_message["text"] =
                    string(final_message["text"]) + "\n" + to_string(count + 1) + ". " + str__institute_year_number;

                count++;
            };

            // Добавляем кнопку назад
            final_message["keyboard"]["buttons"].push_back(final_message["keyboard"]["buttons"][1]);

            // Добавляем пункт назад
            final_message["text"] = string(final_message["text"]) + "\n0. Назад";

            // Удаляем шаблонные кнопки
            final_message["keyboard"]["buttons"].erase(0);
            final_message["keyboard"]["buttons"].erase(0);
        }
        
        
        // Отправка сообщения-клавиатуры
        easy::vkapi::messages_send(final_message, peer_id);

        data_base::set_user_stage(peer_id, current_stage);

        data_base::set_user_cache(peer_id, user_cache);
    }


    // --------------------------------------------------------------------
        //  Изменяется в кэше:
        //   1. ( group = [uint] )  =>  ( group = uint )

    else {

        // Если была нажата кнопка
        if (message.count("payload")) {
            if (message["payload"] != "20545") {
                user_cache.erase("group");
                user_cache["group"] = stoi(string(message["payload"]));
                next_stage = "setting_timetable_lab_group";
            }
            
            else {
                next_stage = "setting_timetable_year";
            }
        }
        

        // Если был введен текст
        else {
            if (message["text"] != "0") {

                for (int i = 0; i < user_cache["group"].size(); i++) {
                    if (to_string(i + 1) == message["text"]) {
                        user_cache.erase("group");
                        user_cache["group"] = user_cache["group"][i];
                        next_stage = "setting_timetable_lab_group";
                    }
                }
            }
            
            else {
                next_stage = "setting_timetable_year";
            }
        }


        // --------------------------------------------------------------------


        data_base::set_user_cache(peer_id, user_cache);

        if (next_stage != current_stage) { stage::function[next_stage](message); }

        else { easy::vkapi::messages_send(string("Нет такого варианта ответа, выберите и напишите цифру из меню 👆"), peer_id); }


    }
    

}