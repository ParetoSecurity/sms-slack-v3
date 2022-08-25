#include <cstdio>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdarg>
#include "helpers.h"
#include "esp_wifi.h"

std::string str_snake_case(const std::string &str) {
  std::string result;
  result.resize(str.length());
  std::transform(str.begin(), str.end(), result.begin(), ::tolower);
  std::replace(result.begin(), result.end(), ' ', '_');
  return result;
}
std::string str_sanitize(const std::string &str) {
  std::string out;
  std::copy_if(str.begin(), str.end(), std::back_inserter(out), [](const char &c) {
    return c == '-' || c == '_' || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  });
  return out;
}
std::string str_snprintf(const char *fmt, size_t len, ...) {
  std::string str;
  va_list args;

  str.resize(len);
  va_start(args, len);
  size_t out_length = vsnprintf(&str[0], len + 1, fmt, args);
  va_end(args);

  if (out_length < len)
    str.resize(out_length);

  return str;
}
std::string str_sprintf(const char *fmt, ...) {
  std::string str;
  va_list args;

  va_start(args, fmt);
  size_t length = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  str.resize(length);
  va_start(args, fmt);
  vsnprintf(&str[0], length + 1, fmt, args);
  va_end(args);

  return str;
}

void get_mac_address_raw(uint8_t *mac) {
#if defined(USE_ESP32)
#if defined(USE_ESP32_IGNORE_EFUSE_MAC_CRC)
  // On some devices, the MAC address that is burnt into EFuse does not
  // match the CRC that goes along with it. For those devices, this
  // work-around reads and uses the MAC address as-is from EFuse,
  // without doing the CRC check.
  esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, mac, 48);
#else
  esp_efuse_mac_get_default(mac);
#endif
#elif defined(USE_ESP8266)
  wifi_get_macaddr(STATION_IF, mac);
#endif
}

std::string get_mac_address() {
  uint8_t mac[6];
  get_mac_address_raw(mac);
  return str_snprintf("%02x%02x%02x%02x%02x%02x", 12, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
std::string get_mac_address_pretty() {
  uint8_t mac[6];
  get_mac_address_raw(mac);
  return str_snprintf("%02X:%02X:%02X:%02X:%02X:%02X", 17, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}