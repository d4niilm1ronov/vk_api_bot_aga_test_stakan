#if !defined(VKAPI_LONG_POLL_API)
#define VKAPI_LONG_POLL_API


////////////////////////////////////////////////////////////////////////


class vkapi::long_poll {
    protected:
        std::string server_str;
        std::string key_str;
        unsigned int last_ts;
        unsigned int error;

        mutable CURL* objCURL;


//----------------------------------------------------------------------


    public:
        long_poll(
            const std::string& server,
            const std::string& key,
            const unsigned int ts
        ) :
            server_str(server),
            key_str(key),
            last_ts(ts),
            error(0),
            objCURL(curl_easy_init())
        {
            curl_easy_setopt(objCURL, CURLOPT_WRITEFUNCTION, cts);
        }

        unsigned int get_ts() const;
        void set_ts(const unsigned int&);

        unsigned int get_error() const;

        // Делает Long Poll запрос
        // Возвращает true если выполнен без ошибок
        // virtual bool request_lp() =0;

};


////////////////////////////////////////////////////////////////////////


class vkapi::bots_long_poll: public vkapi::long_poll {

    public:
        /*
        struct set {

            bool enabled;

            bool message_new;            // Входящие сообщения
            bool message_typing_state;   // Начало нового диалога (печатанья)
            bool message_reply;          // Исходящие сообщения
            bool message_edit;           // Редактирование сообщения
            bool message_allow;          // Подписка на сообщения
            bool message_deny;           // Запрет на сообщения

            // bool photo_new;
            // bool audio_new;
            // bool video_new;

            // bool wall_reply_new;
            // bool wall_reply_edit;
            // bool wall_reply_delete;
            // bool wall_post_new;
            // bool wall_repost;

            // bool board_post_new;
            // bool board_post_edit;
            // bool board_post_delete;
            // bool board_post_restore;

            // bool photo_comment_new;
            // bool photo_comment_edit;
            // bool photo_comment_delete;
            // bool photo_comment_restore;

            // bool video_comment_new;
            // bool video_comment_edit;
            // bool video_comment_delete;
            // bool video_comment_restore;

            // bool market_comment_new;
            // bool market_comment_edit;
            // bool market_comment_delete;
            // bool market_comment_restore;

            // bool poll_vote_new;
            // bool group_join;
            // bool group_leave;
            // bool user_block;
            // bool user_unblock;
            // bool group_change_setting;
            // bool group_change_photo;
            // bool group_officer_edit;

        } setting;
        */

        bots_long_poll(
            const std::string& server_str,
            const std::string& key_str,
            const unsigned int last_ts = 0
        ) :
            vkapi::long_poll(server_str, key_str, last_ts)
        {
            // ...

            // Настройки Long Poll
        }

        // Делает Long Poll запрос
        vkapi::answer_botsLP request_lp();

};


////////////////////////////////////////////////////////////////////////


#endif