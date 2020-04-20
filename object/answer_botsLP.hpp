#if !defined(VKAPI_ANSWER_BOTSLP)
#define VKAPI_ANSWER_BOTSLP

////////////////////////////////////////////////////////////////////////


struct vkapi::answer_botsLP {
    
    struct update {
        std::string      type;
        vkapi::object*   obj;
        unsigned int     group_id;
        std::string      event_id;
    };

    unsigned int   *ts;              // 
    unsigned int   *failed;          // 
    update         *updates;         // Массив событий
    unsigned int    count_updates;   // Кол-во событий (в массиве)

    answer_botsLP(const std::string& req) :
        ts(NULL),
        failed(new unsigned int(0)),
        updates(NULL),
        count_updates(0)
    
    {
        json req_json = json::parse(req);

        // Считывание ошибки (если есть)
        if (req_json.count("failed")) { *failed = req_json["failed"]; }

        // Если запрос ошибочный
        if (*failed != 0) {
            if (*failed == 1) { ts = new unsigned int(req_json["ts"]); }
            //   failed == 2
            //   failed == 3
        } else

        // Если запрос пустой
        if (req_json["updates"].size() == 0) {
            ts = new unsigned int (atoi(std::string(req_json["ts"]).c_str()));
        }
        
        // Нормальный запрос
        else {
            count_updates = req_json["updates"].size();
            updates       = new update[count_updates];
            ts            = new unsigned int (atoi(std::string(req_json["ts"]).c_str()));


            for (unsigned int k = 0; count_updates > k; k++) {
                updates[k].type     = req_json["updates"][k]["type"];
                updates[k].event_id = req_json["updates"][k]["event_id"];
                updates[k].group_id = req_json["updates"][k]["group_id"];

                if (updates[k].type == "message_new") {
                    updates[k].obj = new vkapi::message(req_json["updates"][k]["object"]["message"].dump());

                    // ... client info support ...
                }

                // ... more ...

                else {
                    updates[k].obj = NULL;
                }
            }
            
        }

    }

    update operator[] (unsigned int index) {
        return updates[index];
    }

};


////////////////////////////////////////////////////////////////////////


#endif