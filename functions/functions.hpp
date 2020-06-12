#if !defined(FUNCTIONS)
#define FUNCTIONS

////////////////////////////////////////////////////////////////////////

extern vector<unsigned int> notification;
extern map<string, json> ready_mesg;

////////////////////////////////////////////////////////////////////////

extern const string my_token;
extern const unsigned int group_id;
extern vkapi::token_group test_token;
extern vkapi::bots_long_poll test_blp;

////////////////////////////////////////////////////////////////////////

namespace file {
    void clear(const filesystem::path&);
    bool empty(const filesystem::path&);
    void dublicate(const filesystem::path&, filesystem::path);
    void clear_folder(const filesystem::path&);
};

////////////////////////////////////////////////////////////////////////

namespace data_base {


    //--------------------[ Работа с пользователем ]--------------------

    namespace user {
        struct info;

        // Добавление пользователя
        void add(const unsigned int&);

        // Проверка на наличие в std::map
        bool check(const unsigned int&);

        // Сохранить изменение пользователя
        void push_changelog(const unsigned int&);

    };

    //-------------------[ Работа с пользователями ]--------------------

    namespace users {
        extern filesystem::path data_path;
        extern map<unsigned int, data_base::user::info> data;

        extern filesystem::path changelog_path;

        extern map<unsigned int, json> hash;

        // Пользователи: Постоянная -> Оперативная память
        void download_data();

        // Пользователи: Оперативная -> Постоянная память
        void upload_data();

        // Изменения: Постоянная -> Оперативная память
        void download_changelog();

    };
};

////////////////////////////////////////////////////////////////////////

struct data_base::user::info {
    unsigned int send_id;
    unsigned int level;
    string stage;

    info() : send_id(0), level(0), stage("menu_guest") {}

    info(const json& user_json) :
        send_id(user_json["send_id"]),
        level(user_json["level"]),
        stage(user_json["stage"])
    {
        // . . .
    }

    operator json() {
        json result;
        result["send_id"] = send_id;
        result["level"] = level;
        result["stage"] = stage;

        return result;
    }
};

////////////////////////////////////////////////////////////////////////

namespace stage {
    // Менюшки
    void menu_guest(const json&); // Меню | Гость
    void menu_user(const json&);  // Меню | Пользователь

    // Основная настройка расписания
    void setting_timetable_institute(const json&);
    void setting_timetable_year(const json&);
    void setting_timetable_group(const json&);
    void setting_timetable_lab_group(const json&);

    void search_teacher_input(const json&);

    void search_room_category(const json&);

    extern map<string, std::function<void(json)>> function;
    extern map<string, json>                      message;
};

////////////////////////////////////////////////////////////////////////

namespace date {
    extern unsigned int difference;
    extern time_t current_time;
    extern unsigned int current_lesson;
    extern string arr_wday[];

    tm to_tm(const string&);
    tm get_current_date();
    bool operator>(const tm&, const tm&);
    bool operator<(const tm&, const tm&);

    bool its_spring_semester(const tm&);
    bool its_fall_semester(const tm&);
    int get_stage_tt();
    bool time_to_break();
};

////////////////////////////////////////////////////////////////////////

namespace general {
    void upload();
    void refresh();
};

////////////////////////////////////////////////////////////////////////

bool its_text_message(const json&);
string what_menu(unsigned int);

string kill_rus_e(string);
string to_upper_rus(string);
bool its_rus_name(string);

#endif