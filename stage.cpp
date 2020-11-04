#include <iostream>
#include <filesystem>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <map>

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <string>
#include <dirent.h>
#include <cstdio>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Библиотека для работы с SQLite 3 & C++
#include "sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

// VK API
#include "vkAPI/support/very_eassy_curl.hpp"
#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

#include "data_base.hpp"

#include "additionally.hpp"

#include "stage.hpp"

// фикс
vkapi::token_group& test_token = stankin_bot;

map<string, std::function<json(json)>> stage::function;
map<string, json>                      stage::message;

///////////////////////////////////////////////////////////////////////////

json stage :: menu_guest (const json& message) {
    // Получение peer_id (ID пользователя) и текущий stage
    uint peer_id = message["peer_id"];
    string result = data_base::get_user_stage(peer_id);
    json mesg_result;

    if (message.count("payload")) {
        if (message["payload"] == "1") {
            result = "setting_timetable_institute";
        }
    }

    if (message["text"] == "1") {
        result = "setting_timetable_institute";
    }

    if (result == "setting_timetable_institute") {
        // Установка меню для возвращения обратно в случаи прекращения установки расписания
        json user_cache = data_base::get_user_cache(peer_id);
        user_cache["menu"] = "menu_guest";

        mesg_result = stage::message["setting_timetable_institute"];
        json button = mesg_result["keyboard"]["buttons"][0];

        // Пробигаем по всем доступным институтам, добавляем их в сообщение
        data_base::db
        << "SELECT DISTINCT institute FROM group_stankin;"
        >> [&mesg_result, &button, &user_cache] (int id_institute) {
            string name_institute;

            switch (id_institute) {
                case 10: name_institute = "ИДБ"; break;
                case 20: name_institute = "АДБ"; break;
                case 30: name_institute = "МДБ"; break;
                case 40: name_institute = "ЭДБ"; break;
            }

            // Вставляем в кэш
            user_cache["institute"].push_back(id_institute);

            // Генерация и Вставка кнопки
            button[0]["action"]["label"]   = name_institute;
            button[0]["action"]["payload"] = to_string(id_institute);
            mesg_result["keyboard"]["buttons"].push_back(button);

            // Генерация и Вставка текста (строчки)
            mesg_result["text"] = string(mesg_result["text"])
                    + string("\n") + to_string(user_cache["institute"].size()) + ". " + name_institute;
        };

        // Добавляем кнопку назад
        mesg_result["keyboard"]["buttons"].push_back(mesg_result["keyboard"]["buttons"][1]);

        // Добавляем пункт назад
        mesg_result["text"] = string(mesg_result["text"]) + "\n0. Назад";

        // Удаляем шаблонные кнопки
        mesg_result["keyboard"]["buttons"].erase(0);
        mesg_result["keyboard"]["buttons"].erase(0);

        // Установка кэша пользователя
        data_base::set_user_cache(peer_id, user_cache);

        // Переход на новую стадию
        data_base::set_user_stage(peer_id, "setting_timetable_institute");
    }
    
    else {
        mesg_result["text"] = "Нет такого варианта ответа, напиши цифру из Меню 👆";
    }

    
    return mesg_result;
}

///////////////////////////////////////////////////////////////////////////

json stage :: menu_user (const json& message) {
    // Получение peer_id (ID пользователя) и текущий stage
    uint peer_id = message["peer_id"];
    string result = data_base::get_user_stage(peer_id);
    json mesg_result;

    // Если есть "payload" (нажата кнопка)
    if (message.count("payload")) {
        if (message["payload"] == "1") { result = "menu_guest"; }
    }
    
    // Если было выбранно число из пунктов меню
    else {
        string text = message["text"];

        if (text == "1") { result = "menu_guest"; }
    }

    // --------------------------------------------------------------------

    // Обработка для случая "menu_guest"
    if (result == "menu_guest") {

        mesg_result = stage::message["menu_guest"];

        // Удалением ВСЕХ занятий у пользователя
        data_base::remove_lesson(peer_id);
    }

    // --------------------------------------------------------------------

    else {
        mesg_result["text"] = "Нет такого варианта ответа, напиши цифру из Меню 👆";
    }

    data_base::set_user_stage(peer_id, result);


    return mesg_result;
}

///////////////////////////////////////////////////////////////////////////

