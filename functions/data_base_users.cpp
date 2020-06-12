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

void data_base::users::download_data() {
    data_base::user::info   temp_user;
    json                    user_json;
    std::string             user_str;
    ifstream ifs(data_base::users::data_path, ios::in);

    data_base::users::data.clear();
    ifs.seekg(ios::beg);

    if (!file::empty(data_base::users::data_path)) {
        while (!ifs.eof()) {
            ifs >> user_str;
            user_json = json::parse(user_str);

            temp_user.send_id = user_json["user_info"]["send_id"];
            temp_user.level   = user_json["user_info"]["level"];
            temp_user.stage   = user_json["user_info"]["stage"];

            data_base::users::data.insert_or_assign ( user_json["id"] , temp_user );
        }
    }


    ifs.close();

    cout << "[data_base][users][download_data] "
            "БД пользователей загружена в ОЗУ. Пользователей в ОЗУ: "
         << data_base::users::data.size() << endl;
}

//----------------------------------------------------------------------

void data_base::users::upload_data() {
    file::clear(data_base::users::data_path);
    ofstream ofs(data_base::users::data_path, ios::out | ios::app);

    unsigned int count_user = 0;

    // Делаем запись в файл
    if (!data_base::users::data.empty()) {
        json user_json;

        for (auto iter : data_base::users::data) {

            if (iter.first != (*(data_base::users::data.begin())).first) { ofs << endl; }

            user_json["id"]        =      iter.first;
            user_json["user_info"] = json(iter.second);
            
            ofs << user_json.dump();
            count_user++;
        }
    }

    ofs.close();

    cout << "[data_base][users][upload_data] "
         << count_user << " пользователей из ОЗУ записаны в файл "
         << data_base::users::data_path.filename() << endl;
}

//----------------------------------------------------------------------

void data_base::users::download_changelog() {

    data_base::user::info   user_info_obj;
    json                    user_json;
    std::string             user_str;
    ifstream ifs(data_base::users::changelog_path, ios::in);
    
    ifs.seekg(ios::beg);

    while (!ifs.eof()) {
        ifs >> user_str;
        json user_json = json::parse(user_str);

        user_info_obj.send_id = user_json["user_info"]["send_id"];
        user_info_obj.level   = user_json["user_info"]["level"];
        user_info_obj.stage   = user_json["user_info"]["stage"];

        data_base::users::data.insert_or_assign( user_json["id"], user_info_obj );
    }

    ifs.close();

    cout << "[data_base][users][download_changelog]"
            "Изменения применены к пользователям в ОЗУ" << endl;
}

////////////////////////////////////////////////////////////////////////