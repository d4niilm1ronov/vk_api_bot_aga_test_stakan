#if !defined(VKAPI_OBJECT_PHOTO)
#define VKAPI_OBJECT_PHOTO

////////////////////////////////////////////////////////////////////////

class vkapi::photo : public vkapi::attachment {
    struct size {
        std::string  type;
        std::string  url;
        unsigned int width;
        unsigned int height;
    };

    int*         id;          //    ID фотографии
    int*         album_id;    //    ID альбома, в котором находится фото
    int*         owner_id;    //    ID владельца фото
    int*         user_id;     //    ID пользователя, который загрузил (для групп = 100)
    std::string* text;        //    Описание фотографии
    std::string* access_key;  //    Описание фотографии
    int*         data;        //    Дата добавления в формате Unixtime

    size*        sizes;       //    Массив с копиями изображения в разных размерах.
    unsigned int size_sizes;  //    <- его размер

    int* width;      // Ширина
    int* height;     // Высота

    photo(const std::string& value) :
        id (NULL), album_id(NULL), owner_id(NULL), user_id(NULL), height(NULL),
        text(NULL), data(NULL), sizes(NULL), size_sizes(0), width(NULL)

    {
        json json_obj = json::parse(value);

        if (json_obj.count("id"))       { id = new int(json_obj["id"]); }
        if (json_obj.count("album_id")) { album_id = new int(json_obj["album_id"]); }
        if (json_obj.count("owner_id")) { owner_id = new int(json_obj["owner_id"]); }
        if (json_obj.count("user_id"))  { user_id = new int(json_obj["user_id"]); }
        if (json_obj.count("data"))  { data = new int(json_obj["data"]); }
        if (json_obj.count("width"))  { width = new int(json_obj["width"]); }
        if (json_obj.count("height"))  { height = new int(json_obj["height"]); }
        if (json_obj.count("text"))  { text = new std::string(json_obj["text"]); }
        if (json_obj.count("access_key"))  { access_key = new std::string(json_obj["access_key"]); }

        if (json_obj.count("sizes"))  { size_sizes = json_obj["sizes"].size(); }

        if (size_sizes) {
            sizes = new size[size_sizes];

            for (int i = 0; size_sizes > i; i++) {
                sizes[i].type   = json_obj["sizes"][i] ["type"];
                sizes[i].url    = json_obj["sizes"][i] ["url"];
                sizes[i].width  = json_obj["sizes"][i] ["width"];
                sizes[i].height = json_obj["sizes"][i] ["height"];
            }
        }

    }

    std::string& get_text

    std::string getType() { return "photo"; }

    ~photo() {
        delete id;
        delete album_id;
        delete owner_id;
        delete user_id;
        delete text;
        delete access_key;
        delete data;
        delete width;
        delete height;

        delete[] sizes;
    }
};

////////////////////////////////////////////////////////////////////////

#endif