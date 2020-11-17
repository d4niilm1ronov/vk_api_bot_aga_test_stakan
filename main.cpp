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

#include "additionally.hpp"

#include "stage.hpp"


///////////////////////////////////////////////////////////////////////////


// токен и id сообщества
vkapi::token_group stankin_bot("MDA", 180900);


///////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]) {


    // Инициализация вк-токена и базы данных
    {
        if (argc < 2) { cout << "Список аргументов: json__name_confing [, db3__name_data_base]" << endl; exit(1); }
        if (argc > 3) { cout << "Список аргументов: json__name_confing [, db3__name_data_base]" << endl; exit(1); }

        // Работа с токеном ВКонтакте
        json json__config;
        ifstream(argv[1]) >> json__config;

        stankin_bot = vkapi::token_group(string(json__config["token"]), uint(json__config["group_id"]));


        // Работа с БД
        string string__name_db;

        if (argc > 2) { string__name_db = argv[2]; }
        else { cout << "Введите название файла базы данных (SQLite 3): "; cin >> string__name_db; }

        // Установка базы данных
        sqlite::database test_db(string__name_db); data_base::db = test_db;
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

    time_stakan::last_number_lesson = time_stakan::get_current_number_lesson();

    // Обновление записей прошедших занятий из таблицы «lesson»
    {
        vector<uint> vec__lesson_id;
        uint date_current = time_stakan::get_current_date().format_mmdd();

        data_base::db << "SELECT id FROM lesson WHERE (date < ? ) OR ((time < ? ) AND (date = ? ));"
        << date_current << time_stakan::last_number_lesson << date_current >> [&vec__lesson_id](unsigned int id) {
            vec__lesson_id.push_back(id);
        };

        data_base::db << "SELECT id FROM lesson_stankin WHERE (date < ? );"
        << date_current >> [&vec__lesson_id](unsigned int id) {
            vec__lesson_id.push_back(id);
        };

        for (auto id: vec__lesson_id) { data_base::update_lesson(id); }
    }
    

    // Соединение для работы с Bots LongPoll VK API
    auto bots_longpoll__stankin_bot = stankin_bot.groups_getLongPollServer();

    // Ответ от VK API (по технологии LongPoll) в формате Json
    json json__answer_longpoll;

    auto current_date = time_stakan::get_current_date();


    // Самый главный цикл 💪😎
    while(true) {
        
        // Собираем события от Bots Long Poll API 📩
        json json__answer_longpoll = bots_longpoll__stankin_bot.request_lp();


        // Обработка ошибок в ответе от Bots Long Poll API 📛
        if (json__answer_longpoll.count("failed")) {
            // Ошибка связанная с ts (1)
            if (json__answer_longpoll["failed"] == 1) {
                bots_longpoll__stankin_bot.set_ts(json__answer_longpoll["ts"]);
                continue;
            } else
            
            // Ошибка связанная с key (2)
            if (json__answer_longpoll["failed"] == 2) {
                bots_longpoll__stankin_bot = stankin_bot.groups_getLongPollServer();
                continue;
            } else

            // Ошибка связанная с key и ts (3)
            if (json__answer_longpoll["failed"] == 3) {
                bots_longpoll__stankin_bot = stankin_bot.groups_getLongPollServer();
                continue;
            }
            
            else {
                cout << "[Критическая ошибка] "
                        "Неизвестный номер ошибки в ответе Bots Long Poll" << endl;
                break;
            }
        }


        // Обработка новых сообщений 🔄
        for (unsigned int i = 0; json__answer_longpoll["updates"].size() > i; i++) {
            const json message = move(json__answer_longpoll["updates"][i]["object"]["message"]);
            uint peer_id = message["peer_id"];

            // Заглушка от сообщества 
            if (peer_id >= 2000000000) { continue; } else

            // Добавление нового пользователя 
            if (!data_base::check_user_id(peer_id)) {
                data_base::add_user(peer_id);
                easy::vkapi::messages_send(stage::message["menu_guest"], peer_id);
            } else

            // Проверка на Текстовое сообщение
            if (!its_text_message(message)) {
                easy::vkapi::messages_send(string("Я понимаю только номера пунктов меню и нажатия на кнопки 🤷‍♀️"), peer_id); 
            } else {
                // Обработка сообщения
                stage::function[data_base::get_user_stage(peer_id)](message);
            }
        }


        // Если началось время следующей пары (Рассылка уведомлений о занятий)
        if (time_stakan::last_number_lesson != time_stakan::get_current_number_lesson()) {

            time_stakan::last_number_lesson = time_stakan::get_current_number_lesson();

            // Проверка на то, что это пара, а не конец учебного дня
            if ((time_stakan::last_number_lesson > 0) and (time_stakan::last_number_lesson < 9)) {
                // Получаю в Вектор занятия, о которых нужно предупредить
                auto vector__lesson_user = data_base::get_lesson(
                    time_stakan::get_current_date().format_mmdd(),
                    time_stakan::last_number_lesson
                );


                // Рассылка сообщений об предстоящем занятии    
                for (auto i: vector__lesson_user) {
                    string text = "Следующее занятие 👩‍🏫\n\n";
                    text = text + string(i["lesson"]["name"]);
                    

                    if (int(i["lesson"]["type"]) == 1) { text = text + " [Лекция]\n"; } else
                    if (int(i["lesson"]["type"]) == 2) { text = text + " [Семинар]\n"; } else
                                                       { text = text + " [Лабораторная работа]\n"; }

                    if (i["lesson"]["place"] != "null") {
                        text = text + "Аудитория: " + string(i["lesson"]["place"]) + "\n";
                    }

                    if (i["lesson"]["teacher"] != "null") {
                        text = text + "Преподаватель: " + string(i["lesson"]["teacher"]) + "\n";
                    }
                    

                    easy::vkapi::messages_send(text, uint(i["user"]["id"]));
                }


                // Удаление/Обновление записей прошлых занятий из lesson_user
                if (time_stakan::last_number_lesson != 0) {

                    vector<uint> vec__lesson_id;

                    data_base::db << "SELECT id FROM lesson WHERE (time = ? ) AND (date = ? );"
                    << current_date.format_mmdd() << time_stakan::last_number_lesson
                    >> [&vec__lesson_id](unsigned int id) {
                        vec__lesson_id.push_back(id);
                    };

                    for (auto id: vec__lesson_id) { data_base::update_lesson(id); }
                }
            }
        }

        // Если случился переход на другой день
        if (current_date != time_stakan::get_current_date()) {

            // Обновляем занятия с прошлого дня в lesson_stankin
            {
                vector<uint> vec__lesson_id;

                data_base::db << "SELECT id FROM lesson_stankin WHERE (date = ? );" << current_date.format_mmdd()
                >> [&vec__lesson_id](unsigned int id) {
                    vec__lesson_id.push_back(id);
                };

                for (auto id: vec__lesson_id) { data_base::update_lesson(id); }
            }

            // Обновляем переменую с текущей датой
            current_date = time_stakan::get_current_date();
        }

        // Устанавливаем свежиий идентификатор сообытий 🛠
        bots_longpoll__stankin_bot.set_ts(stoi(std::string(json__answer_longpoll["ts"])));
    }


    return 0;
}