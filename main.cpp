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

#include "vkAPI/support/very_eassy_curl.hpp"

#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

#include "functions/functions.hpp"



////////////////////////////////////////////////////////////////////////

int main() {
    // Остальное
    json ans_longpoll_json;

    general::upload();

    cout << stage::message.size() << endl;

    // Самый главный цикл 💪😎
    while(true) {
        bool stop_flag = false;
        
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
            if (ans_longpoll_json["failed"] == 2 ) {
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

        // Обработка новых сообщений 🔄
        for (unsigned int i = 0; ans_longpoll_json["updates"].size() > i; i++) {
            const json message = move(ans_longpoll_json["updates"][i]["object"]["message"]);
            unsigned int peer_id = message["peer_id"];

            // Добавление нового пользователя 
            if (!data_base::user::check(peer_id)) {
                data_base::user::add(peer_id);
                data_base::users::hash.insert({peer_id, json()});
                test_token.messages_send(peer_id, ready_mesg["welcome"]);
                test_token.messages_send(peer_id, stage::message["menu_guest"]);
                continue;
            }

            // Вывод меню (от Перезапуска бота)
            if (data_base::users::data[peer_id].stage == "error") {
                data_base::users::data[peer_id].stage = what_menu(peer_id);

                json answer_message;
                answer_message["text"] = "Был перезапуск бота, Вас вернули в безопасное место - Меню 😌";
                test_token.messages_send(peer_id, answer_message);

                test_token.messages_send(peer_id, stage::message[data_base::users::data[peer_id].stage]);

                data_base::user::push_changelog(peer_id);
                continue;
            }

            // Проверка на Текстовое сообщение
            if (!its_text_message(message)) {
                json answer_message;
                answer_message["text"] = "Я понимаю только символы и нажатия на кнопки 🥴";
                test_token.messages_send(peer_id, answer_message); 
                continue;
            }

            // Стоп-слово
            if (message["text"] == std::string("стоп")) {
                stop_flag = true; break;
            }

            // Обработка сообщения
            stage::function[data_base::users::data[peer_id].stage](message);
        }
        
        auto temp_date = date::get_current_date();
        cout << temp_date.tm_mday << "." <<  temp_date.tm_mon + 1 << "." << temp_date.tm_year + 1900 << " " << temp_date.tm_hour << ":" << temp_date.tm_min << ":" << temp_date.tm_sec  << " (" << temp_date.tm_yday  << ")" << endl;

        // Рассылка уведомлений о перерыве 🛎
        if (date::time_to_break()) {
            for (auto peer_id : notification) {
                cout << 2222 << endl;
                test_token.messages_send(peer_id, ready_mesg["break"]);
            }

            notification.clear();
        }

        // Рассылка уведомлений о следующем занятии ⏰
        if (date::current_lesson != date::get_stage_tt()) {
            cout << "ПАРА" << endl;
            auto stage_tt = date::get_stage_tt();
            int tm_yday = date::get_current_date().tm_yday + 1;
            
            if (date::get_stage_tt() != 0) {

                cout << "ПАРА2" << endl;

                for (auto user : data_base::users::data) {
                    if (user.second.send_id == 0) { continue; }
                    string path_tt = string("data/users/tt") + to_string(user.first) + "/" + date::arr_wday[date::get_current_date().tm_wday];
                    cout << path_tt << endl;
                    json json_tt; ifstream(path_tt) >> json_tt;
                    
                    for (int i = 0; i < json_tt.size(); i++) {
                        // Если занятие подходит по времени
                        if (stage_tt == json_tt[i]["time"]) {
                            cout << "find" << endl;

                            cout << int(json_tt[i]["dates"][0]) << " vs " << tm_yday << endl;

                            // Нахождение (почему-то) не отправленных пар
                            if (json_tt[i]["dates"][0] < tm_yday) {
                                cout << "kill" << endl;
                                for (int i = 0; json_tt[i]["dates"].size() > i; i++) {
                                    if (json_tt[i]["dates"][0] >= tm_yday) { break; }
                                    json_tt[i]["dates"].erase(0);
                                }

                                if (json_tt[i]["dates"].empty()) {
                                    json_tt.erase(i);
                                    ofstream(path_tt, ios::trunc) << json_tt;
                                    continue;
                                }

                                ofstream(path_tt, ios::trunc) << json_tt;
                            }


                            // Если занятие подходит еще и по дате
                            if (json_tt[i]["dates"][0] == tm_yday) {
                                cout << "FIND!" << endl;
                                json lesson = json_tt[i];
                                json mesg;
                                string text;

                                // Удаляем из расписания пользователя это занятие
                                json_tt[i]["dates"].erase(0);
                                // Если дат больше нет по этому занятию - удаляем занятие
                                if (json_tt[i]["dates"].empty()) { json_tt.erase(i); }
                                // Сохраняем такое расписание
                                ofstream(path_tt, ios::trunc) << json_tt;

                                // Отправка номера занятия
                                if (stage_tt == 1) { text = "Первое занятие (08:30 - 10:10)"; } else
                                if (stage_tt == 2) { text = "Второе занятие (10:20 - 12:00)"; } else
                                if (stage_tt == 3) { text = "Третье занятие (12:20 - 14:00)"; } else
                                if (stage_tt == 4) { text = "Четвертое занятие (14:10 - 15:50)"; } else
                                if (stage_tt == 5) { text = "Пятое занятие (16:00 - 17:40)"; } else
                                if (stage_tt == 6) { text = "Шестое занятие (18:00 - 19:30)"; } else
                                if (stage_tt == 7) { text = "Седьмое занятие (19:40 - 21:10)"; } else
                                                   { text = "Восьмое занятие (21:20 - 22:50)"; }

                                mesg["text"] = text; text.clear();
                                cout << test_token.messages_send(user.second.send_id, mesg) << endl;

                                // Отправка информации о занятии
                                text = string(lesson["name"]);

                               
                                if (lesson.count("room")) {
                                     text += "\n📌 Ауд. ";
                                    if (lesson["room"].is_string()) {
                                        text += string(lesson["room"]);
                                    } else {
                                        // 📌 ДОБАВИТЬ ДОБАВИТЬ ДОБАВИТЬ 📌
                                    }
                                }

                                if (lesson["type_lesson"] == 1) {
                                    text = text + "\n📌 Лекция";
                                } else
                                if (lesson["type_lesson"] == 2) {
                                    text = text + "\n📌 Cеминар";
                                } else {
                                    text = text + "\n📌 Лабораторные занятия";
                                    if (lesson.count("group")) {
                                        text = text + "\n📌 " + string(lesson["group"]);
                                    }
                                }

                                if (lesson.count("teacher")) {
                                    text = text + "\n📌 " + string(lesson["teacher"]);
                                }
                                
                                mesg["text"] = text;
                                cout << test_token.messages_send(user.second.send_id, mesg) << endl;

                                // Запоминаем напомнить об перерыве
                                notification.push_back(user.second.send_id);
                            }
                        } else

                        if (stage_tt < json_tt[i]["time"]) { break; }
                    }
                }
            }

            date::current_lesson = stage_tt;
        }

        if (stop_flag) { break; }

        // Устанавливаем свежиий идентификатор сообытий 🛠
        test_blp.set_ts(stoi(std::string(ans_longpoll_json["ts"])));
    }

    // Правильное выключение
    data_base::users::upload_data();
    
    

    return 0;
}