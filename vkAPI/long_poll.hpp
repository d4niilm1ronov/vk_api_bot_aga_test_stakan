#if !defined(VKAPI_LONG_POLL_API)
#define VKAPI_LONG_POLL_API


////////////////////////////////////////////////////////////////////////


class vkapi::long_poll {
    protected:
        std::string server_str;
        std::string key_str;
        unsigned int ts_int;

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
            ts_int(ts),
            objCURL(curl_easy_init())
        {
            curl_easy_setopt(objCURL, CURLOPT_WRITEFUNCTION, cts);
        }

        unsigned int get_ts() const;
        void set_ts(const unsigned int&);

        virtual nlohmann::json request_lp() const =0;

        // Делает Long Poll запрос
        // Возвращает true если выполнен без ошибок
        // virtual bool request_lp() =0;

};


////////////////////////////////////////////////////////////////////////


class vkapi::bots_long_poll: public vkapi::long_poll {
    public:
        bots_long_poll(
            const std::string& server_str,
            const std::string& key_str,
            const unsigned int last_ts = 0
        ) :
            vkapi::long_poll(server_str, key_str, last_ts)
        {
            // Настройки Long Poll
        }

        nlohmann::json request_lp() const;

};


////////////////////////////////////////////////////////////////////////


#endif