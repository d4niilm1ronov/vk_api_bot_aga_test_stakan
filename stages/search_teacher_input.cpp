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

void stage :: search_teacher_input (const json& message) {

    const string this_stage = "search_teacher_input";

    // Получение параметров отправителя сообщения
    uint   peer_id    = message["peer_id"];
    string user_stage = data_base::get_user_stage(peer_id);
    json   user_cache = data_base::get_user_cache(peer_id);


    // --------------------------------------------------------------------

    // Если пользователь с прошлого stage попал на этот stage
    if (user_stage != this_stage) {
        data_base::set_user_cache(peer_id, user_cache);
        data_base::set_user_stage(peer_id, this_stage);
        easy::vkapi::messages_send(stage::message[this_stage], peer_id);
    }

    // --------------------------------------------------------------------

    // Если пользователь сделать действие (на этом stage)
    else {

        // Обработка нажатия кнопки (через payload)
        if (message.count("payload")) {
            if (message["payload"] == "20545") { user_stage = string("menu_") + string(user_cache["menu"]); }
        }
        

        // Обработка выбора пункта меню (через text сообщения)
        else {
            if (message["text"] == "0") { user_stage = string("menu_") + string(user_cache["menu"]); }
        }

        // --------------------------------------------------------------------

        // Поиск фамилии
        if (user_stage == this_stage) {
            bool done = false;

            if (string(message["text"]) != "null") {
                string teacher = string(message["text"]);

                data_base::db << "SELECT ls.time, ls.arr_date, ls.name, ls.place "
                                 "FROM lesson_stankin AS ls "
                                 "WHERE teacher = ? "
                                 "ORDER BY ls.date, ls.time; "
                << teacher
                >> [&done, peer_id](uint time, string str__arr_date, string name, string place) {

                    json arr_date = json::parse(str__arr_date);
                    string text_message_lesson;

                    // Название предмета
                    text_message_lesson += name + "\n";

                    // Аудитория
                    text_message_lesson += "Аудитория: " + place + "\n";

                    // Даты
                    text_message_lesson += "Даты:\n";
                    for (uint i = 0; arr_date.size() > i; i++) {
                        uint date = arr_date[i];
                        text_message_lesson += "– ";

                        // Обычная дата
                        if (date < 9999) {
                            text_message_lesson += to_string(date % 100) + "." + to_string(date / 100) ;
                        }
                        
                        // Повторяющийся
                        else {
                            // Ближайшая дата
                            text_message_lesson += to_string((date % 10000) % 100) + "." + to_string((date % 10000) / 100);
                            // Разделитель
                            text_message_lesson += " - ";
                            // Дальняя дата
                            text_message_lesson += to_string(((date / 10000) % 10000) % 100) + "." + to_string(((date / 10000) % 10000) / 100);

                            if ((date / 100000000) == 1) {
                                text_message_lesson += " каждую неделю";
                            } else {
                                text_message_lesson += " через неделю";
                            }
                        }

                        text_message_lesson += "\n";
                    }

                    // Время занятия
                    if (time == 1) { text_message_lesson += string("Время: 08:30 - 10:10"); } else
                    if (time == 2) { text_message_lesson += string("Время: 10:20 - 12:00"); } else
                    if (time == 3) { text_message_lesson += string("Время: 12:20 - 14:00"); } else
                    if (time == 4) { text_message_lesson += string("Время: 14:10 - 15:50"); } else
                    if (time == 5) { text_message_lesson += string("Время: 16:00 - 17:40"); } else
                    if (time == 6) { text_message_lesson += string("Время: 18:00 - 19:30"); } else
                    if (time == 7) { text_message_lesson += string("Время: 19:40 - 21:10"); } else
                                   { text_message_lesson += string("Время: 21:20 - 22:50"); }


                    if (place != "null") {
                        easy::vkapi::messages_send(text_message_lesson, peer_id);
                        done = true;
                    }
                    
                };
            }

            if (done) {
                user_stage = string("menu_") + string(user_cache["menu"]);
            }

        }

        // --------------------------------------------------------------------

        // Если в результате действия пользователя было выполненно действие
        if (user_stage != this_stage) {
            data_base::set_user_cache(peer_id, user_cache);
            stage::function[user_stage](message);
        }

        else { easy::vkapi::messages_send(string("😔 Такой фамилии нет в расписании университета. Возможно, преподаватель закончил преподавать в этом семестре. Проверьте правильность ввода или выберите пункт меню."), peer_id); }
    }
    

}