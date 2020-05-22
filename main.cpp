#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;



#include "vkAPI/support/very_eassy_curl.hpp"

#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

#include "functions/functions.hpp"

// Стартовые определения для data_base::users
    filesystem::path data_base::users::data_path      = "data/database_user.json";
    map<unsigned int, data_base::user::info> data_base::users::data;

    filesystem::path data_base::users::changelog_path = "data/changelog_user.json";
    fstream data_base::users::changelog(data_base::users::changelog_path, ios::out | ios::in | ios::app);

    map<unsigned int, json> data_base::users::hash;

int main() {
    srand(time(NULL));

    // Стартовые операции для vkAPI
        const std::string my_token = "c7364e48cab5cbd2ae3268104fb95d7b8dfa830431a664f256bf9dae36b31685efef421173ac8f784076f";
        const unsigned int group_id = 193038255;

        vkapi::token_group test_token(my_token, group_id);
        vkapi::bots_long_poll test_blp = test_token.groups_getLongPollServer();

        json ans_longpoll_json; 

    // Стартовые обязательные операции для data_base::users
        // Создаем копии БД'шек
        file::dublicate(data_base::users::data_path, "data/dublicate");
        file::dublicate(data_base::users::changelog_path, "data/dublicate");

        // Загружаем пользователей в ОЗУ
        data_base::users::download_data();

        // Если остались изменения в data_base::users::changelog
        if (!file::empty(data_base::users::changelog_path)) {
            // Загружаем в ОЗУ изменения
            data_base::users::download_changelog();

            // Правильная очистка changelog
            data_base::users::changelog .close();
            file::clear(data_base::users::changelog_path);
            data_base::users::changelog
                .open(data_base::users::changelog_path, ios::out | ios::in | ios::app);
        }

    // Самый главный цикл 💪😎
        while(true) {
            // Собираем события от Bots Long Poll API ❗️
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

            bool stop_flag = false;

            // Обработка новых сообщений 🔄
            for (unsigned int i = 0; ans_longpoll_json["updates"].size() > i; i++) {
                const json message_json = move(ans_longpoll_json["updates"][i]["object"]["message"]);

                if (!data_base::user::check(message_json["peer_id"])) {
                    data_base::user::add(message_json["peer_id"]);
                }

                if (message_json["text"] == std::string("стоп")) {
                    stop_flag = true;
                }

            }

            if (stop_flag) { break; }

            // Устанавливаем свежиий идентификатор сообытий 🛠
            test_blp.set_ts(stoi(std::string(ans_longpoll_json["ts"])));
        }

        // Цикл (события BLP API) {
            //     если (👤 🙅‍♀️ в 🗂) {
            //         добавить 👤 в 🗂;
            //         отправить 👋;
            //     }
            //
            //     иначе {
            //         (узнать кто 👤 отправитель письма)
            //         узнать stage у 👤;
            //         загрузить 📩 в stage этого 👤;
            //     }
            // }
            //
            // [Операция] Собираем время
            // [Операции] Рассылка расписания
            // 
            // [Операции] Рассылка расписания
        
        // Правильное выключение
        data_base::users::upload_data();
    

    return 0;
}