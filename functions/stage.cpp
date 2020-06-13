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

using namespace std;
using json = nlohmann::json;

#include "../vkAPI/support/very_eassy_curl.hpp"

#include "../vkAPI/vk_api.hpp"
#include "../vkAPI/long_poll.hpp"
#include "../vkAPI/token_vk.hpp"

#include "functions.hpp"

////////////////////////////    [ menu ]    ////////////////////////////

void stage::menu_guest(const json& message) {
    unsigned int peer_id = message["peer_id"];
    string result = data_base::users::data[peer_id].stage;

    if (message.count("payload")) {
        if (message["payload"] == "1") {
            result = "setting_timetable_institute";
        }
    }

    if (message["text"] == "1") {
        result = "setting_timetable_institute";
    }

    if (result != data_base::users::data[peer_id].stage) {
        data_base::users::data[peer_id].stage = result;
        data_base::user::push_changelog(peer_id);
        test_token.messages_send(peer_id, stage::message[result]);
    }
    
    else {
        json answer_message;
        answer_message["text"] = "Нет такого варианта ответа, напиши цифру из Меню 🙄";
        test_token.messages_send(peer_id, answer_message);
    }
}

//----------------------------------------------------------------------

void stage::menu_user(const json& message) {
    unsigned int peer_id = message["peer_id"];
    string result = data_base::users::data[peer_id].stage;

    if (message.count("payload")) {
        if (message["payload"] == "1") {
            result = "setting_timetable_institute";
        }

        if (message["payload"] == "2") {
            result = "menu_guest";
        }
    }

    if (message["text"] == "1") {
        result = "setting_timetable_institute";
    }

    if (message["text"] == "2") {
        result = "menu_guest";
    }

    if (result != data_base::users::data[peer_id].stage) {
        if (result == "del") {
            json mesg; mesg["text"] = "Вы отписаны от расписания 😔";
            test_token.messages_send(peer_id, mesg);
            file::clear_folder(string("data/users/tt") + to_string(peer_id));
            
            data_base::users::data[peer_id].send_id = 0;
            data_base::users::data[peer_id].level = 0;
        }

        data_base::users::data[peer_id].stage = result;
        data_base::user::push_changelog(peer_id);
        test_token.messages_send(peer_id, stage::message[result]);
    }
    
    else {
        json answer_message;
        answer_message["text"] = "Нет такого варианта ответа, напиши цифру из Меню 🙄";
        test_token.messages_send(peer_id, answer_message);
    }
}

/////////////////////    [ setting_timetable ]    //////////////////////

void stage::setting_timetable_institute(const json& message) {
    string result = "setting_timetable_institute";
    unsigned int peer_id = message["peer_id"];

    unsigned int size =
        stage::message["setting_timetable_institute"]["keyboard"]["buttons"].size();

    string cancel_label = stage::message["setting_timetable_institute"]
                            ["keyboard"]["buttons"].back()[0]["action"]["label"];

    // Если есть payload
    if (message.count("payload")) {
        unsigned int payload = stoi(static_cast<string>(message["payload"]));

        if ((payload >= (size % 1)) & (payload <= size)) {
            if (payload != size) {
                result = stage::message["setting_timetable_institute"]
                    ["keyboard"]["buttons"][payload - 1][0]["action"]["label"];
            } else {
                result = cancel_label;
            }
        }
    }

    // Если нет payload
    else {
        string text = message["text"];
        
        if (text.size() == 1) {
            for (unsigned int i = 0; size > i; i++) {
                if (text == to_string(i)) {
                    if (i != 0) {
                        result = stage::message["setting_timetable_institute"]
                            ["keyboard"]["buttons"][i - 1][0]["action"]["label"];
                    } else {
                        result = cancel_label;
                    }
                }
            }
        }
    }

    if (result != "setting_timetable_institute") {
        // Пользователь возвращается Назад
        if (result == cancel_label) {
            //  📌 ДОБАВИТЬ ДОБАВИТЬ ДОБАВИТЬ 📌
            data_base::users::data[peer_id].stage = what_menu(peer_id);
            data_base::users::hash[peer_id].clear();
        }

        // Пользователь сделал правильное действие
        else {
            data_base::users::data[peer_id].stage = string("setting_timetable_year_") + result;

            if (!data_base::users::hash[peer_id].count("new_send_id")) {
                data_base::users::hash[peer_id]["new_send_id"] = peer_id; 
            }
        }

        data_base::user::push_changelog(peer_id);
        test_token.messages_send(peer_id, stage::message[data_base::users::data[peer_id].stage]);
    }
    
    else {
        json answer_message;
        answer_message["text"] = "Нет такого варианта ответа, напиши цифру из Меню 🙄";
        test_token.messages_send(peer_id, answer_message);
    }
}