json stage :: setting_timetable_institute (const json& message) {
    // Получение peer_id (ID пользователя) и текущий stage
    uint peer_id = message["peer_id"];
    string result = data_base::get_user_stage(peer_id);

    json user_cache = data_base::get_user_cache(peer_id);
    uint institute;

    json mesg_result;

    // Если есть "payload" (нажата кнопка)
    if (message.count("payload")) {
        if (message["payload"] != "404") {
            institute = stoi(string(message["payload"]));
            result = "setting_timetable_year";
        }
        
        else {
            result = user_cache["menu"];
            user_cache.erase("menu");
            user_cache.erase("institute");
        }
    }
    
    // Если было выбранно число из пунктов меню
    else {
        string text = message["text"];

        if (text != "0") {

            for (int i = 0; i < user_cache["institute"].size(); i++) {
                if (to_string(i + 1) == text) {
                    institute = user_cache["institute"][i];
                    result = "setting_timetable_year";
                }
            }
        }
        
        else {
            result = user_cache["menu"];
            user_cache.erase("menu");
            user_cache.erase("institute");
        }

    }

    // --------------------------------------------------------------------

    if (result == "setting_timetable_year") {
        mesg_result = stage::message["setting_timetable_year"];

        json button = mesg_result["keyboard"]["buttons"][0];

        // Пробигаем по всем доступным институтам, добавляем их в сообщение
        data_base::db
        << "SELECT DISTINCT year FROM group_stankin WHERE institute = ?;" << institute
        >> [&mesg_result, &button, &user_cache, institute] (int year_group) {
            // Генерируем институт+год_начала_обучения
            string str__institute_year;
            {
                switch (institute) {
                    case 10: str__institute_year = "ИДБ-"; break;
                    case 20: str__institute_year = "АДБ-"; break;
                    case 30: str__institute_year = "МДБ-"; break;
                    case 40: str__institute_year = "ЭДБ-"; break;
                }

                str__institute_year += to_string(year_group);
            }

            // Вставляем в кэш
            user_cache["year_group"].push_back(year_group);

            // Генерация и Вставка кнопки
            button[0]["action"]["label"]   = str__institute_year;
            button[0]["action"]["payload"] = to_string(year_group);
            mesg_result["keyboard"]["buttons"].push_back(button);

            // Генерация и Вставка текста (строчки)
            mesg_result["text"] = string(mesg_result["text"])
                    + "\n" + to_string(user_cache["year_group"].size()) + ". " + str__institute_year;
        };

        // Добавляем кнопку назад
        mesg_result["keyboard"]["buttons"].push_back(mesg_result["keyboard"]["buttons"][1]);

        // Добавляем пункт назад
        mesg_result["text"] = string(mesg_result["text"]) + "\n0. Назад";

        // Удаляем шаблонные кнопки
        mesg_result["keyboard"]["buttons"].erase(0);
        mesg_result["keyboard"]["buttons"].erase(0);

        // Установка кэша пользователя
        user_cache.erase("institute");
        user_cache["institute"] = institute;
    } else



    if (result == "menu_guest") {
        mesg_result = stage::message["menu_guest"];
    }

    // --------------------------------------------------------------------

    else {
        mesg_result["text"] = "Нет такого варианта ответа, напиши число из меню 👆";
    }

    data_base::set_user_cache(peer_id, user_cache);
    data_base::set_user_stage(peer_id, result);


    return mesg_result;
}

///////////////////////////////////////////////////////////////////////////

