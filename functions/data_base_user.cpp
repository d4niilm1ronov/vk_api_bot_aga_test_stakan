#include <iostream>
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <ctime>
#include <unistd.h>

using namespace std;
using json = nlohmann::json;

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

    changelog <<
        "{"
            "\"id\":" << to_string(id) << ","
            "\"user_info\":{"
                "\"level\":"     "0,"
                "\"send_id\":"   "0,"
                "\"stage\":"     "100000"
            "}"
        "}";

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

    changelog <<
        "{"
            "\"id\":" << to_string(id) << ","
            "\"user_info\":{"
                "\"level\":"     << to_string(data_base::users::data[id].level)   << ","
                "\"send_id\":"   << to_string(data_base::users::data[id].send_id) << ","
                "\"stage\":"     << to_string(data_base::users::data[id].stage)   <<
            "}"
        "}";

    cout << "[data_base][user][push_changelog] Продублированны изменения пользователя id" << id <<
            "в файл " << data_base::users::changelog_path.filename() << endl;
}

////////////////////////////////////////////////////////////////////////