//----------------------------------------------------------------------

void stage::setting_timetable_year(const json& message) {
    unsigned int peer_id = message["peer_id"];
    string result = data_base::users::data[peer_id].stage;
    string stage = result;

    unsigned int size = stage::message[stage]["keyboard"]["buttons"].size();

    string cancel_label = stage::message[stage]["keyboard"]["buttons"].back()[0]["action"]["label"];

    // Если есть payload
    if (message.count("payload")) {
        unsigned int payload = stoi(static_cast<string>(message["payload"]));

        if ((payload >= (size % 1)) & (payload <= size)) {
            if (payload != size) {
                result = stage::message[stage]["keyboard"]["buttons"][payload - 1][0]["action"]["label"];
            } else {
                result = cancel_label;
            }
        }
    }

    // Если нет payload
    else {
        string text = message["text"];
        
        if (text.size() == 1) {
            for (unsigned int i = 0; size > i; i++) {
                if (text == to_string(i)) {
                    if (i != 0) {
                        result = stage::message[stage]["keyboard"]["buttons"][i - 1][0]["action"]["label"];
                    } else {
                        result = cancel_label;
                    }
                }
            }
        }
    }

    if (result != stage) {
        // Пользователь возвращается Назад
        if (result == cancel_label) {
            data_base::users::data[peer_id].stage = "setting_timetable_institute";
        }
        
        // Пользователь сделал правильное действие
        else {
            data_base::users::data[peer_id].stage = string("setting_timetable_group_") + result;
        }

        data_base::user::push_changelog(peer_id);
        test_token.messages_send(peer_id, stage::message[data_base::users::data[peer_id].stage]);
    }
    
    else {
        json answer_message;
        answer_message["text"] = "Нет такого варианта ответа, напиши цифру из Меню 🙄";
        test_token.messages_send(peer_id, answer_message);
    }
}

//----------------------------------------------------------------------