json stage :: setting_timetable_year (const json& message) {
    // Получение peer_id (ID пользователя) и текущий stage
    uint peer_id = message["peer_id"];
    string result = data_base::get_user_stage(peer_id);

    json user_cache = data_base::get_user_cache(peer_id);
    uint institute = user_cache["institute"];
    uint year;

    json mesg_result;

    // Если есть "payload" (нажата кнопка)
    if (message.count("payload")) {
        if (string(message["payload"]) != "404") {
            year = stoi(string(message["payload"]));
            result = "setting_timetable_group";
        }
        
        else {
            result = "setting_timetable_institute";
        }
    }
    
    // Если было выбранно число из пунктов меню
    else {
        string text = message["text"];

        if (text != "0") {

            for (int i = 0; i < user_cache["year_group"].size(); i++) {
                if (to_string(i + 1) == text) {
                    year = user_cache["year_group"][i];
                    result = "setting_timetable_group";
                }
            }
        }
        
        else {
            result = "setting_timetable_institute";
        }

    }

    // --------------------------------------------------------------------

    // Переход на следующий этап, генерация для него меню
    if (result == "setting_timetable_group") {
        mesg_result = stage::message["setting_timetable_group"];

        json button = mesg_result["keyboard"]["buttons"][0];

        uint count = 0;

        // Пробигаем по всем доступным институтам, добавляем их в сообщение
        data_base::db
        << "SELECT num FROM group_stankin WHERE (institute = ?) AND (year = ?) ;"
        << institute << year
        >> [&mesg_result, &button, &user_cache, &count, institute, year] (int num_group) {
            // Генерируем институт+год_начала_обучения
            string str__institute_year_number;
            {
                switch (institute) {
                    case 10: str__institute_year_number = "ИДБ-"; break;
                    case 20: str__institute_year_number = "АДБ-"; break;
                    case 30: str__institute_year_number = "МДБ-"; break;
                    case 40: str__institute_year_number = "ЭДБ-"; break;
                }

                str__institute_year_number += to_string(year);
                str__institute_year_number += "-" + to_string(num_group);
            }

            // Вставляем в кэш
            user_cache["num_group"].push_back(num_group);

            // Генерация кнопки
            button[0]["action"]["label"]   = str__institute_year_number;
            button[0]["action"]["payload"] = to_string(num_group);

            // Вставка кнопки
            if (count % 2) {
                mesg_result["keyboard"]["buttons"][(count / 2) + 2].push_back(button[0]);
            } else {
                mesg_result["keyboard"]["buttons"].push_back(button);
            }
            

            // Генерация и Вставка текста (строчки)
            mesg_result["text"] =
                string(mesg_result["text"]) + "\n" + to_string(count + 1) + ". " + str__institute_year_number;

            count++;
        };

        // Добавляем кнопку назад
        mesg_result["keyboard"]["buttons"].push_back(mesg_result["keyboard"]["buttons"][1]);

        // Добавляем пункт назад
        mesg_result["text"] = string(mesg_result["text"]) + "\n0. Назад";

        // Удаляем шаблонные кнопки
        mesg_result["keyboard"]["buttons"].erase(0);
        mesg_result["keyboard"]["buttons"].erase(0);

        // Установка кэша пользователя
        user_cache.erase("year_group");
        user_cache["year_group"] = year;
    } else

    // Переход на предыдущий этап, генерация для него меню
    if (result == "setting_timetable_institute") {
        user_cache.erase("year_group");
        user_cache.erase("institute");

        mesg_result = stage::message["setting_timetable_institute"];
        json button = mesg_result["keyboard"]["buttons"][0];

        // Пробигаем по всем доступным институтам, добавляем их в сообщение
        data_base::db
        << "SELECT DISTINCT institute FROM group_stankin;"
        >> [&mesg_result, &button, &user_cache] (int id_institute) {
            string name_institute;

            switch (id_institute) {
                case 10: name_institute = "ИДБ"; break;
                case 20: name_institute = "АДБ"; break;
                case 30: name_institute = "МДБ"; break;
                case 40: name_institute = "ЭДБ"; break;
            }

            // Вставляем в кэш
            user_cache["institute"].push_back(id_institute);

            // Генерация и Вставка кнопки
            button[0]["action"]["label"]   = name_institute;
            button[0]["action"]["payload"] = to_string(id_institute);
            mesg_result["keyboard"]["buttons"].push_back(button);

            // Генерация и Вставка текста (строчки)
            mesg_result["text"] = string(mesg_result["text"])
                    + string("\n") + to_string(user_cache["institute"].size()) + ". " + name_institute;
        };

        // Добавляем кнопку назад
        mesg_result["keyboard"]["buttons"].push_back(mesg_result["keyboard"]["buttons"][1]);

        // Добавляем пункт назад
        mesg_result["text"] = string(mesg_result["text"]) + "\n0. Назад";

        // Удаляем шаблонные кнопки
        mesg_result["keyboard"]["buttons"].erase(0);
        mesg_result["keyboard"]["buttons"].erase(0);
    }

    // --------------------------------------------------------------------

    else {
        mesg_result["text"] = "Нет такого варианта ответа, напиши число из меню 👆";
    }

    data_base::set_user_cache(peer_id, user_cache);
    data_base::set_user_stage(peer_id, result);
    return mesg_result;
}

///////////////////////////////////////////////////////////////////////////

