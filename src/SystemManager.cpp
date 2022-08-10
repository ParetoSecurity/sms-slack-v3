#include <SystemManager.h>
#include <WiFi.h>
#include <ImprovSerial.h>
#include <GSMManager.h>
#include "helpers.h"
namespace app
{
    // The getter for the instantiated singleton instance
    SystemManager_ &SystemManager_::getInstance()
    {
        static SystemManager_ instance;
        return instance;
    }

    // Initialize the global shared instance
    SystemManager_ &manager = manager.getInstance();

    void SystemManager_::processSMSQueue()
    {
        struct gsm::SMSMessage *pSMS;
        if (xQueueReceive(gsm::manager.smsQueue, &(pSMS), (TickType_t)10))
        {
            StaticJsonDocument<512> doc;
            char output[512];

            JsonObject blocks_0 = doc["blocks"].createNestedObject();
            blocks_0["type"] = "section";

            JsonObject blocks_0_text = blocks_0.createNestedObject("text");
            blocks_0_text["type"] = "mrkdwn";
            blocks_0_text["text"] = pSMS->message.c_str();

            JsonArray blocks_0_fields = blocks_0.createNestedArray("fields");

            JsonObject blocks_0_fields_0 = blocks_0_fields.createNestedObject();
            blocks_0_fields_0["type"] = "mrkdwn";
            blocks_0_fields_0["text"] = str_sprintf("*From:* %s", pSMS->number);

            JsonObject blocks_0_fields_1 = blocks_0_fields.createNestedObject();
            blocks_0_fields_1["type"] = "mrkdwn";
            blocks_0_fields_1["text"] = str_sprintf("*At:* %s", pSMS->smsTime);

            serializeJson(doc, output);
        }
    }
    void SystemManager_::setup()
    {
        auto name = std::string("SMS2Slack-" + get_mac_address().substr(6));
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(name.c_str());
        improv_serial::instance.setup(name.c_str(), SMS2SLACK_VERSION, PLATFORMIO_ENV, WiFi.macAddress().c_str());
        delay(1000);
    }

    void SystemManager_::loop()
    {
        improv_serial::instance.loop();
        processSMSQueue();
    }

}