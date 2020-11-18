#include <string>
#include <vector>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Библиотека для работы с SQLite 3 & C++
#include "sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

// VK API
#include "vkAPI/support/very_eassy_curl.hpp"
#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

#include "data_base.hpp"

#include "date.hpp"

/////////////////////////////////////////////////////////////////////////////

sqlite::database data_base::db(":memory:");

/////////////////////////////////////////////////////////////////////////////

void data_base::update_lesson(uint id) {
    auto cur_date = time_stakan::get_current_date();

    uint date = 0;

    json arr_date;

    // Определяем таблицу по ID записи
    string name_db = (id > 2000000000 ? "lesson_user" : "lesson_stankin") ;
    
    // Получаем значения date и arr_date
    data_base::db << "SELECT date, arr_date FROM " + name_db + " WHERE id = ? ;"
    << id
    >> [&date, &arr_date](uint d, string str_arr_d) {
        date = d;
        arr_date = json::parse(str_arr_d);

    } ;

    // Проверка на ID
    if (date == 0) { cout << "[data_base::update_lesson] Записи с таким id не найдено" << endl; return; }

    // Обрабатываем arr_date
    while (arr_date.size()) {
        
        // Если вверхняя дата – конечная
        if (arr_date[0] < 9999) {

            // Если такая дата раньше или равно текущей
            if (cur_date.format_mmdd() >= arr_date[0]) {
                // Удаляем ее и переходим к следующей
                arr_date.erase(0); continue;
            }

            // Если такая дата позже текущей
            else {
                // Дата для обновления найдена. Поиск остановлен.
                date = arr_date[0]; break;
            }
        }
        
        // Если вверхняя дата – зацикленная
        else {

            // Если такая дата позже или равно текущей
            if ( cur_date.format_mmdd() >= ((uint(arr_date[0]) / 10000 ) % 10000) ) {
                // Удаляем ее и переходим к следующей
                arr_date.erase(0); continue;
            }

             // Если такая дата позже текущей
            else {

                
                auto increment_date = time_stakan::date( uint(arr_date[0]) % 10000 );

                if ((uint(arr_date[0]) / 100000000) == 1) {

                    while (increment_date <= cur_date) {
                        increment_date = increment_date.plus_one_week();
                    }
                    
                }

                else {

                    while (increment_date <= cur_date) {
                        increment_date = increment_date.plus_two_week();
                    }

                }

                date = increment_date.format_mmdd();

                if (date == ((uint(arr_date[0]) / 10000 ) % 10000)) {
                    
                    arr_date[0] = date;
                    
                } else {
                    
                    arr_date[0] = ((uint(arr_date[0]) / 10000 ) * 10000) + date;
                    
                }

                break;

            }

        }

    }

    if ( arr_date.size() ) {
        data_base::db << "UPDATE " + name_db + " SET date = ? , arr_date = ? WHERE id = ? ;"
        << date << arr_date.dump(0) << id;
    } else {
        data_base::db << "DELETE FROM " + name_db + " WHERE id = ? ;" << id;
    }
}

/////////////////////////////////////////////////////////////////////////////

vector<json> data_base::get_lesson__user(uint id_user, uint date_MMDD) {
    vector<json> vector__result;
    
    // Делаем запрос информации о текущих занятиях.
    // Обрабатываем их в лямбде (как цикл).
    data_base::db << "SELECT les.id, les.name, les.type, les.lab_group, "
                     "       les.teacher, les.place, les.time "
                     "FROM lesson_user AS les "
                     "WHERE (les.user_id = ?) AND (les.date = ?);"
       << id_user << date_MMDD >> [&vector__result](
            uint   les__id,
            string les__name,
            uint   les__type,
            string les__lab_group,
            string les__teacher,
            string les__place,
            uint   les__time
       ) {
            json json__result;

            json__result["id"]         = les__id;
            json__result["time"]       = les__time;
            json__result["name"]       = les__name;
            json__result["type"]       = les__type;
            json__result["lab_group"]  = les__lab_group;
            json__result["teacher"]    = les__teacher;
            json__result["place"]      = les__place;


            vector__result.push_back(json__result);
       };


    return vector__result;
}

/////////////////////////////////////////////////////////////////////////////

