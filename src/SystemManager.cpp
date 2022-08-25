#include <SystemManager.h>
#include <WiFi.h>
#include <GSMManager.h>
#include "helpers.h"
namespace app {
bool isConnected = false;
const char *slackRootCACertificate = "-----BEGIN CERTIFICATE-----\n"
                                     "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
                                     "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                                     "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
                                     "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
                                     "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
                                     "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
                                     "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
                                     "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
                                     "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
                                     "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
                                     "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
                                     "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
                                     "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
                                     "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
                                     "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
                                     "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
                                     "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
                                     "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
                                     "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
                                     "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
                                     "-----END CERTIFICATE-----\n";

static void gotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  isConnected = true;
  ESP_LOGI(TAG, "IP: %s", WiFi.localIP().toString().c_str());
}

static void lostIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  isConnected = false;
  ESP_LOGI(TAG, "WiFi: lost connection");
}

// The getter for the instantiated singleton instance
SystemManager_ &SystemManager_::getInstance() {
  static SystemManager_ instance;
  return instance;
}

// Initialize the global shared instance
SystemManager_ &manager = manager.getInstance();

void SystemManager_::processSMSQueue() {
  struct gsm::SMSMessage pSMS;
  if (xQueueReceive(gsm::manager.smsQueue, &pSMS, (TickType_t) 0) == pdTRUE) {
    StaticJsonDocument<512> doc;
    std::string output;

    JsonObject blocks_0 = doc["blocks"].createNestedObject();
    blocks_0["type"] = "section";

    JsonObject blocks_0_text = blocks_0.createNestedObject("text");
    blocks_0_text["type"] = "mrkdwn";
    blocks_0_text["text"] = pSMS.message.c_str();

    JsonArray blocks_0_fields = blocks_0.createNestedArray("fields");

    JsonObject blocks_0_fields_0 = blocks_0_fields.createNestedObject();
    blocks_0_fields_0["type"] = "mrkdwn";
    blocks_0_fields_0["text"] = str_sprintf("*From:* %s", pSMS.number.c_str());

    serializeJson(doc, output);
    ESP_LOGI(TAG, "JSON: %s", output.c_str());

    String slackURL = preferences.getString("slack", SMS_SLACK);
    if (slackURL.isEmpty()) {
      ESP_LOGI(TAG, "Slack URL is not set %s", slackURL.c_str());
      return;
    } else {
      ESP_LOGI(TAG, "Slack URL is %s", slackURL.c_str());
    }
    sendSlack(slackURL, output);
  }
}

void SystemManager_::setup() {
  preferences.begin(PREF_NAMESPACE);
  auto name = std::string("SMS2Slack-" + get_mac_address().substr(6));
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(name.c_str());

  // reconnect
  String ssid = preferences.getString("ssid", WIFI_SSID);
  String pass = preferences.getString("pass", WIFI_PASS);
  if (!ssid.isEmpty()) {
    WiFi.begin(ssid.c_str(), pass.c_str());
  }

  WiFi.onEvent(gotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(lostIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_LOST_IP);

  wifiClient->setCACert(slackRootCACertificate);
  delay(1000);
}

void SystemManager_::loop() {
  if (isConnected) {
    processSMSQueue();
  }
}

void SystemManager_::sendSlack(String url, std::string body) {
  this->client_.setReuse(true);
  bool begin_status = this->client_.begin(url.c_str());
  if (!begin_status) {
    this->client_.end();
    ESP_LOGI(TAG, "HTTP Request failed at the begin phase. Please check the configuration");
    return;
  }
  int http_code = this->client_.POST(body.c_str());
  if (http_code < 0) {
    ESP_LOGI(TAG, "HTTP Request failed; URL: %s; Error: %s", url.c_str(), HTTPClient::errorToString(http_code).c_str());
    return;
  }

  if (http_code < 200 || http_code >= 300) {
    ESP_LOGI(TAG, "HTTP Request failed; URL: %s; Code: %d", url.c_str(), http_code);
    return;
  }
  ESP_LOGD(TAG, "HTTP Request completed; URL: %s; Code: %d", url.c_str(), http_code);
}

}  // namespace app