#pragma once

#include <Arduino.h>
#include <GsmModule.h>
#include <SimcomAtCommands.h>
#include <GsmLibHelpers.h>
#include <OperatorNameHelper.h>
#include <GsmModule.h>
#include <SimcomAtCommandsEsp32.h>
#include <FixedString.h>
#include <ArduinoJSON.h>
#include "helpers.h"

namespace gsm
{
    static const char *const TAG = "GSMManager";

    extern struct SMSMessage
    {
        FixedString16 number;
        FixedString256 message;
        time_t smsTime;
    } xMessage;

    class GSMManager_
    {
    private:
        GSMManager_() = default;

    public:
        static GSMManager_ &getInstance();

        void setup();
        void loop();
        QueueHandle_t smsQueue;
    };

    extern GSMManager_ &manager;
}