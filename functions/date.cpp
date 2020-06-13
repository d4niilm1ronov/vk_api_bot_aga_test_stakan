#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <iterator>
#include <ctime>
#include <vector>
#include <regex>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "../vkAPI/support/very_eassy_curl.hpp"

#include "../vkAPI/vk_api.hpp"
#include "../vkAPI/long_poll.hpp"
#include "../vkAPI/token_vk.hpp"

#include "functions.hpp"



unsigned int date::current_lesson = 0;
unsigned int date::difference = 10461600;
time_t date::current_time = static_cast<unsigned int>(time(NULL)) - date::difference;

string date::arr_wday[7] = { "", "monday.json",  "tuesday.json", "wednesday.json",
                             "thursday.json", "friday.json", "saturday.json"  };

tm date::to_tm(const string& str_time) {
    int year = (localtime(&current_time) -> tm_year) + 1900;
    bool this_no_leap_year = (year % 4);

    cmatch rgex_result;
    regex regular( "(0[1-9]|[12][0-9]|3[01])[.](0[1-9]|1[012])" );

    if (!regex_match(str_time.c_str(), rgex_result, regular)) {
        throw (string("Не соответствует формату DD.MM"));
    }

    int day = stoi(string(str_time.begin(), str_time.begin() + 2));
    int mon = stoi(string(str_time.begin() + 3, str_time.end()));

    if ((mon==4)|(mon==6)|(mon==9)|(mon==11)) {
        if (day>30) { throw string("Нету 31 числа для этого месяца"); }
    } else

    if (mon==2) {
        if (this_no_leap_year) {
            if (day > 28) {
                if (day == 29) { throw string("Нету 29 числа для месяца Февраль в этом году"); }
                else           { throw string("Нету 30/31 числа для месяца Февраль"); }
            }
        } else {
            if (day > 29) {
                throw string("Нету 30/31 числа для месяца Февраль");
            }
        }
    }

    tm tm_result;
    tm_result.tm_year = year - 1900;
    tm_result.tm_mon  = mon - 1;
    tm_result.tm_mday = day;

    tm_result.tm_sec  = 0;
    tm_result.tm_min  = 0;
    tm_result.tm_hour = 0;

    auto ttttemp = mktime(&tm_result);
    tm_result = *localtime(&ttttemp);

    return tm_result;
}

bool date::operator>(const tm& time1, const tm& time2) {
    if (time1.tm_year != time2.tm_year) { return time1.tm_year > time2.tm_year; }
    if (time1.tm_mon != time2.tm_mon) { return time1.tm_mon > time2.tm_mon; }
    if (time1.tm_mday != time2.tm_mday) { return time1.tm_mday > time2.tm_mday; }
    if (time1.tm_hour != time2.tm_hour) { return time1.tm_hour > time2.tm_hour; }
    if (time1.tm_min != time2.tm_min) { return time1.tm_min > time2.tm_min; }

    return (time1.tm_sec > time2.tm_sec);
}

bool date::operator<(const tm& time1, const tm& time2) {
    if (time1.tm_year != time2.tm_year) { return time1.tm_year < time2.tm_year; }
    if (time1.tm_mon != time2.tm_mon) { return time1.tm_mon < time2.tm_mon; }
    if (time1.tm_mday != time2.tm_mday) { return time1.tm_mday < time2.tm_mday; }
    if (time1.tm_hour != time2.tm_hour) { return time1.tm_hour < time2.tm_hour; }
    if (time1.tm_min != time2.tm_min) { return time1.tm_min < time2.tm_min; }

    return (time1.tm_sec < time2.tm_sec);
}

bool date::its_spring_semester(const tm& tm_date) {
    if ((tm_date.tm_mon > 0)&(tm_date.tm_mon<5)) {
        return true;
    }

    return false;
};

bool date::its_fall_semester(const tm& tm_date) {
    if (tm_date.tm_mon > 7) {
        return true;
    }

    return false;
};

tm date::get_current_date() {
    date::current_time = static_cast<unsigned int>(time(NULL)) - date::difference;
    auto result_ptr = localtime(&date::current_time);
    auto result_tm = *result_ptr;

    return result_tm;
}

int date::get_stage_tt() {
    auto time = get_current_date();

    // Рассылка информации о 1 занятии
    if ((time.tm_hour == 8) & (time.tm_min == 0)) {
        return 1;
    }

    // Рассылка информации о 2 занятии
    if ((time.tm_hour == 10) & (time.tm_min == 0)) {
        return 2;
    }

    // Рассылка информации о 3 занятии
    if ((time.tm_hour == 11) & (time.tm_min == 50)) {
        return 3;
    }

    // Рассылка информации о 4 занятии
    if ((time.tm_hour == 13) & (time.tm_min == 50)) {
        return 4;
    }

    // Рассылка информации о 5 занятии
    if ((time.tm_hour == 15) & (time.tm_min == 40)) {
        return 5;
    }

    // Рассылка информации о 6 занятии
    if ((time.tm_hour == 17) & (time.tm_min == 30)) {
        return 6;
    }

    // Рассылка информации о 7 занятии
    if ((time.tm_hour == 19) & (time.tm_min == 20)) {
        return 7;
    }

    // Рассылка информации о 8 занятии
    if ((time.tm_hour == 21) & (time.tm_min == 0)) {
        return 8;
    }

    // Рассылка информации о 8 занятии
    if (time.tm_hour == 23) {
        return 0;
    }

    return date::current_lesson;
}

bool date::time_to_break() {
    auto time = get_current_date();

    if ((time.tm_hour == 9) & (time.tm_min == 15)) { return true; }
    if ((time.tm_hour == 11) & (time.tm_min == 5)) { return true; }
    if ((time.tm_hour == 13) & (time.tm_min == 5)) { return true; }
    if ((time.tm_hour == 14) & (time.tm_min == 55)) { return true; }
    if ((time.tm_hour == 16) & (time.tm_min == 45)) { return true; }
    if ((time.tm_hour == 18) & (time.tm_min == 45)) { return true; }
    if ((time.tm_hour == 20) & (time.tm_min == 25)) { return true; }
    if ((time.tm_hour == 22) & (time.tm_min == 5)) { return true; }

    return false;
}