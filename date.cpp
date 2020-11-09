#include <ctime>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// VK API
#include "vkAPI/support/very_eassy_curl.hpp"
#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

#include "date.hpp"


extern vkapi::token_group stankin_bot;


/////////////////////////////////////////////////////////////////////////////


time_stakan::date::date(const uint& YYMMDD) {
    struct_tm.tm_year = (YYMMDD / 10000) + 100;
    struct_tm.tm_mon = ((YYMMDD / 100) % 100) - 1;
    struct_tm.tm_mday = YYMMDD % 100;

    struct_tm.tm_hour = 12;
    struct_tm.tm_min  = 0;
    struct_tm.tm_sec  = 0;
    
    time_t time_t__temp = mktime(&struct_tm);
    struct_tm = *localtime(&time_t__temp);
}

//---------------------------------------------------------------------------

time_stakan::date::date(const uint& d, const uint& m, const uint& y) {
    struct_tm.tm_year = (y % 100) + 100;
    struct_tm.tm_mon  = m - 1;
    struct_tm.tm_mday = d;

    struct_tm.tm_hour = 12;
    struct_tm.tm_min  = 0;
    struct_tm.tm_sec  = 0;
    
    time_t time_t__temp = mktime(&struct_tm);
    struct_tm = *localtime(&time_t__temp);
}

//---------------------------------------------------------------------------

time_stakan::date time_stakan::date::plus_one_day() const {
    date date__result = *this;

    time_t time_t__ti3 = mktime(&(date__result.struct_tm)) + 60*60*24;
    
    struct tm* timeinfo = localtime (&time_t__ti3);
    date__result.struct_tm = *timeinfo;
    
    return (date__result);
}

//---------------------------------------------------------------------------

time_stakan::date time_stakan::date::plus_one_week() const {
    date date__result = *this;

    time_t time_t__ti3 = mktime(&(date__result.struct_tm)) + 60*60*24*7;
    
    struct tm* timeinfo = localtime (&time_t__ti3);
    date__result.struct_tm = *timeinfo;
    
    return (date__result);
}

//---------------------------------------------------------------------------

time_stakan::date time_stakan::date::plus_two_week() const {
    date date__result = *this;

    time_t time_t__ti3 = mktime(&(date__result.struct_tm)) + 60*60*24*7*2;
    
    struct tm* timeinfo = localtime (&time_t__ti3);
    date__result.struct_tm = *timeinfo;
    
    return (date__result);
}

//---------------------------------------------------------------------------

uint time_stakan::date::format_yymmdd() const {

    return ((struct_tm.tm_year % 100) * 10000) + ((struct_tm.tm_mon + 1) * 100) + (struct_tm.tm_mday);

}


/////////////////////////////////////////////////////////////////////////////

uint time_stakan::last_number_lesson = 0;

//---------------------------------------------------------------------------

uint time_stakan::get_current_number_lesson() {
    time_t rawtime = stankin_bot.utils_getServerTime();
    struct tm* timeinfo = localtime (&rawtime);

    uint uint_time = (100 * timeinfo->tm_hour) + (timeinfo->tm_min);

    if (uint_time >= 800  & uint_time < 1010) { return 1; }
    if (uint_time >= 1010 & uint_time < 1200) { return 2; }
    if (uint_time >= 1200 & uint_time < 1400) { return 3; }
    if (uint_time >= 1400 & uint_time < 1550) { return 4; }
    if (uint_time >= 1550 & uint_time < 1740) { return 5; }
    if (uint_time >= 1740 & uint_time < 1930) { return 6; }
    if (uint_time >= 1930 & uint_time < 2110) { return 7; }
    if (uint_time >= 2120 & uint_time < 2250) { return 8; }
    if (uint_time >= 2250)                    { return 9; }

    return 0;
}

//---------------------------------------------------------------------------

time_stakan::date time_stakan::get_current_date() {
    time_t rawtime = stankin_bot.utils_getServerTime();
    struct tm* timeinfo = localtime (&rawtime);
 
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    return date(timeinfo -> tm_mday, timeinfo -> tm_mon + 1, timeinfo -> tm_year);
}

//---------------------------------------------------------------------------