json stage :: setting_timetable_group (const json& message) {
    // Получение peer_id (ID пользователя) и текущий stage
    uint peer_id = message["peer_id"];
    string result = data_base::get_user_stage(peer_id);

    json user_cache = data_base::get_user_cache(peer_id);
    uint institute = user_cache["institute"];
    uint year = user_cache["year_group"];
    uint num_group;

    json mesg_result;

    // Если есть "payload" (нажата кнопка)
    if (message.count("payload")) {
        if (string(message["payload"]) != "404") {
            num_group = stoi(string(message["payload"]));
            result = "setting_timetable_lab_group";
        }
        
        else {
            result = "setting_timetable_year";
        }
    }
    
    // Если было выбранно число из пунктов меню
    else {
        if (message["text"] != "0") {

            for (int i = 0; i < user_cache["num_group"].size(); i++) {
                if (message["text"] == to_string(i + 1)) {
                    num_group = user_cache["num_group"][i];
                    result = "setting_timetable_lab_group";
                }
            }
        }
        
        else {
            result = "setting_timetable_year";
        }

    }

    // --------------------------------------------------------------------

    // Переход на следующий этап, генерация для него меню
    if (result == "setting_timetable_lab_group") {
        mesg_result = stage::message["setting_timetable_lab_group"];

        json button = mesg_result["keyboard"]["buttons"][0];

        json arr_lab_group = data_base::get_group_labs(institute, year, num_group);

        if (!arr_lab_group.size()) {
            // Установка кэша пользователя
            user_cache.erase("num_group");
            user_cache["num_group"] = num_group;

            json new_message;
            new_message["text"] = "9";

            data_base::set_user_cache(peer_id, user_cache);
            data_base::set_user_stage(peer_id, result);

            return stage::function[data_base::get_user_stage(peer_id)](message);
        }

        user_cache["lab_group"] = arr_lab_group;

        for (int i = 0; arr_lab_group.size() > i; i++) {

            // Генерация и Вставка кнопки
            button[0]["action"]["label"]   = string(arr_lab_group[i]);
            button[0]["action"]["payload"] = to_string(i + 1);
            mesg_result["keyboard"]["buttons"].push_back(button);

            // Генерация и Вставка текста (строчки)
            mesg_result["text"] = string(mesg_result["text"])
                    + "\n" + to_string(i + 1) + ". " + string(arr_lab_group[i]);
        }


        // Добавляем кнопку назад
        mesg_result["keyboard"]["buttons"].push_back(mesg_result["keyboard"]["buttons"][1]);

        // Добавляем пункт назад
        mesg_result["text"] = string(mesg_result["text"]) + "\n0. Назад";

        // Удаляем шаблонные кнопки
        mesg_result["keyboard"]["buttons"].erase(0);
        mesg_result["keyboard"]["buttons"].erase(0);

        // Установка кэша пользователя
        user_cache.erase("num_group");
        user_cache["num_group"] = num_group;
    } else

    // Переход на предыдущий этап, генерация для него меню
    if (result == "setting_timetable_year") {
        user_cache.erase("num_group");
        user_cache.erase("year_group");

        mesg_result = stage::message["setting_timetable_year"];

        json button = mesg_result["keyboard"]["buttons"][0];

        // Пробигаем по всем доступным институтам, добавляем их в сообщение
        data_base::db
        << "SELECT DISTINCT year FROM group_stankin WHERE institute = ?;" << institute
        >> [&mesg_result, &button, &user_cache, institute] (int year_group) {
            // Генерируем институт+год_начала_обучения
            string str__institute_year;
            {
                switch (institute) {
                    case 10: str__institute_year = "ИДБ-"; break;
                    case 20: str__institute_year = "АДБ-"; break;
                    case 30: str__institute_year = "МДБ-"; break;
                    case 40: str__institute_year = "ЭДБ-"; break;
                }

                str__institute_year += to_string(year_group);
            }

            // Вставляем в кэш
            user_cache["year_group"].push_back(year_group);

            // Генерация и Вставка кнопки
            button[0]["action"]["label"]   = str__institute_year;
            button[0]["action"]["payload"] = to_string(year_group);
            mesg_result["keyboard"]["buttons"].push_back(button);

            // Генерация и Вставка текста (строчки)
            mesg_result["text"] = string(mesg_result["text"])
                    + "\n" + to_string(user_cache["year_group"].size()) + ". " + str__institute_year;
        };

        // Добавляем кнопку назад
        mesg_result["keyboard"]["buttons"].push_back(mesg_result["keyboard"]["buttons"][1]);

        // Добавляем пункт назад
        mesg_result["text"] = string(mesg_result["text"]) + "\n0. Назад";

        // Удаляем шаблонные кнопки
        mesg_result["keyboard"]["buttons"].erase(0);
        mesg_result["keyboard"]["buttons"].erase(0);
    }

    // --------------------------------------------------------------------

    else {
        mesg_result["text"] = "Нет такого варианта ответа, напиши число из меню 👆";
    }

    data_base::set_user_cache(peer_id, user_cache);
    data_base::set_user_stage(peer_id, result);


    return mesg_result;
}