void stage::setting_timetable_group(const json& message) {
    unsigned int peer_id = message["peer_id"];
    string result = data_base::users::data[peer_id].stage;
    string stage = result;

    unsigned int size = stage::message[stage]["keyboard"]["buttons"].size();

    string cancel_label = stage::message[stage]["keyboard"]["buttons"].back()[0]["action"]["label"];

    // Если есть payload
    if (message.count("payload")) {
        unsigned int payload = stoi(static_cast<string>(message["payload"]));

        if ((payload >= (size % 1)) & (payload <= size)) {
            if (payload != size) {
                result = stage::message[stage]["keyboard"]["buttons"][payload - 1][0]["action"]["label"];
            } else {
                result = cancel_label;
            }
        }
    }

    // Если нет payload
    else {
        string text = message["text"];
        
        if ((text.size() == 1) | (text.size() == 2)) {
            for (unsigned int i = 0; size > i; i++) {
                if (text == to_string(i)) {
                    if (i != 0) {
                        result = stage::message[stage]["keyboard"]["buttons"][i - 1][0]["action"]["label"];
                    } else {
                        result = cancel_label;
                    }
                }
            }
        }
    }

    if (result != stage) {
        // Пользователь возвращается Назад
        if (result == cancel_label) {
            data_base::users::data[peer_id].stage = "setting_timetable_group";
            test_token.messages_send(peer_id, stage::message[data_base::users::data[peer_id].stage]);
        }
        
        // Пользователь сделал правильное действие
        else {
            filesystem::path path_timetables;
            string temp_str;

            // Находим путь к файлу (тройной цикл, так держать, чувак 👍)
            for (auto& ptr_institute : filesystem::directory_iterator("data/timetables")) {
                temp_str = ptr_institute.path().stem().string();
                cout << temp_str << endl;
                if ((temp_str == ".DS_Store") | (temp_str == ".vscode") | (!ptr_institute.is_directory())) { continue; }

                for (auto& ptr_year : filesystem::directory_iterator(ptr_institute)) {
                    temp_str = ptr_year.path().stem().string();
                    cout << "   " << temp_str << endl;
                    if ((temp_str == ".DS_Store") | (temp_str == ".vscode") | (!ptr_year.is_directory())) { continue; }

                    for (auto& ptr_group : filesystem::directory_iterator(ptr_year)) {
                        temp_str = ptr_group.path().stem().string();
                        cout << "      " << temp_str << endl;
                        if ((temp_str == ".DS_Store") | (temp_str == ".vscode") | (ptr_group.is_directory())) { continue; }

                        // Нашел расписание
                        if (temp_str == result) { cout << "find" << endl; path_timetables = ptr_group.path(); break; }
                    }
                }
            }

            // Создаем папку
            filesystem::create_directory(string("data/users/tt") + to_string(peer_id));

            // Очищаем (если до этого была создана и там что-то было)
            file::clear_folder(string("data/users/tt") + to_string(peer_id));

            // Загружаем расписание
            json json_timetables;
            ifstream(path_timetables) >> json_timetables;

            // Записываем занятия из готового расписания в расписание пользователя
            for (int i = 0; json_timetables["lessons"].size() > i; i++) {
                ofstream(string("data/users/tt") + to_string(peer_id) + "/" + date::arr_wday[i + 1])
                    << json_timetables["lessons"][i];
            }

            data_base::users::data[peer_id].level = 1;
            data_base::users::data[peer_id].send_id = data_base::users::hash[peer_id]["new_send_id"];
            data_base::users::hash[peer_id].clear();

            json temp_mesg;
            temp_mesg["text"] = "Расписание установлено 😌";
            test_token.messages_send(peer_id, temp_mesg);

            if (data_base::users::data[peer_id].send_id < 2000000000) {
                data_base::users::hash[peer_id]["lab_groups"] = json_timetables["groups"];
                data_base::users::data[peer_id].stage = "setting_timetable_lab_group";

                // Создаем клавиартуру и текст сообщения
                temp_mesg = stage::message["setting_timetable_lab_group"];
                json button = temp_mesg["keyboard"]["buttons"][0];

                for (int i = 0; data_base::users::hash[peer_id]["lab_groups"].size() > i; i++) {
                    button[0]["action"]["label"] = string(data_base::users::hash[peer_id]["lab_groups"][i]);
                    button[0]["action"]["payload"] = to_string(i + 1);
                    temp_mesg["keyboard"]["buttons"].push_back(button);

                    temp_mesg["text"] = string(temp_mesg["text"]) + "\n" + to_string(i + 1) + ". " + string(button[0]["action"]["label"]);
                }

                temp_mesg["keyboard"]["buttons"].erase(0);
                button = temp_mesg["keyboard"]["buttons"][0];
                temp_mesg["keyboard"]["buttons"].erase(0);
                temp_mesg["keyboard"]["buttons"].push_back(button);

                temp_mesg["text"] = string(temp_mesg["text"]) + "\n0. Пропустить";
                cout << test_token.messages_send(peer_id, temp_mesg) << endl;
            } else {
                data_base::users::data[peer_id].stage = what_menu(peer_id);
            }
        }

        data_base::user::push_changelog(peer_id);
    }
    
    else {
        json answer_message;
        answer_message["text"] = "Нет такого варианта ответа, напиши цифру из Меню 🙄";
        test_token.messages_send(peer_id, answer_message);
    }
}

