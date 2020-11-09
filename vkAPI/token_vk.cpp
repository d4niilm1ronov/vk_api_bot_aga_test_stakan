// ОПИСАНИЕ: Определение конструкторов и методов классов
//
// vkapi
// | ---- token_base
//        | ---- constructor(string)
//
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

}


vkapi::token_group::token_group(void) : vkapi::token_base(string()) {


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


// НАЗВАНИЕ: Метод vkapi::token_group для отправки сообщений (реализация messages.send)
// ОПИСАНИЕ: В качестве аргументов принимает объект Сообщение (см. в документации) в виде .json
nlohmann::json vkapi::token_group::messages_send(const nlohmann::json& mesg) const {
    string request;

    // Считывание peer_id,
    if (mesg.count("peer_id"))   { request += "peer_id="   +  to_string(mesg["peer_id"])    + "&"; }

    // Считывание random_id
    if (mesg.count("random_id")) { request += "random_id=" +  to_string(mesg["random_id"])  + "&"; }
    else                         { request += "random_id=" +  to_string(rand())             + "&"; }


    // Считывание keyboard,
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
              << "использовал метод messages.send (peer_id = " << uint(mesg["peer_id"]) << ")" << std::endl;

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
    )));;
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

nlohmann::json vkapi::token_group::messages_send(const unsigned int& peer_id, string text) const {
    json mesg; mesg["peer_id"] = peer_id; mesg["text"] = text;

    return messages_send(mesg);
}


// НАЗВАНИЕ: 
// ОПИСАНИЕ: 
vkapi::token_group& vkapi::token_group::operator=(const vkapi::token_group& right_token) {
    this -> ID    = right_token.ID;
    this -> TOKEN = right_token.TOKEN;

    return *this;
}


uint vkapi::token_group::utils_getServerTime() const {

    json json__result = json::parse(reqCURL (
        objCURL,
        string("https://api.vk.com/method/utils.getServerTime?") +
        "access_token=" +
        TOKEN           +
        "&group_id="    +
        to_string(ID)   +
        "&v=5.103"
    ));;

    return json__result["response"];
}