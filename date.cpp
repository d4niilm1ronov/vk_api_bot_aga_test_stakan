#include <ctime>

using namespace std;

#include "date.hpp"

/////////////////////////////////////////////////////////////////////////////

uint time_university::last_number_lesson
        = time_university::get_current_number_lesson();

//---------------------------------------------------------------------------

uint time_university::get_current_number_lesson() {
    time_t rawtime;
    struct tm* timeinfo;
 
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    uint uint_time = (100 * timeinfo->tm_hour) + (timeinfo->tm_min);

    if (uint_time >= 800  & uint_time < 1010) { return 1; }
    if (uint_time >= 1010 & uint_time < 1200) { return 2; }
    if (uint_time >= 1200 & uint_time < 1400) { return 3; }
    if (uint_time >= 1400 & uint_time < 1550) { return 4; }
    if (uint_time >= 1550 & uint_time < 1740) { return 5; }
    if (uint_time >= 1740 & uint_time < 1930) { return 6; }
    if (uint_time >= 1930 & uint_time < 2110) { return 7; }
    if (uint_time >= 2120 & uint_time < 2250) { return 8; }

    return 0;
}

//---------------------------------------------------------------------------

uint time_university::get_current_date() {
    time_t rawtime;
    struct tm* timeinfo;
 
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    return (timeinfo -> tm_mday) * 10000 + (timeinfo -> tm_mon + 1) * 100 + 20;
}

//---------------------------------------------------------------------------