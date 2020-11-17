#if !defined(DATE_NAMESPACE)
#define DATE_NAMESPACE

typedef unsigned int uint;

namespace time_stakan {

    //---------------------------------------------------------------------

    struct date {

        tm struct_tm;

        //-----------------------------------------------------------------

        // MM [01-12] , DD [01-31]
        date(const uint& MMDD);

        // d: от 1 до 31 (день месяца)
        // m: от 1 до 12 (номер месяца)
        date(const uint& d, const uint& m);

        //-----------------------------------------------------------------

        uint format_mmdd() const;

        date plus_one_day() const;
        date plus_one_week() const;
        date plus_two_week() const;

        //-----------------------------------------------------------------

        bool operator> (const time_stakan::date&) const;
        bool operator>= (const time_stakan::date&) const;
        bool operator< (const time_stakan::date&) const;
        bool operator<= (const time_stakan::date&) const;
        bool operator== (const time_stakan::date&) const;
        bool operator!= (const time_stakan::date&) const;

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