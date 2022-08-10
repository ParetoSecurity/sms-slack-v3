#ifndef HELPERS_H_
#define HELPERS_H_

#include <string>
#include <cstdio>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdarg>

std::string str_snake_case(const std::string &str);
std::string str_sanitize(const std::string &str);
std::string str_snprintf(const char *fmt, size_t len, ...);
std::string str_sprintf(const char *fmt, ...);
void get_mac_address_raw(uint8_t *mac);
std::string get_mac_address();
std::string get_mac_address_pretty();

#endif /* HELPERS_H_ */