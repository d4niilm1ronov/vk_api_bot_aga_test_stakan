#if !defined(MAIN_NAMESPACE)
#define MAIN_NAMESPACE

/////////////////////////////////////////////////////////////////////////////

// VK API
extern vkapi::token_group stankin_bot;

//---------------------------------------------------------------------------

namespace easy {
    namespace vkapi {
        extern json messages_send(json message);
        extern json messages_send(json message, const uint& peer_id);
        extern json messages_send(const string& text, const uint& peer_id);
    };
};

//---------------------------------------------------------------------------

bool its_text_message(const json&);
string what_menu(unsigned int);

string kill_rus_e(string);
string to_upper_rus(string);
bool its_rus_name(string);

//---------------------------------------------------------------------------

#endif