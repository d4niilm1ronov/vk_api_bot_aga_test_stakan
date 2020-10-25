// ОПИСАНИЕ: Определение конструкторов и методов классов
//
// vkapi
// | ---- token_base
// |      | ---- constructor(string)
// |
// | ---- token_group
//        | ---- constructor(string, uint)
//        | ---- groups_getLongPollServer()
//        | ---- groups_getLongPollSettings()
//        | ---- groups_setLongPollSettings(string, bool)
//        | ---- groups_setLongPollSettings(json)
//        | ---- messages_send(json)
//        | ---- messages_send(id, json)

#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "support/very_eassy_curl.hpp"

#include "vk_api.hpp"
    #include "long_poll.hpp"
    #include "token_vk.hpp"



// НАЗВАНИЕ: Конструктор класса vkapi::token_base
// ОПИСАНИЕ: Конструктор для базового класса, для работы с токенами ВКонтакте.
//           Инициализирует объект cURL, для работы с запросами
//           и поле, содержащую токен в виде STRING
vkapi::token_base::token_base(string token) : TOKEN(token), objCURL(curl_easy_init()) {
    curl_easy_setopt(objCURL, CURLOPT_WRITEFUNCTION, cts);
}


// НАЗВАНИЕ: Конструктор класса vkapi::token_group
// ОПИСАНИЕ: Конструктор для класса токенов групп ВКонтакте.
//           Принимает токен и ID группы, чтобы проинициализировать поля
//           Запрашивает и локально устанавливает параметры Long Poll
vkapi::token_group::token_group(string token_str, unsigned int id) : vkapi::token_base(token_str),  ID(id) {

        groups_setLongPollSettings(groups_getLongPollSettings());

        // Вывод информации в консоль
        std::cout << setw(16) << "[VK API]" << " Create new token (group id" << id << ") "
        "token: " << std::string(begin(token_str), begin(token_str) + 10) << "..." << std::endl;
    }


// НАЗВАНИЕ: Метод vkapi::token_group для получения объекта vkapi::bots_long_poll
// ОПИСАНИЕ: Конструктор для класса токенов групп ВКонтакте.
//           Принимает токен и ID группы, чтобы проинициализировать поля
//           Запрашивает и локально устанавливает параметры Long Poll
vkapi::bots_long_poll vkapi::token_group::groups_getLongPollServer() const {
    // Делаем запрос, чтобы узнать параметры для Long Poll
    json answer = json::parse(
        reqCURL (
            objCURL,
            "https://api.vk.com/method/groups.getLongPollServer?group_id=" +
            to_string(ID) +
            "&access_token=" +
            TOKEN +
            "&v=5.103"
        )
    );

    // Вывод информации
    std::cout << setw(16) << "[VK API]" << " Токен (" << std::string(begin(TOKEN), begin(TOKEN) + 10) << ") "
              << " запросил данные для Long Boll в виде объекта класса"<< std::endl;

    return vkapi::bots_long_poll(
             answer["response"]["server"],
             answer["response"]["key"],
        stoi(string(answer["response"]["ts"]))
    );
}


// НАЗВАНИЕ: Метод vkapi::token_group для получения настроек Long Poll
// ОПИСАНИЕ: Возвращает настройки Long Poll из группы, которой принадлежит данный токен,
//           в формате JSON по данному токена
nlohmann::json vkapi::token_group::groups_getLongPollSettings() const {
    // Вывод информации
    std::cout << setw(16) << "[VK API]" << " Токен (" << std::string(begin(TOKEN), begin(TOKEN) + 10) << ") "
              << "запросил настройки Long Boll в виде .json"<< std::endl;

    return nlohmann::json ( nlohmann::json::parse (
        reqCURL (
            objCURL,
            "https://api.vk.com/method/groups.getLongPollSettings?group_id=" +
            to_string(ID) +
            "&access_token=" +
            TOKEN +
            "&v=5.103"
        )
    ) ) ;
}