vector<json> data_base::get_lesson__user(uint id_user, uint date_MMDD, uint time) {
    vector<json> vector__result;
    
    // Делаем запрос информации о текущих занятиях.
    // Обрабатываем их в лямбде (как цикл).
    data_base::db << "SELECT les.id, les.name, les.type, les.lab_group, "
                     "       les.teacher, les.place, les.time "
                     "FROM lesson_user AS les "
                     "WHERE ( les.user_id = ? ) AND ( les.date = ? ) AND ( les.time = ? );"
       << id_user << date_MMDD << time >> [&vector__result](
            uint   les__id,
            string les__name,
            uint   les__type,
            string les__lab_group,

            string les__teacher,
            string les__place,
            uint   les__time
       ) {
            json json__result;

            json__result["id"]         = les__id;
            json__result["name"]       = les__name;
            json__result["type"]       = les__type;
            json__result["lab_group"]  = les__lab_group;
            
            json__result["teacher"]    = les__teacher;
            json__result["place"]      = les__place;
            json__result["time"]       = les__time;


            vector__result.push_back(json__result);
       };


    return vector__result;
}

/////////////////////////////////////////////////////////////////////////////

vector<json> data_base::get_lesson(uint date_MMDD, uint time) {
    vector<json> vector__result;
    
    // Делаем запрос информации о текущих занятиях.
    // Обрабатываем их в лямбде (как цикл).
    data_base::db << "SELECT les.id, les.name, les.type, les.lab_group, les.teacher, "
                     "       les.place, les.arr_date, les.date"
                     "       user.id, user.stage, user.cache "
                     "FROM lesson_user AS les, user "
                     "WHERE (les.user_id = user.id) AND ((les.time = ?) AND (les.date = ?));"
       << time << date_MMDD >> [&vector__result, time](
            uint   les__id,
            uint   les__date,
            string les__name,
            uint   les__type,
            string les__lab_group,
            string les__teacher,
            string les__place,
            string les__arr_date,

            uint user__id,
            string user__stage,
            string user__cache
       ) {
            json json__result;

            json__result["lesson"] = json();
            json__result["user"]   = json();

            json__result["lesson"]["time"]       = time;
            json__result["lesson"]["date"]       = les__date;
            json__result["lesson"]["name"]       = les__name;
            json__result["lesson"]["type"]       = les__type;
            json__result["lesson"]["lab_group"]  = les__lab_group;
            json__result["lesson"]["teacher"]    = les__teacher;
            json__result["lesson"]["place"]      = les__place;
            json__result["lesson"]["arr_date"]   = les__arr_date;
            json__result["lesson"]["id"]         = les__id;

            json__result["user"]["id"]    = user__id;
            json__result["user"]["stage"] = user__stage;
            json__result["user"]["cache"] = user__cache;


            vector__result.push_back(json__result);
       };


    return vector__result;
}

/////////////////////////////////////////////////////////////////////////////

bool data_base::check_user_id(const uint& id) {
    int result;
    data_base::db << "SELECT count(*) FROM user WHERE id = ? ;"
                  << id
                  >> result;

    return result;
}

//---------------------------------------------------------------------------

void data_base::add_user(uint user_id) {
    data_base::db << "INSERT INTO user (cache, stage , id) "
                     "VALUES ( '{\"menu\": \"guest\"}' , 'menu_guest' , ? );"
                  << user_id;
}

//---------------------------------------------------------------------------

json data_base::get_user_cache(const uint& user_id) {
    string cache__string;

    data_base::db << "SELECT cache FROM user WHERE id == ? ;"
                  << user_id
                  >> cache__string;

    return json::parse(cache__string);
}

//---------------------------------------------------------------------------

void data_base::set_user_cache(const uint& user_id, const json& cache) {

    data_base::db << "UPDATE user SET cache = ? WHERE id = ?;"
       << cache.dump(0) << user_id;
}

//---------------------------------------------------------------------------

string data_base::get_user_stage(const uint& user_id) {
    string stage__string;

    data_base::db << "SELECT stage FROM user WHERE id == ? ;"
                  << user_id
                  >> stage__string;

    return stage__string;
}

//---------------------------------------------------------------------------

void data_base::set_user_stage(const uint& user_id, const string& stage) {

    data_base::db << "UPDATE user SET stage = ? WHERE id = ?;"
       << stage << user_id;
}

//---------------------------------------------------------------------------

