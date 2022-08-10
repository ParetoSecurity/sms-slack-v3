#include <Arduino.h>
#include <SystemManager.h>
#include <GSMManager.h>
#include "helpers.h"
#include <ImprovSerial.h>

void setup() {
  Serial.begin(115200);
  auto name = std::string("SMS2Slack-" + get_mac_address().substr(6));
  improv_serial::instance.setup(F("SMS2Slack"), SMS2SLACK_VERSION, PLATFORMIO_ENV, name.c_str());
  gsm::manager.setup();
  app::manager.setup();
}

void loop() {
  improv_serial::instance.loop();
  gsm::manager.loop();
  app::manager.loop();
}