// НАЗВАНИЕ: Метод vkapi::token_group для установки параметра для Long Poll по названию и флага типа bool
nlohmann::json vkapi::token_group::groups_setLongPollSettings(const std::string& param, const bool& value) const {

    std::string request = "api_version=5.103&";

    if (param != "api_version") {
        if (param == "is_enabled" | param == "enabled") {
            request += "enabled=";
        } else {
            request += param + "=";
        }

        request += to_string(static_cast<int>(value));
    }

    // Вывод информации
    std::cout << setw(16) << "[VK API]" << " Токен (" << std::string(begin(TOKEN), begin(TOKEN) + 10) << ") "
              << "установил у Long Poll параметр " << param << " значение " << value << std::endl;

    return nlohmann::json ( nlohmann::json::parse (
        reqCURL (
            objCURL,
            "https://api.vk.com/method/groups.setLongPollSettings?" +
            request +
            "&group_id=" +
            to_string(ID) +
            "&access_token=" +
            TOKEN +
            "&v=5.103"
        )
    ) ) ;
}


// НАЗВАНИЕ: Метод vkapi::token_group для установки конфига с параметров для Long Poll
// ОПИСАНИЕ: Конфиг формата .json, который содержит параметры и значения к ним, которые нужно поменять
nlohmann::json vkapi::token_group::groups_setLongPollSettings(const nlohmann::json& setting_json) const {
    std::string request;

    request += "api_version=5.103&";

    if (setting_json.count("response")) {

        if (setting_json["response"].count("if_enabled")) { request += "enabled=" + to_string(setting_json["response"]["if_enabled"]) + "&"; }
        
        if (setting_json["response"].count("events")) {

            if (setting_json["response"]["events"].count("message_new")) { request += "message_new=" +  to_string(setting_json["response"]["events"]["message_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("message_reply")) { request += "message_reply=" +  to_string(setting_json["response"]["events"]["message_reply" ]) + "&"; }
            if (setting_json["response"]["events"].count("message_allow")) { request += "message_allow=" +  to_string(setting_json["response"]["events"]["message_allow" ]) + "&"; }
            if (setting_json["response"]["events"].count("message_deny")) { request += "message_deny=" +  to_string(setting_json["response"]["events"]["message_deny" ]) + "&"; }
            if (setting_json["response"]["events"].count("message_read")) { request += "message_read=" +  to_string(setting_json["response"]["events"]["message_read" ]) + "&"; }
            if (setting_json["response"]["events"].count("message_typing_state")) { request += "message_typing_state=" +  to_string(setting_json["response"]["events"]["message_typing_state" ]) + "&"; }
            if (setting_json["response"]["events"].count("message_edit")) { request += "message_edit=" +  to_string(setting_json["response"]["events"]["message_edit" ]) + "&"; }

            if (setting_json["response"]["events"].count("photo_new")) { request += "photo_new=" +  to_string(setting_json["response"]["events"]["photo_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("video_new")) { request += "video_new=" +  to_string(setting_json["response"]["events"]["video_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("audio_new")) { request += "audio_new=" +  to_string(setting_json["response"]["events"]["audio_new" ]) + "&"; }

            if (setting_json["response"]["events"].count("wall_reply_new")) { request += "wall_reply_new=" +  to_string(setting_json["response"]["events"]["wall_reply_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("wall_reply_edit")) { request += "wall_reply_edit=" +  to_string(setting_json["response"]["events"]["wall_reply_edit" ]) + "&"; }
            if (setting_json["response"]["events"].count("wall_reply_delete")) { request += "wall_reply_delete=" +  to_string(setting_json["response"]["events"]["wall_reply_delete" ]) + "&"; }
            if (setting_json["response"]["events"].count("wall_post_new")) { request += "wall_post_new=" +  to_string(setting_json["response"]["events"]["wall_post_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("wall_repost")) { request += "wall_repost=" +  to_string(setting_json["response"]["events"]["wall_repost" ]) + "&"; }

            if (setting_json["response"]["events"].count("board_post_new")) { request += "board_post_new=" +  to_string(setting_json["response"]["events"]["board_post_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("board_post_edit")) { request += "board_post_edit=" +  to_string(setting_json["response"]["events"]["board_post_edit" ]) + "&"; }
            if (setting_json["response"]["events"].count("board_post_delete")) { request += "board_post_delete=" +  to_string(setting_json["response"]["events"]["board_post_delete" ]) + "&"; }
            if (setting_json["response"]["events"].count("board_post_restore")) { request += "board_post_restore=" +  to_string(setting_json["response"]["events"]["board_post_restore" ]) + "&"; }

            if (setting_json["response"]["events"].count("photo_comment_new")) { request += "photo_comment_new=" +  to_string(setting_json["response"]["events"]["photo_comment_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("photo_comment_edit")) { request += "photo_comment_edit=" +  to_string(setting_json["response"]["events"]["photo_comment_edit" ]) + "&"; }
            if (setting_json["response"]["events"].count("photo_comment_delete")) { request += "photo_comment_delete=" +  to_string(setting_json["response"]["events"]["photo_comment_delete" ]) + "&"; }
            if (setting_json["response"]["events"].count("photo_comment_restore")) { request += "photo_comment_restore=" +  to_string(setting_json["response"]["events"]["photo_comment_restore" ]) + "&"; }

            if (setting_json["response"]["events"].count("video_comment_new")) { request += "video_comment_new=" +  to_string(setting_json["response"]["events"]["video_comment_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("video_comment_edit")) { request += "video_comment_edit=" +  to_string(setting_json["response"]["events"]["video_comment_edit" ]) + "&"; }
            if (setting_json["response"]["events"].count("video_comment_delete")) { request += "video_comment_delete=" +  to_string(setting_json["response"]["events"]["video_comment_delete" ]) + "&"; }
            if (setting_json["response"]["events"].count("video_comment_restore")) { request += "video_comment_restore=" +  to_string(setting_json["response"]["events"]["video_comment_restore" ]) + "&"; }

            if (setting_json["response"]["events"].count("market_comment_new")) { request += "market_comment_new=" +  to_string(setting_json["response"]["events"]["market_comment_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("market_comment_edit")) { request += "market_comment_edit=" +  to_string(setting_json["response"]["events"]["market_comment_edit" ]) + "&"; }
            if (setting_json["response"]["events"].count("market_comment_delete")) { request += "market_comment_delete=" +  to_string(setting_json["response"]["events"]["market_comment_delete" ]) + "&"; }
            if (setting_json["response"]["events"].count("market_comment_restore")) { request += "market_comment_restore=" +  to_string(setting_json["response"]["events"]["market_comment_restore" ]) + "&"; }

            if (setting_json["response"]["events"].count("poll_vote_new")) { request += "poll_vote_new=" +  to_string(setting_json["response"]["events"]["poll_vote_new" ]) + "&"; }
            if (setting_json["response"]["events"].count("group_join")) { request += "group_join=" +  to_string(setting_json["response"]["events"]["group_join" ]) + "&"; }
            if (setting_json["response"]["events"].count("group_leave")) { request += "group_leave=" +  to_string(setting_json["response"]["events"]["group_leave" ]) + "&"; }
            if (setting_json["response"]["events"].count("user_block")) { request += "user_block=" +  to_string(setting_json["response"]["events"]["user_block" ]) + "&"; }
            if (setting_json["response"]["events"].count("user_unblock")) { request += "user_unblock=" +  to_string(setting_json["response"]["events"]["user_unblock" ]) + "&"; }
            if (setting_json["response"]["events"].count("group_change_settings")) { request += "group_change_settings=" +  to_string(setting_json["response"]["events"]["group_change_settings" ]) + "&"; }
            if (setting_json["response"]["events"].count("group_change_photo")) { request += "group_change_photo=" +  to_string(setting_json["response"]["events"]["group_leave" ]) + "&"; }
            if (setting_json["response"]["events"].count("group_officers_edit")) { request += "group_officers_edit=" +  to_string(setting_json["response"]["events"]["group_officers_edit" ]) + "&"; }

        }

    }

    // Вывод информации
    std::cout << setw(16) << "[VK API]" << " Токен (" << std::string(begin(TOKEN), begin(TOKEN) + 10) << ") "
              << "переустановил в Long Poll параметров" << std::endl;

    return nlohmann::json ( nlohmann::json::parse (
        reqCURL (
            objCURL,
            "https://api.vk.com/method/groups.setLongPollSettings?" +
            request +
            "group_id=" +
            to_string(ID) +
            "&access_token=" +
            TOKEN +
            "&v=5.103"
        )
    ) ) ;
}


// НАЗВАНИЕ: Метод vkapi::token_group для отправки сообщений (реализация messages.send)
// ОПИСАНИЕ: В качестве аргументов принимает объект Сообщение (см. в документации) в виде .json
nlohmann::json vkapi::token_group::messages_send(const nlohmann::json& mesg) const {
    string request;

    // Считывание peer_id,
    if (mesg.count("peer_id"))   { request += "peer_id="   +  to_string(mesg["peer_id"])    + "&"; }

    // Считывание random_id
    if (mesg.count("random_id")) { request += "random_id=" +  to_string(mesg["random_id"])  + "&"; }
    else                         { request += "random_id=" +  to_string(rand())            + "&"; }

    if (mesg.count("keyboard"))  { request += "keyboard="  +  mesg["keyboard"].dump()       + "&"; }

    // Считывание текста сообщения
    if (mesg.count("text")) {
        std::string text = mesg["text"];

        // Адаптируем переносы строк
        while ((text.find('\n')) != text.npos) {
            text.replace(text.find('\n'), 1, "%0A");
        }

        // Запись в запрос
        request = request + "message=" + text + "&";
    }

    // Считывание attachments
    if (mesg.count("attachments")) {
        if (mesg["attachments"].size()) {
            request += "attachment=";
            json attachments = mesg["attachments"];

            for (int i = 0; attachments.size() > i; i++) {
                request += attachments[i]["type"];
                request += to_string(attachments[i] [std::string(attachments[i]["type"])] ["owner_id"]);
                request += '_';
                request += to_string(attachments[i] [std::string(attachments[i]["type"])] ["id"]);

                if (attachments[i][std::string(attachments[i]["type"])].count("access_key")) {
                    request += '_';
                    request += attachments[i] [std::string(attachments[i]["type"])] ["access_key"];
                }

                if (i + 1 != attachments.size()) { request += ','; }
            }

            request += "&";
        }
    }

    // Адаптируем пробелы
    replace(request.begin(), request.end(), ' ', '+');

    

    // Вывод информации
    std::cout << setw(16) << "[VK API]" << " Токен (" << std::string(begin(TOKEN), begin(TOKEN) + 10) << ") "
              << "переустановил в Long Poll параметров" << std::endl;

    // Отправка сообщения (запроса)
    return nlohmann::json(json::parse(reqCURL (
        objCURL,
        "https://api.vk.com/method/messages.send?" +
        request         +
        "access_token=" +
        TOKEN           +
        "&group_id="    +
        to_string(ID)   +
        "&v=5.103"
    )));
}


// НАЗВАНИЕ: Метод vkapi::token_group для отправки сообщений (реализация messages.send)
// ОПИСАНИЕ: В качестве аргументов принимается peer_id (ID получателя)
//           и объект Сообщение (см. в документации) в виде .json .
//           Данный метод используется для отправки готовых объектов Сообщений,
//           для которых нужно изменить только peer_id,
//           т.е. отпадает надобность менять json объект в теле, откуда отправляется сообщение
nlohmann::json vkapi::token_group::messages_send(const unsigned int& peer_id, nlohmann::json mesg) const {
    mesg["peer_id"] = peer_id;
    
    return messages_send(mesg);
}