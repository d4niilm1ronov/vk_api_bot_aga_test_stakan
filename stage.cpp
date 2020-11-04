#include <iostream>
#include <filesystem>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <map>

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <string>
#include <dirent.h>
#include <cstdio>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Библиотека для работы с SQLite 3 & C++
#include "sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

// VK API
#include "vkAPI/support/very_eassy_curl.hpp"
#include "vkAPI/vk_api.hpp"
#include "vkAPI/long_poll.hpp"
#include "vkAPI/token_vk.hpp"

#include "data_base.hpp"

#include "additionally.hpp"

#include "stage.hpp"

// фикс
vkapi::token_group& test_token = stankin_bot;

map<string, std::function<void(json)>> stage::function;
map<string, json>                      stage::message;