//----------------------------------------------------------------------

void stage::setting_timetable_lab_group(const json& message) {
    unsigned int peer_id = message["peer_id"];
    string result = data_base::users::data[peer_id].stage;
    string stage = result;

    unsigned int size = data_base::users::hash[peer_id]["lab_groups"].size();

    string cancel_label = "12345";

    // Если есть payload
    if (message.count("payload")) {
        if (static_cast<string>(message["payload"]) == cancel_label) {
            result = cancel_label;
        } else {
            for (size_t i = 1; i <= size; i++) {
                if (static_cast<string>(message["payload"]) == to_string(i)) {
                    result = message["payload"];
                }
            }
        }
    }

    // Если нет payload
    else {
        string text = message["text"];

        if (text == "0") { result = cancel_label; }
        
        for (int i = 1; size >= i; i++) {
            if (to_string(i) == text) {
                result = text; break;
            }
        }
    }

    if (result != stage) {
        // Убираем не нужные группы для пользователя
        if (result != cancel_label) {
            string group = data_base::users::hash[peer_id]["lab_groups"][stoi(result) - 1];

            for (auto& ptr_wday : filesystem::directory_iterator(string("data/users/tt") + to_string(peer_id))) {
                if (ptr_wday.path().extension().string() != ".json") { continue; }

                json json_tt; ifstream(ptr_wday.path()) >> json_tt;

                cout << "result group: " << group << endl;

                // Проверяем все занятия
                for (size_t i = 0; i < json_tt.size(); i++) {
                    // Нашли лабораторную работу
                    if (json_tt[i].count("group")) {
                        if (json_tt[i]["group"] == group) { // удаляем занятие
                            json_tt[i].erase("group");
                        } else { // удаляем упоминание о лаб. группе
                            json_tt.erase(i);
                        }
                    }
                }

                ofstream(ptr_wday.path(), ios::trunc) << json_tt;
            }
        }
        
        data_base::users::hash[peer_id].clear();
        data_base::users::data[peer_id].stage = what_menu(peer_id);
        test_token.messages_send(peer_id, stage::message[data_base::users::data[peer_id].stage]);
    }
    
    else {
        json answer_message;
        answer_message["text"] = "Нет такого варианта ответа, напиши цифру из Меню 🙄";
        test_token.messages_send(peer_id, answer_message);
    }
}

//////////////////////    [ search_teacher ]    ////////////////////////

void stage::search_teacher_input(const json& message) {
    string result = "search_teacher_input";
    unsigned int peer_id = message["peer_id"];

    if (message.count("payload")) {
        if (message["payload"] == "1") {
            result = "back";
        }
    } else

    if (message["text"] == "0") {
        result = "back";
    }

    // else
    // ..........

    if (result != "search_teacher_input") {
        if (result == "back") {
            data_base::users::data[peer_id].stage = what_menu(peer_id);
        }

        data_base::user::push_changelog(peer_id);
        test_token.messages_send(peer_id, stage::message[data_base::users::data[peer_id].stage]);
    }
    
    else {
        json answer_message;
        answer_message["text"] = "Нет такого варианта ответа, напиши цифру из Меню или фамилию преподавателя 🙄";
        test_token.messages_send(peer_id, answer_message);
    }
}

////////////////////////////////////////////////////////////////////////

string what_menu(unsigned int peer_id) {
    unsigned int level = data_base::users::data[peer_id].level;

    if (level == 1) { return "menu_user"; }

    return "menu_guest";
}

////////////////////////////////////////////////////////////////////////