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

    std::cout << "[VK API] Create new token (group id" << id
              << ") token: " << std::string(begin(token_str), begin(token_str) + 10) << "..." << std::endl;

    groups_setLongPollSettings(groups_getLongPollSettings());
}

////////////////////////////////////////////////////////////////////////

vkapi::bots_long_poll vkapi::token_group::groups_getLongPollServer() const {
    std::cout << "[VK API] groups.getLongPollServer" << std::endl;

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

nlohmann::json vkapi::token_group::groups_getLongPollSettings() const {
    std::cout << "[VK API] groups.getLongPollSettings" << std::endl;

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

////////////////////////////////////////////////////////////////////////

nlohmann::json vkapi::token_group::groups_setLongPollSettings(const std::string& param, const bool& value) const {
    std::cout << "[VK API] groups.setLongPollSettings" << std::endl;

    std::string request = "api_version=5.103&";

    if (param != "api_version") {
        if (param == "is_enabled" | param == "enabled") {
            request += "enabled=";
        } else {
            request += param + "=";
        }

        request += to_string(static_cast<int>(value));
    }

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

//----------------------------------------------------------------------

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

    std::cout << "[VK API] groups.setLongPollSettings" << std::endl;

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

    std::cout << "[VK API] messages.send" << std::endl;

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