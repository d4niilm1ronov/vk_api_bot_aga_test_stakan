#if !defined(VKAPI_TOKEN_VK)
#define VKAPI_TOKEN_VK

class vkapi::token_base {
    protected:
        mutable CURL* objCURL;
        string TOKEN;

    public:
        token_base(string);


};

class vkapi::token_group : public vkapi::token_base {
    private:
        unsigned int ID;

    public:
        token_group(void);
        token_group(string, unsigned int);

        nlohmann::json messages_send(const nlohmann::json&) const;
        nlohmann::json messages_send(const unsigned int&, nlohmann::json) const;
        nlohmann::json messages_send(const unsigned int&, string) const;

        vkapi::bots_long_poll groups_getLongPollServer() const;

        token_group& operator=(const token_group&);
};

#endif