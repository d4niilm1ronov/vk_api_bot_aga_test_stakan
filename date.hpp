#if !defined(DATE_NAMESPACE)
#define DATE_NAMESPACE

typedef unsigned int uint;

namespace time_stakan {

    //---------------------------------------------------------------------

    struct date {

        tm struct_tm;

        //-----------------------------------------------------------------

        // YY [0-99] , MM [01-12] , DD [01-31]
        date(const uint& YYMMDD);

        // d: от 1 до 31 (день месяца)
        // m: от 1 до 12 (номер месяца)
        // y: год (2014) или его последние 2 цифры (14)
        date(const uint& d, const uint& m, const uint& y);

        //-----------------------------------------------------------------

        uint format_yymmdd() const;

        date plus_one_week() const;
        date plus_two_week() const;

        //-----------------------------------------------------------------

        explicit operator string() const {

            return(to_string(struct_tm.tm_mday) + "." + to_string(struct_tm.tm_mon + 1));

        }
    };

    //---------------------------------------------------------------------

    extern uint last_number_lesson;

    //---------------------------------------------------------------------

    uint get_current_number_lesson();

    date get_current_date();

};

#endif