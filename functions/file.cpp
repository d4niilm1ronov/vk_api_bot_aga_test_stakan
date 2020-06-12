#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <iterator>
#include <ctime>
#include <vector>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

#include "../vkAPI/support/very_eassy_curl.hpp"

#include "../vkAPI/vk_api.hpp"
#include "../vkAPI/long_poll.hpp"
#include "../vkAPI/token_vk.hpp"

#include "functions.hpp"

////////////////////////////////////////////////////////////////////////

void file::clear(const filesystem::path& file_path) {
    ofstream ofs(file_path, ios::out);
    ofs.close();

    cout << "[file][clear] Файл " << file_path.filename() << " стал пустым" << endl;
}

////////////////////////////////////////////////////////////////////////

bool file::empty(const filesystem::path& file_path) {
    return (!filesystem::file_size(file_path));
}

////////////////////////////////////////////////////////////////////////

void file::dublicate(const filesystem::path& from_file_path, filesystem::path to_dir_path = "") {
    // Узнаем локальное время
    char date[20];
    time_t t = time(NULL);
    strftime(date, 20, "%m.%d.%Y_%H.%M.%S", localtime(&t));

    // Создаем имя нового файла
    filesystem::path new_file_path = from_file_path.stem().string() + '_' + // Название файла
                                     string(date) +                         // Дата и Время
                                     from_file_path.extension().string();   // Расширение файла

    if (to_dir_path != filesystem::path("")) {
        filesystem::create_directory(to_dir_path);
    }

    to_dir_path = to_dir_path / new_file_path;
    
    // Копируем файл
    filesystem::copy(from_file_path, to_dir_path);

    cout << "[file][dublicate] Создан дубликат файла " << from_file_path.filename() << endl;
}

////////////////////////////////////////////////////////////////////////

void file::clear_folder(const filesystem::path& folder_path) {
    for (auto& ptr_file : filesystem::directory_iterator(folder_path)) {
        remove(ptr_file.path().string().c_str());
    }
}

////////////////////////////////////////////////////////////////////////