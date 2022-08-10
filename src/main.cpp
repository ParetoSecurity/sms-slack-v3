#include <Arduino.h>
#include <SystemManager.h>
#include <GSMManager.h>
#include "helpers.h"

void setup()
{
  Serial.begin(115200);
  gsm::manager.setup();
  app::manager.setup();
}

void loop()
{
  gsm::manager.loop();
  app::manager.loop();
}
