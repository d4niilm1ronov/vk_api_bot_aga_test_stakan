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

        vkapi::token_group new_token(str_ans, int_ans);
        stankin_bot = new_token;

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

    
    // Соединение для работы с Bots LongPoll VK API
    auto bots_longpoll__stankin_bot = stankin_bot.groups_getLongPollServer();

    // Ответ от VK API (по технологии LongPoll) в формате Json
    json json__answer_longpoll;


    // Самый главный цикл 💪😎
    while(true) {

        // Собираем события от Bots Long Poll API 📩
        json__answer_longpoll = bots_longpoll__stankin_bot.request_lp();

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
            if (time_stakan::last_number_lesson) {
                // Получаю в Вектор занятия, о которых нужно предупредить
                auto vector__lesson_user = data_base::get_cur_less(
                    time_stakan::last_number_lesson,
                    time_stakan::get_current_date().format_yymmdd()
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

                // Обновляем даты у записей занятий
                for (auto iter: vector__lesson_user) {

                    // Если это последняя дата у занятия
                    if (iter["lesson"]["date"] == iter["lesson"]["date_end"]) {
                        // Удаляем запись этого занятия
                        data_base::db << "DELETE FROM lesson WHERE id = ? ;"
                                      << uint(iter["lesson"]["id"]);
                    }

                    // Если это НЕ последняя дата у занятия
                    else {
                        uint date_YYMMDD = iter["lesson"]["date"];
                        time_stakan::date next_date(date_YYMMDD);

                        // Увеличиваем дату следующего занятия на 1 или 2 недели
                        if (iter["lesson"]["repit"] == 2) { next_date = next_date.plus_two_week(); }
                        else                              { next_date = next_date.plus_one_week(); }

                        // Обновляем запись этого занятия
                        data_base::db << "UPDATE lesson SET date = ? WHERE id = ? ;"
                                      << next_date.format_yymmdd()
                                      << uint(iter["lesson"]["id"]);
                    }

                }
            }
        }


        // Устанавливаем свежиий идентификатор сообытий 🛠
        bots_longpoll__stankin_bot.set_ts(stoi(std::string(json__answer_longpoll["ts"])));
    }


    return 0;
}