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

#include "../vkAPI/support/very_eassy_curl.hpp"

#include "../vkAPI/vk_api.hpp"
#include "../vkAPI/long_poll.hpp"
#include "../vkAPI/token_vk.hpp"

#include "functions.hpp"


////////////////////////////////////////////////////////////////////////

map<unsigned int, data_base::user::info> data_base::users::data;
filesystem::path data_base::users::data_path      = "data/" "users/" "database_user.txt";
filesystem::path data_base::users::changelog_path = "data/" "users/" "changelog_user.txt";
map<unsigned int, json> data_base::users::hash;

////////////////////////////////////////////////////////////////////////

map<string, std::function<void(json)>> stage::function;
map<string, json>                      stage::message;

////////////////////////////////////////////////////////////////////////

const string my_token = "c7364e48cab5cbd2ae3268104fb95d7b8dfa830431a664f256bf9dae36b31685efef421173ac8f784076f";
const unsigned int group_id = 193038255;
vkapi::token_group test_token(my_token, group_id);
vkapi::bots_long_poll test_blp = test_token.groups_getLongPollServer();

////////////////////////////////////////////////////////////////////////

map<string, json> ready_mesg;
vector<unsigned int> notification;

////////////////////////////////////////////////////////////////////////

void general::upload() {
    setlocale(LC_ALL, "ru_RU.utf-8");
    srand(time(NULL));
    
    // Заполнение stage::message и ready_mesg
    {
        json   message_json;
        string message_name;

        for (auto& p : filesystem::directory_iterator("data/messages")) {
            if (p.path().extension().string() == ".json") {
                message_name = p.path().stem().string();
                fstream(p) >> message_json;

                ready_mesg.insert({message_name, message_json});
            }
        }

        for (auto& p : filesystem::directory_iterator("data/stages")) {
            if (p.path().extension().string() == ".json") {
                message_name = p.path().stem().string();
                fstream(p) >> message_json;

                stage::message.insert({message_name, message_json});
            }
        }
    }

    // Заполнение stage::function
    {
        stage::function.insert({"menu_guest", stage::menu_guest});
        stage::function.insert({"menu_user", stage::menu_user});

        stage::function.insert({"setting_timetable_institute", stage::setting_timetable_institute});
        stage::function.insert({"setting_timetable_year", stage::setting_timetable_year});
        stage::function.insert({"setting_timetable_group", stage::setting_timetable_group});
        stage::function.insert({"setting_timetable_lab_group", stage::setting_timetable_lab_group});

        stage::function.insert({"search_teacher_input", stage::search_teacher_input});
    }

    // Редактирование stage::message["setting_timetable_institute"]
    {
        json button =
            stage::message["setting_timetable_institute"]
                          ["keyboard"]["buttons"][0];

        const json button_cancel =
            stage::message["setting_timetable_institute"]
                          ["keyboard"]["buttons"][1];

        stage::message["setting_timetable_institute"]["keyboard"]["buttons"].erase(1);

        // Кол-во институтов
        unsigned int count_institute = 1;
        
        for (auto& ptr_institute : filesystem::directory_iterator("data/timetables")) {
            // Название института группы в названии
            string name_institute = ptr_institute.path().stem().string();

            if (!ptr_institute.is_directory()) { continue; }
            if ((name_institute == ".DS_Store") | (name_institute == ".vscode")) { continue; }

            // Добавляем кнопку
            button[0]["action"]["label"] = name_institute;
            button[0]["action"]["payload"] = to_string(count_institute);
            stage::message["setting_timetable_institute"]["keyboard"]["buttons"].push_back(button);

            // Добавляем текст
            string text = stage::message["setting_timetable_institute"]["text"];
            text.push_back('\n');
            text.push_back(static_cast<char>('0' + count_institute));
            text.push_back('.');  text.push_back(' ');
            text += name_institute;
            stage::message["setting_timetable_institute"]["text"] = text;

            count_institute++;
        }

        // Удаляем холст-кнопку
        stage::message["setting_timetable_institute"]["keyboard"]["buttons"].erase(0);

        // Добавляем кнопку Назад
        stage::message["setting_timetable_institute"]["keyboard"]["buttons"].push_back(button_cancel);
        stage::message["setting_timetable_institute"]["keyboard"]["buttons"].back()
                      [0]["action"]["payload"] = to_string(count_institute);

        // Добавляем пункт Назад в текст сообщения
        string text = stage::message["setting_timetable_institute"]["text"];
        text += "\n0. Назад";
        stage::message["setting_timetable_institute"]["text"] = text;
    }

    // Редактирование stage::message["setting_timetable_year"]
    {
        json button =
            stage::message["setting_timetable_year"]
                          ["keyboard"]["buttons"][0];

        const json button_cancel =
            stage::message["setting_timetable_year"]
                          ["keyboard"]["buttons"][1];

        stage::message["setting_timetable_year"]["keyboard"]["buttons"].erase(1);
        
        // Создание клавиатур для каждого института
        for (auto& ptr_institute : filesystem::directory_iterator("data/timetables")) {
            string name_institute = ptr_institute.path().stem().string();

            if (!ptr_institute.is_directory()) { continue; }
            if ((name_institute == ".DS_Store") | (name_institute == ".vscode")) { continue; }

            // Название института группы в названии
            string name_message = string("setting_timetable_year_") + name_institute;
            // Кол-во годов групп
            unsigned int count_year = 1;

            stage::message.insert({name_message, stage::message["setting_timetable_year"]});

            // Добавление кнопочек и пунктов в текст
            for (auto& ptr_year : filesystem::directory_iterator(ptr_institute)) {
                if (!ptr_year.is_directory()) { continue; }
                // Название института группы в названии
                string name_groups = ptr_year.path().stem().string();

                if ((name_groups == ".DS_Store") | (name_groups == ".vscode")) { continue; }
                
                // Добавляем кнопку
                button[0]["action"]["label"] = name_groups;
                button[0]["action"]["payload"] = to_string(count_year);
                stage::message[name_message]["keyboard"]["buttons"].push_back(button);

                // Добавляем текст
                string text = stage::message[name_message]["text"];
                text.push_back('\n');
                text += to_string(count_year) + ". " + name_groups;
                stage::message[name_message]["text"] = text;

                count_year++;
            }

              
            stage::function.insert({name_message, stage::function["setting_timetable_year"]});

            // Удаляем холст-кнопку
            stage::message[name_message]["keyboard"]["buttons"].erase(0);

            // Добавляем кнопку Назад
            stage::message[name_message]["keyboard"]["buttons"].push_back(button_cancel);
            stage::message[name_message]["keyboard"]["buttons"].back()[0]["action"]["payload"] = to_string(count_year);

            // Добавляем пункт Назад в текст сообщения
            string text = stage::message[name_message]["text"];
            text += "\n0. Назад";
            stage::message[name_message]["text"] = text;
        }

    }

    // Редактирование stage::message["setting_timetable_group"]
    {
        json button =
            stage::message["setting_timetable_group"]
                          ["keyboard"]["buttons"][0];

        const json button_cancel =
            stage::message["setting_timetable_group"]
                          ["keyboard"]["buttons"][1];

        stage::message["setting_timetable_group"]["keyboard"]["buttons"].erase(1);
        
        // Перебираем институты
        for (auto& ptr_institute : filesystem::directory_iterator("data/timetables")) {
            string name_institute = ptr_institute.path().stem().string();

            if (!ptr_institute.is_directory()) { continue; }
            if ((name_institute == ".DS_Store") | (name_institute == ".vscode")) { continue; }

            // Перебираем потоки
            for (auto& ptr_year : filesystem::directory_iterator(ptr_institute)) {
                string name_groups = ptr_year.path().stem().string();

                if (!ptr_year.is_directory()) { continue; }
                if ((name_groups == ".DS_Store") | (name_groups == ".vscode")) { continue; }

                unsigned int count_group = 1;

                string name_message = string("setting_timetable_group_") + name_groups;
                stage::message.insert({name_message, stage::message["setting_timetable_group"]});

                // Создаем клавиатуру для этого потока с выбором номера группы
                for (auto& ptr_group : filesystem::directory_iterator(ptr_year)) {
                    // Название института группы в названии
                    string name_group = ptr_group.path().stem().string();

                    if (ptr_group.is_directory()) { continue; }
                    if ((name_group == ".DS_Store") | (name_group == ".vscode")) { continue; }
                    
                    // Добавляем кнопку
                    button[0]["action"]["label"] = name_group;
                    button[0]["action"]["payload"] = to_string(count_group);

                    if (count_group % 2) {
                        stage::message[name_message]["keyboard"]["buttons"].push_back(button);
                    }

                    else {
                        stage::message[name_message]["keyboard"]["buttons"][count_group / 2].push_back(button[0]);
                    }

                    // Добавляем текст
                    string text = stage::message[name_message]["text"];
                    text.push_back('\n');
                    text += to_string(count_group) + ". " + name_group;
                    stage::message[name_message]["text"] = text;

                    count_group++;
                }

                stage::function.insert({name_message, stage::function["setting_timetable_group"]});

                // Удаляем холст-кнопку
                stage::message[name_message]["keyboard"]["buttons"].erase(0);

                // Добавляем кнопку Назад
                stage::message[name_message]["keyboard"]["buttons"].push_back(button_cancel);
                stage::message[name_message]["keyboard"]["buttons"].back()[0]["action"]["payload"] = to_string(count_group);

                // Добавляем пункт Назад в текст сообщения
                string text = stage::message[name_message]["text"];
                text += "\n0. Назад";
                stage::message[name_message]["text"] = text;
            }

        }

    }

    // Стартовые обязательные операции для data_base::users
    {
        // Создаем копии БД'шек
        file::dublicate(data_base::users::data_path, "data/dublicate");
        file::dublicate(data_base::users::changelog_path, "data/dublicate");

        // Загружаем пользователей в ОЗУ
        data_base::users::download_data();

        // Если остались изменения в data_base::users::changelog
        if (!file::empty(data_base::users::changelog_path)) {
            // Загружаем в ОЗУ изменения
            data_base::users::download_changelog();

            // Очистка changelog
            file::clear(data_base::users::changelog_path);
            data_base::users::upload_data();
        }

        // Определяем БД кэша пользователей
        for (auto iter : data_base::users::data) {
            data_base::users::hash.insert({iter.first, json()});
        }
    }

    // Определяем БД кэша пользователей
    for (auto iter : data_base::users::data) {
        data_base::users::hash.insert({iter.first, json()});

        // Возвращение в меню (через ошибку в stage)
        if ((iter.second.stage != what_menu(iter.first)) & (iter.second.stage != "error")) {
            data_base::users::data[iter.first].stage = "error";
            data_base::user::push_changelog(iter.first);
        }
    }

}

////////////////////////////////////////////////////////////////////////

void general::refresh() {

    
}

////////////////////////////////////////////////////////////////////////