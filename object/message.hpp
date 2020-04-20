#if !defined(VKAPI_OBJECT_MESSAGE)
#define VKAPI_OBJECT_MESSAGE

////////////////////////////////////////////////////////////////////////

struct vkapi::message : public vkapi::object {

//----------------------------------------------------------------------

        unsigned int      *id;                   // Идентификатор сообщения
        unsigned int      *date;                 // Время отправки в Unixtime
        int               *peer_id;              // Идентификатор назначения
        int               *from_id;              // Идентификатор отправителя
        string            *text;                 // Текст сообщения
        unsigned int      *random_id;            // Идентификатор, для исходящих сообщения
        string            *ref;                  // Произвольный параметр для работы с Источником переходов
        string            *ref_source;           // Произвольный параметр для работы с Источником переходов

        bool              *important;            // Пометка "важное сообщение"
        string            *payload;              // Сервисное поле для сообщений ботам (полезная нагрузка)
        vkapi::keyboard   *keyboard;             // Объект клавиатуры для ботов
        vkapi::message    *replay_message;       // Сообщение, в ответ на которое отправлено текущее.

        vkapi::message    *fwd_messages;         // Массив пересланных сообщений
        unsigned int      count_fwd_messages;    // Счётчик fwd_messages

        vkapi::attachment  **attachments;        // Массив медиавложений сообщений
        unsigned int       count_attachments;    // Счетчик attachments

        unsigned int      *conversation_message_id;

        // Информация о сервисном действии с чатом
        struct struct_action {
            string  *type;        // Тип действия
            int     *member_id;   // C кем это действие связанно (если пользователь)
            string  *email;       // C кем это действие связанно (если email)
            string  *text;        // Название беседы (если поменяли)
            
            // Новое изображение чата
            struct photo {
                string  *photo_50;    // URL изображения 50x50px;
                string  *photo_100;   // URL изображения 100x100px;
                string  *photo_200;   // URL изображения 200x200px;
            } *photo;
        } *action;

////////////////////////////////////////////////////////////////////////

        message(const std::string& req) :
            conversation_message_id(NULL), action(NULL),
            id(NULL), date(NULL), peer_id(NULL), from_id(NULL),
            text(NULL), random_id(NULL), ref(NULL), ref_source(NULL),
            important(NULL), payload(NULL), keyboard(NULL), replay_message(NULL),
            count_attachments(0), count_fwd_messages(0), attachments(NULL), fwd_messages(NULL)
        {
            json req_json = json::parse(req);

            cout << "New Message!" << endl;

            // Чтение основных полей
            if (req_json.count("id"))         { id         = new unsigned int(req_json["id"]); }       
            if (req_json.count("date"))       { date       = new unsigned int(req_json["date"]); }     
            if (req_json.count("peer_id"))    { peer_id    = new int(req_json["peer_id"]); }           
            if (req_json.count("from_id"))    { from_id    = new int(req_json["from_id"]); }           
            if (req_json.count("text"))       { text       = new string(req_json["text"]); }           
            if (req_json.count("random_id"))  { random_id  = new unsigned int(req_json["random_id"]); }
            if (req_json.count("ref"))        { ref        = new string(req_json["ref"]); }            
            if (req_json.count("ref_source")) { ref_source = new string(req_json["ref_source"]); }

            // Чтение attachments
            if (req_json.count("attachments")) {
                if (req_json["attachments"].size()) {
                    count_attachments = req_json["attachments"].size();
                    attachments = new vkapi::attachment*[count_attachments];

                    for (int i = 0; count_attachments > i; i++) {
                        if (req_json["attachments"][i]["type"] == "photo")
                            { attachments[i] = new vkapi::photo(req_json["attachments"][i]["photo"].dump()); }

                        // + + + + +
                    }
                }
            }
        }
};     

////////////////////////////////////////////////////////////////////////

#endif