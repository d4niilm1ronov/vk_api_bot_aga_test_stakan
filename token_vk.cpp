#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "very_eassy_curl.hpp"

#include "vk_api.hpp"
    #include "long_poll.hpp"
    #include "token_vk.hpp"

////////////////////////////////////////////////////////////////////////
//-----------------//     VKAPI :: token_base    //-------------------//
////////////////////////////////////////////////////////////////////////


vkapi::token_base::token_base(string token) : TOKEN(token), objCURL(curl_easy_init()) {
    curl_easy_setopt(objCURL, CURLOPT_WRITEFUNCTION, cts);
}

////////////////////////////////////////////////////////////////////////
//----------------//     VKAPI :: token_group    //-------------------//
////////////////////////////////////////////////////////////////////////

vkapi::token_group::token_group(string token_str, unsigned int id) : vkapi::token_base(token_str),  ID(id) {

}

////////////////////////////////////////////////////////////////////////

vkapi::bots_long_poll vkapi::token_group::groups_getLongPollServer() const {
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


    return vkapi::bots_long_poll(
             answer["response"]["server"],
             answer["response"]["key"],
        stoi(string(answer["response"]["ts"]))
    );
}

////////////////////////////////////////////////////////////////////////

nlohmann::json vkapi::token_group::messages_send(const nlohmann::json& mesg) const {
    string request;

    // Считывание peer_id, random_id
    if (mesg.count("peer_id")) { request += "peer_id=" + to_string(mesg["peer_id"]) + "&"; }
    
    request += "random_id=" + to_string(rand()) + "&";




    // Считывание text
    if (mesg.count("text")) {
        std::string text = mesg["text"];

        // Адаптируем пробелы
        for (auto iter = text.begin(); iter != text.end(); iter++) { if (*iter == ' ') { *iter = '+'; } }
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


        

    // Отправка сообщения (запроса)
    return nlohmann::json(json::parse(reqCURL (
        objCURL,
        "https://api.vk.com/method/messages.send?" +
        request +
        "access_token=" +
        TOKEN +
        "&v=5.103"
    )));
}

////////////////////////////////////////////////////////////////////////