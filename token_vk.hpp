#if !defined(VKAPI_TOKEN_VK)
#define VKAPI_TOKEN_VK

class vkapi::token_base {
    protected:
        mutable CURL* objCURL;
        string TOKEN;

    public:
        token_base(string token) : TOKEN(token), objCURL(curl_easy_init()) {
            curl_easy_setopt(objCURL, CURLOPT_WRITEFUNCTION, cts);
        }


};

class vkapi::token_group : public vkapi::token_base {
    private:
        unsigned int ID;

    public:
        token_group(string token, unsigned int id) : token_base(token), ID(id) {
            
        }


        void messages_send(const vkapi::message&) const;
        vkapi::bots_long_poll groups_getLongPollServer() const;


};

#endif