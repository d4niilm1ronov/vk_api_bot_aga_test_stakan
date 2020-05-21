#include <iostream>
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <ctime>
#include <unistd.h>

using namespace std;
using json = nlohmann::json;

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