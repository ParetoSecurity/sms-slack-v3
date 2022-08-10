#pragma once

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>

namespace app {
static const char *const TAG = "SystemManager";
static void gotIP(WiFiEvent_t event, WiFiEventInfo_t info);
static void lostIP(WiFiEvent_t event, WiFiEventInfo_t info);

class SystemManager_ {
 private:
  SystemManager_() = default;
  void processSMSQueue();
  void sendSlack(String url, std::string body);

 protected:
  HTTPClient client_{};
  Preferences preferences;
  WiFiClientSecure *wifiClient = new WiFiClientSecure;

 public:
  static SystemManager_ &getInstance();

  void setup();
  void loop();
};

extern SystemManager_ &manager;
}  // namespace app