///////////////////////////////////////////////////////////////////////////

json stage :: setting_timetable_lab_group (const json& message) {
    // Получение peer_id (ID пользователя) и текущий stage
    uint peer_id = message["peer_id"];
    string result = data_base::get_user_stage(peer_id);

    json user_cache = data_base::get_user_cache(peer_id);
    uint institute  = user_cache["institute"];
    uint year_group = user_cache["year_group"];
    uint num_group  = user_cache["num_group"];
    uint num_lab_group;

    json mesg_result;

    // Если есть "payload" (нажата кнопка)
    if (message.count("payload")) {
        

        if (string(message["payload"]) != "404") {

            num_lab_group = stoi(string(message["payload"]));

            result = "menu_user";
        }
        
        else {
            result = "setting_timetable_group";
        }
    }
    
    // Если было выбранно число из пунктов меню
    else {
        if (message["text"] != "0") {

            for (int i = 0; i < user_cache["lab_group"].size(); i++) {
                if (message["text"] == to_string(i + 1)) {
                    result = "menu_user";
                    num_lab_group = i + 1;
                    break;
                }
            }
        }
        
        else {
            result = "setting_timetable_group";
        }

    }

    // --------------------------------------------------------------------


    if (result == "menu_user") {
        data_base::add_lesson(peer_id, institute * 10000 + year_group * 100 + num_group, num_lab_group);

        easy::vkapi::messages_send(string("Расписание установлено! 🥳"), peer_id);
        mesg_result = stage::message["menu_user"];
        user_cache = json::parse("{}");
    } else

    // Переход на предыдущий этап, генерация для него меню
    if (result == "setting_timetable_group") {
        user_cache.erase("lab_group");
        user_cache.erase("num_group");

        mesg_result = stage::message["setting_timetable_group"];

        json button = mesg_result["keyboard"]["buttons"][0];

        uint count = 0;

        // Пробигаем по всем доступным институтам, добавляем их в сообщение
        data_base::db
        << "SELECT num FROM group_stankin WHERE (institute = ?) AND (year = ?) ;"
        << institute << year_group
        >> [&mesg_result, &button, &user_cache, &count, institute, year_group] (int num_group) {
            // Генерируем институт+год_начала_обучения
            string str__institute_year_number;
            {
                switch (institute) {
                    case 10: str__institute_year_number = "ИДБ-"; break;
                    case 20: str__institute_year_number = "АДБ-"; break;
                    case 30: str__institute_year_number = "МДБ-"; break;
                    case 40: str__institute_year_number = "ЭДБ-"; break;
                }

                str__institute_year_number += to_string(year_group);
                str__institute_year_number += "-" + to_string(num_group);
            }

            // Вставляем в кэш
            user_cache["num_group"].push_back(num_group);

            // Генерация кнопки
            button[0]["action"]["label"]   = str__institute_year_number;
            button[0]["action"]["payload"] = to_string(num_group);

            // Вставка кнопки
            if (count % 2) {
                mesg_result["keyboard"]["buttons"][(count / 2) + 2].push_back(button[0]);
            } else {
                mesg_result["keyboard"]["buttons"].push_back(button);
            }
            

            // Генерация и Вставка текста (строчки)
            mesg_result["text"] =
                string(mesg_result["text"]) + "\n" + to_string(count + 1) + ". " + str__institute_year_number;

            count++;
        };

        // Добавляем кнопку назад
        mesg_result["keyboard"]["buttons"].push_back(mesg_result["keyboard"]["buttons"][1]);

        // Добавляем пункт назад
        mesg_result["text"] = string(mesg_result["text"]) + "\n0. Назад";

        // Удаляем шаблонные кнопки
        mesg_result["keyboard"]["buttons"].erase(0);
        mesg_result["keyboard"]["buttons"].erase(0);
    }

    // --------------------------------------------------------------------

    else {
        mesg_result["text"] = "Нет такого варианта ответа, напиши число из меню 👆";
    }

    data_base::set_user_cache(peer_id, user_cache);
    data_base::set_user_stage(peer_id, result);


    return mesg_result;
}

///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////