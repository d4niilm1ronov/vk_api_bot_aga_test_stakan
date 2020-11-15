#if !defined(DATABASE_NAMESPACE)
#define DATABASE_NAMESPACE

namespace data_base {

//---------------------------------------------------------------------------

    extern sqlite::database db;

//---------------------------------------------------------------------------

    extern vector<json> get_lesson__user(uint id_user, uint date_MMDD);
    extern vector<json> get_lesson__user(uint id_user, uint date_MMDD, uint time);

    extern vector<json> get_lesson(uint date_MMDD, uint time);


    // НАЗВАНИЕ: Проверка юзера на вхождение в базу данных
    // ОПИСАНИЕ: По ID пользователю делается запрос в лист "user".
    //           Если найдены строки (т.е. одна строка),
    //           то пользователь зарегистрирован и функция вернет true.
    extern bool check_user_id(const uint& id);


    // НАЗВАНИЕ: Добавление нового пользователя в базу данных (если написал в ЛС)
    // ОПИСАНИЕ: Если пользователь написал сообщение,
    //           но функция check_user(uint) не нашла его в базе данных,
    //           то он достоен добавления. Присваются стартовые параметры
    extern void add_user(uint user_id);


    // НАЗВАНИЕ: Получение кэша пользователя
    // ОПИСАНИЕ: 
    extern json get_user_cache(const uint& user_id);


    // НАЗВАНИЕ: Установка кэша пользователя
    // ОПИСАНИЕ: 
    extern void set_user_cache(const uint& user_id, const json& cache);


    extern string get_user_stage(const uint& user_id);


    extern void set_user_stage(const uint& user_id, const string& stage);


    extern json get_group_labs(const uint& institute, const uint& year, const uint& num);


    extern void add_lesson(const uint& user_id, const uint& id_group);
    extern void add_lesson(const uint& user_id, const uint& id_group, const uint id_lab_group);


    extern void remove_lesson(const uint& id);
    
};

#endif