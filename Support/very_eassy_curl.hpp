#if !defined(VERY_EASSY_CURL)
#define VERY_EASSY_CURL

// Копирование из curl в string
std::size_t cts(const char*, std::size_t, std::size_t, std::string*);

// Запрос с помощью CURL
std::string reqCURL(CURL*, const std::string&);

#endif