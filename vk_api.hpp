#if !defined(VKAPI_NAMESPACE)
#define VKAPI_NAMESPACE

namespace vkapi {


    class long_poll;
    class bots_long_poll;

    class token_base;
    class token_group;

    struct answer_botsLP;

    struct event;

    struct object;
        struct message;
        class attachment;
            class photo;

    /*
    [ === Медиавложения === ]
    */

    struct video;
    struct audio;
    struct doc;
    struct link;
    struct market;
    struct market_album;
    struct wall;
    struct wall_reply;
    struct sticker;
    struct gift;

    // Объект, описывающий граффити
    // Он может быть возвращен лишь для записей,
    // созданных раньше 2013 года.
    // После 2013 года возвращается объект photo. 
    struct graffiti;

    // Объект, описывающий заметку
    struct note;

    // Это устаревший тип вложений.
    // Он возвращается для записей, созданных раньше 2013 года. 
    struct app;

    // Объект, описывающий опрос
    struct poll;

    // Объект, описывающий вики-страницу
    struct page;

    // Объект, описывающий альбом с фотографиями
    struct album;

    // Массив из строк, содержащих идентификаторы фотографий.
    // Сами фотографии дублируются в виде приложенных объектов фотографий,
    // однако этот список необходим в случае,
    // если фотографий больше макс. кол-во возвращаемых вложений (10)
    struct photos_list;

    // Массив элементов-карточек
    struct pretty_cards;

    // Объект, описывающий встречу
    struct event;

    // Информация о местоположении
    struct geo;

    struct keyboard;

    struct push_setting;
};




#endif