#include <string>

#include <curl/curl.h>

using namespace std;

#include "very_eassy_curl.hpp"

////////////////////////////////////////////////////////////////////////

size_t cts(const char *ptr, size_t size, size_t nmemb, string* data) {
    if (data) {
        data -> append(ptr, size*nmemb);
        return (size * nmemb);
    }
    else {
        return 0;  // будет ошибка
    }
}

////////////////////////////////////////////////////////////////////////

string reqCURL(CURL* ptr, const string& str_request) {
    curl_easy_setopt(ptr, CURLOPT_URL, str_request.c_str());

    string result_req;
    curl_easy_setopt(ptr, CURLOPT_WRITEDATA, &result_req);
    curl_easy_perform(ptr);
    
    return result_req;
}

////////////////////////////////////////////////////////////////////////