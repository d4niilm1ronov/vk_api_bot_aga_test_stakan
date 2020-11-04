#if !defined(STAGE_NAMESPACE)
#define STAGE_NAMESPACE

namespace stage {
    // Менюшки
    void menu_guest(const json&); // Меню | Гость
    void menu_user(const json&);  // Меню | Пользователь

    // Основная настройка расписания
    void setting_timetable_institute(const json&);
    void setting_timetable_year(const json&);
    void setting_timetable_group(const json&);
    void setting_timetable_lab_group(const json&);

    extern map<string, std::function<void(json)>> function;
    extern map<string, json>                      message;


};


#endif