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

// Библиотека для работы с SQLite 3 & C++
#include "sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

// VK API
#include "vkAPI/support/very_eassy_curl.hpp"
#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

#include "data_base.hpp"

#include "date.hpp"

#include "main.hpp"

#include "stage.hpp"


// токен и id сообщества
vkapi::token_group stankin_bot(string("c7364e48cab5cbd2ae3268104fb95d7b8dfa830431a664f256bf9dae36b31685efef421173ac8f784076f"), 193038255);



////////////////////////////////////////////////////////////////////////

bool its_text_message(const json& message) {
    if (!message.count("text")) { return false; }
    if (!static_cast<string>(message["text"]).size()) { return false; }

    if (message.count("attachments")) {
        if (message["attachments"].size()) {
            return false;
        }
    }

    if (message.count("fwd_messages")) {
        if (message["fwd_messages"].size()) {
            return false;
        }
    }
    
    if (message.count("reply_message")) {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////

string kill_rus_e(string str) {
    for (unsigned int i = 0; i < str.size(); i++) {
        if ((str[i] == -48) | (str[i] == -47)) {

            if (str[i] == -48) {
                i++;

                if (str[i] == -127) {
                    str[i] = -107;
                }
            }
            
            else {
                i++;

                if (str[i] == -111) {
                    str[i] = -75;
                    str[i - 1] = -48;
                }
            }

        }
    }

    return str;
}

////////////////////////////////////////////////////////////////////////

string to_upper_rus(string str) {
    for (unsigned int i = 0; i < str.size(); i++) {
        if ((str[i] == -48) | (str[i] == -47)) {

            if (str[i] == -48) {
                i++;

                if ((str[i] >= -80) & (str[i] <= -65)) {
                    str[i] = str[i] - 32;
                }
            }
            
            else {
                i++;

                if ((str[i] >= -128) & (str[i] <= -113)) {
                    str[i] = str[i] + 32;
                    i--;
                    str[i] = -48;
                    i++;
                }
            }

        }
    }

    return str;
}

////////////////////////////////////////////////////////////////////////

bool its_rus_name(string surname) {
    string caps_surname = to_upper_rus(kill_rus_e(surname));

    for (unsigned int i = 0; i < caps_surname.size(); i++) {
        if (caps_surname[i] == '-') { continue; }
        if (caps_surname[i] != -48) { return false; } else { i++; }
        if (!((caps_surname[i] >= -112 ) & (caps_surname[i] <= -81))) { return false; }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

    // Инициализация вк-токена и базы данных
    {
        string str_ans;
        int int_ans;

        // АРГУМЕНТ 1: название файла, в котором хранится токен бота,
        // к сообществу ВКонтакте которому он принадлжеит
        if (argc > 1) { ifstream(argv[1]) >> str_ans; }
        else { cout << "Введите токен сообщества вашего бота: "; cin >> str_ans; }

        // АРГУМЕНТ 2: название файла, в котором хранится ID сообщества ВКонтакте,
        // к которому принадлежит бот
        if (argc > 2) { ifstream(argv[2]) >> int_ans; }
        else { cout << "Введите ID сообщества вашего бота: "; cin >> int_ans; }

        // [Изменить]
        // создание нового токена
        // старый_токен = новый_токен

        // АРГУМЕНТ 3: название файла БД (SQLite 3)
        if (argc > 3) { ifstream(argv[3]) >> str_ans; }
        else { cout << "Введите название файла базы данных (SQLite 3): "; cin >> str_ans; }

        // Установка базы данных
        sqlite::database test_db(argv[3]); data_base::db = test_db;
    }


    // Заполнение stage::message
    {
        json   message_json;
        string message_name;

        for (auto& p : std::__fs::filesystem::directory_iterator("data/stages")) {
            if (p.path().extension().string() == ".json") {
                message_name = p.path().stem().string();
                std::ifstream(p.path()) >> message_json;

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

    }

    


    // Остальное
    json ans_longpoll_json;

    auto test_blp = stankin_bot.groups_getLongPollServer();
    vkapi::token_group& test_token = stankin_bot;

    // Самый главный цикл 💪😎
    while(true) {

        // Собираем события от Bots Long Poll API 📩
        ans_longpoll_json = test_blp.request_lp();

        // Обработка ошибок в ответе от Bots Long Poll API 📛
        if (ans_longpoll_json.count("failed")) {
            // Ошибка связанная с ts (1)
            if (ans_longpoll_json["failed"] == 1) {
                test_blp.set_ts(ans_longpoll_json["ts"]);
                continue;
            } else
            
            // Ошибка связанная с key (2)
            if (ans_longpoll_json["failed"] == 2) {
                test_blp = test_token.groups_getLongPollServer();
                continue;
            } else

            // Ошибка связанная с key и ts (3)
            if (ans_longpoll_json["failed"] == 3) {
                test_blp = test_token.groups_getLongPollServer();
                continue;
            }
            
            else {
                cout << "[Критическая ошибка] "
                        "Неизвестный номер ошибки в ответе Bots Long Poll" << endl;
                break;
            }
        }
        /*
        // Обработка новых сообщений 🔄
        for (unsigned int i = 0; ans_longpoll_json["updates"].size() > i; i++) {
            const json message = move(ans_longpoll_json["updates"][i]["object"]["message"]);
            uint peer_id = message["peer_id"];
            json result_mesg;

            // Заглушка от сообщества 
            if (peer_id >= 2000000000) { continue; } else

            // Добавление нового пользователя 
            if (!data_base::check_user_id(peer_id)) {
                data_base::add_user(peer_id);
                result_mesg = stage::message["menu_guest"];
            } else

            // Проверка на Текстовое сообщение
            if (!its_text_message(message)) {
                result_mesg["text"] = "Я понимаю только номера пунктов меню и нажатия на кнопки 🤷‍♀️"; 
            } else {
                // Обработка сообщения
                result_mesg = stage::function[data_base::get_user_stage(peer_id)](message);
            }

            

            // Запись random_id    
            {
                uint random_id;

                data_base::db << "SELECT random_id FROM user WHERE id = ? ;"
                << peer_id
                >> random_id;

                random_id++;

                data_base::db << "UPDATE user SET random_id = ? WHERE id = ? ;"
                << random_id
                << peer_id;

                result_mesg["random_id"] = random_id;
            }
                
            cout << result_mesg.dump(1) << endl;
            cout << stankin_bot.messages_send(peer_id, result_mesg) << endl;
            
        }
        */


        // Если началось время следующей пары (Рассылка уведомлений о занятий)

        if (time_university::last_number_lesson != time_university::get_current_number_lesson()) {
            cout << 's' << endl;
            time_university::last_number_lesson = time_university::get_current_number_lesson();

            // Проверка на то, что это пара, а не конец учебного дня
            if (time_university::last_number_lesson) {
                auto vec_lesson = data_base::get_cur_less(
                    time_university::last_number_lesson,
                    time_university::get_current_date()
                );

                cout << time_university::last_number_lesson << endl << time_university::get_current_date() << endl;

                cout << vec_lesson.size() << endl;

                for (auto i: vec_lesson) {
                    stankin_bot.messages_send(uint(i["user"]["id"]), i["lesson"].dump(0));
                }
            }
        }

        // Устанавливаем свежиий идентификатор сообытий 🛠
        test_blp.set_ts(stoi(std::string(ans_longpoll_json["ts"])));
    }


    return 0;
}