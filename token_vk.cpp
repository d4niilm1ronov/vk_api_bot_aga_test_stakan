#include <iostream>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "vk_api.hpp"

#include "very_eassy_curl.hpp"

#include "object/objects.hpp"

#include "object/attachment.hpp"
#include "object/photo.hpp"
#include "object/message.hpp"
#include "object/answer_botsLP.hpp"

#include "long_poll.hpp"
#include "token_vk.hpp"

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

    // cout << answer.dump(4) << endl;

    // Здесь должны быть эксепшены
    if (answer.count("error") != 0) {
        cout << "Error VK API" << endl;
        cout << "Error code: " << answer["error"]["error_code"] << endl;
        exit(1);
    }

    return vkapi::bots_long_poll(
             answer["response"]["server"],
             answer["response"]["key"],
        stoi(string(answer["response"]["ts"]))
    );
}

////////////////////////////////////////////////////////////////////////

void vkapi::token_group::messages_send(const vkapi::message& mesg) const {
    string answer;

    // Считывание peer_id
    if (mesg.peer_id) { answer = answer + "peer_id=" + to_string(*mesg.peer_id) + "&"; }

    // Считывание random_id
    if (mesg.random_id) { answer = answer + "random_id=" + to_string(rand()) + "&"; }

    // Считывание text_id
    if (mesg.text) {
        std::string text = *mesg.text;

        // Адаптируем пробелы
        for (auto iter = text.begin(); iter != text.end(); iter++) { if (*iter == ' ') { *iter = '+'; } }

        answer = answer + "message=" + text + "&";
    }

    // Считывание attachments
    if (mesg.attachments) {
        answer += "attachment=";

        for (int i = 0; mesg.count_attachments > i; i++) {
            answer +=
                mesg.attachments[i] -> getType() +
                to_string(*(reinterpret_cast<vkapi::photo*>(mesg.attachments[i]) -> owner_id)) + "_" +
                to_string(*(reinterpret_cast<vkapi::photo*>(mesg.attachments[i]) -> id));

            if (reinterpret_cast<vkapi::photo*>(mesg.attachments[i]) -> access_key) {
                answer += "_" + *(reinterpret_cast<vkapi::photo*>(mesg.attachments[i]) -> access_key);
            }
        }

        answer += "&";
    }

    reqCURL (
        objCURL,
        "https://api.vk.com/method/messages.send?" +
        answer +
        "access_token=" +
        TOKEN +
        "&v=5.103"
    );
}