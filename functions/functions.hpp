#if !defined(DBUSER)
#define DBUSER

namespace file {
    void clear(const filesystem::path&);
    bool empty(const filesystem::path&);
    void dublicate(const filesystem::path&, filesystem::path);
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
        extern fstream changelog;

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
    unsigned int stage;

    info() : send_id(0), level(0), stage(100000) {}

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

#endif