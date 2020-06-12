#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <iterator>
#include <ctime>
#include <vector>
#include <unistd.h>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "../vkAPI/support/very_eassy_curl.hpp"

#include "../vkAPI/vk_api.hpp"
#include "../vkAPI/long_poll.hpp"
#include "../vkAPI/token_vk.hpp"

#include "functions.hpp"

////////////////////////////////////////////////////////////////////////

void data_base::user::add (const unsigned int& id) {
    fstream changelog(data_base::users::changelog_path, ios::out | ios::in | ios::app);

    // Добавление в Оперативную
    data_base::users::data.insert( { id , data_base::user::info() } );
    
    // Добавление в Постоянную (changelog)
    if (!file::empty(data_base::users::changelog_path)) {
        changelog << endl;
    }

    json temp_json;
    temp_json["id"] = id;
    temp_json["user_info"] = json(data_base::users::data[id]);


    changelog << temp_json.dump();

    cout << "[data_base][user][add] Добавлен новый пользователь (id" << id << ") "
            "в файл " << data_base::users::changelog_path.filename() << endl;
}

//----------------------------------------------------------------------

bool data_base::user::check (const unsigned int& id) {
    return (data_base::users::data.find(id) != data_base::users::data.end());
}

//----------------------------------------------------------------------

void data_base::user::push_changelog (const unsigned int& id) {
    fstream changelog(data_base::users::changelog_path, ios::out | ios::in | ios::app);

    if (!file::empty(data_base::users::changelog_path)) { changelog << endl; }

    json temp_json;
    temp_json["id"] = id;
    temp_json["user_info"] = json(data_base::users::data[id]);


    changelog << temp_json.dump();

    cout << "[data_base][user][push_changelog] Продублированны изменения пользователя id" << id <<
            " в файл " << data_base::users::changelog_path.filename() << endl;
}

////////////////////////////////////////////////////////////////////////