void data_base::add_lesson(const uint& user_id, const uint& id_group, const uint id_lab_group) {
    // Добавляем пары с 'lesson_stankin.id_lab_group = id_lab_group' (лабы определенной подгруппы)
    data_base::db <<
    "INSERT INTO lesson_user ("
    "  time,"
    "  date,"
    "  name,"
    "  type,"
    "  teacher,"
    "  place,"
    "  arr_date,"
    "  lab_group,"
    "  user_id"
    ") "
    "SELECT lesson_stankin.time,"
    "       lesson_stankin.date,"
    "       lesson_stankin.name,"
    "       lesson_stankin.type,"
    "       lesson_stankin.teacher,"
    "       lesson_stankin.place,"
    "       lesson_stankin.arr_date,"
    "       'null',"
    "       ? "
    "FROM lesson_stankin "
    "WHERE (lesson_stankin.id_group = ?) AND ((lesson_stankin.id_lab_group = ?) OR (lesson_stankin.id_lab_group = 0)); "
    
    << user_id
    << id_group
    << id_lab_group;
}

//---------------------------------------------------------------------------

void data_base::add_lesson(const uint& user_id, const uint& id_group) {
    // Добавляем пары с 'lesson_stankin.id_lab_group = 0' (лекции и семинары)
    data_base::db << "INSERT INTO lesson_user ("
                     "  time,"
                     "  date,"
                     "  name,"
                     "  type,"
                     "  teacher,"
                     "  place,"
                     "  arr_date,"
                     "  lab_group,"
                     "  user_id"
                     ") "
                     "SELECT"
                     "       lesson_stankin.time,"
                     "       lesson_stankin.date,"
                     "       lesson_stankin.name,"
                     "       lesson_stankin.type,"
                     "       lesson_stankin.teacher,"
                     "       lesson_stankin.place,"
                     "       lesson_stankin.arr_date,"
                     "       'null',"
                     "       ? "
                     "FROM lesson_stankin "
                     "WHERE (lesson_stankin.id_group = ?) AND (lesson_stankin.id_lab_group = 0); "
                  << user_id
                  << id_group;


    // Массив с названиями подгрупп для лабораторных занятий
    json json_arr__name_lab_group;

    // Получаем этот^ массив
    string string__json_arr__name_lab_group;
    data_base::db << "SELECT lab_group "
                         "FROM group_stankin "
                         "WHERE (institute = ?) AND "
                         "      (year = ?)      AND "
                         "      (num = ?);       "
        << (id_group / 10000) << ((id_group % 10000) / 100) << (id_group % 100)
        >> string__json_arr__name_lab_group;

    json_arr__name_lab_group = json::parse(string__json_arr__name_lab_group);

    // Цикл, который проходиться по всем подгруппа по лабораторным группы
    // и добавляет их в базу данных
    for (uint i = 0; i < json_arr__name_lab_group.size(); i++) {
         // Получаем название подгруппы
        string name_lab_group = string(json_arr__name_lab_group[i]);

        // Добавляем пары с 'lesson_stankin.id_lab_group = id_lab_group' (лабы определенной подгруппы)
        data_base::db << "INSERT INTO lesson_user ("
                            "  time,"
                            "  date,"
                            "  name,"
                            "  type,"
                            "  teacher,"
                            "  place,"
                            "  arr_date,"
                            "  lab_group,"
                            "  user_id"
                            ") "
                            "SELECT lesson_stankin.time,"
                            "       lesson_stankin.date,"
                            "       lesson_stankin.name,"
                            "       lesson_stankin.type,"
                            "       lesson_stankin.teacher,"
                            "       lesson_stankin.place,"
                            "       lesson_stankin.arr_date,"
                            "       ?,"
                            "       ? "
                            "FROM lesson_stankin "
                            "WHERE (lesson_stankin.id_group = ?) AND (lesson_stankin.id_lab_group = ?); "
                    << name_lab_group
                    << user_id
                    << id_group
                    << i + 1;
    }
}

//---------------------------------------------------------------------------

void data_base::remove_lesson(const uint& id) {
    if (id > 2000000000) {
        data_base::db << "DELETE FROM lesson_user WHERE id = ? ;" << id;
    } else {
        data_base::db << "DELETE FROM lesson_user WHERE user_id = ? ;" << id;
    }
}

//---------------------------------------------------------------------------

json data_base::get_group_labs(const uint& institute, const uint& year, const uint& num) {
    string string__json__arr_lab_group;

    data_base::db << "SELECT lab_group FROM group_stankin WHERE (institute == ?) AND (year == ?) AND (num == ?);"
    << institute << year << num
    >> string__json__arr_lab_group;

    return json::parse(string__json__arr_lab_group);
}