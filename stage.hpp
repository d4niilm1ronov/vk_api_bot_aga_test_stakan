#if !defined(STAGE_NAMESPACE)
#define STAGE_NAMESPACE

namespace stage {
    // Менюшки
    json menu_guest(const json&); // Меню | Гость
    json menu_user(const json&);  // Меню | Пользователь

    // Основная настройка расписания
    json setting_timetable_institute(const json&);
    json setting_timetable_year(const json&);
    json setting_timetable_group(const json&);
    json setting_timetable_lab_group(const json&);

    extern map<string, std::function<json(json)>> function;
    extern map<string, json>                      message;


};


#endif