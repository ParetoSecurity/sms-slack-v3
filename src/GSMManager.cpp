#include <GSMManager.h>

namespace gsm
{
    SimcomAtCommandsEsp32 gsmAt(Serial2, SMS_RX, SMS_TX, SMS_DTR);
    GsmModule gsm(gsmAt);

    void OnLog(const char *logLine, bool flush)
    {
        ESP_LOGI(TAG, "%s", logLine);
    }

    // The getter for the instantiated singleton instance
    GSMManager_ &GSMManager_::getInstance()
    {
        static GSMManager_ instance;
        return instance;
    }

    // Initialize the global shared instance
    GSMManager_ &manager = manager.getInstance();

    void GSMManager_::setup()
    {
        gsmAt.Logger().LogAtCommands = true;
        gsm.OnLog(OnLog);
        this->smsQueue = xQueueCreate(10, sizeof(struct SMSMessage *));
    }

    void GSMManager_::loop()
    {
        if (!gsmAt.EnsureModemConnected(115200))
        {
            ESP_LOGI(TAG, "No modem found");
            delay(500);
            return;
        }

        int16_t signalQuality;
        IncomingCallInfo callInfo;
        GsmRegistrationState registrationStatus;
        FixedString32 imei;

        gsmAt.GetSignalQuality(signalQuality);
        gsmAt.GetIncomingCall(callInfo);
        gsmAt.GetImei(imei);

        ESP_LOGI(TAG, "Signal quality: %d", signalQuality);

        if (gsmAt.GetRegistrationStatus(registrationStatus) == AtResultType::Success)
        {
            ESP_LOGI(TAG, "reg status: %s", RegStatusToStr(registrationStatus));
        }
        else
        {
            ESP_LOGI(TAG, "Failed to get reg status");
        }

        if (callInfo.HasIncomingCall)
        {
            // callInfo.CallerNumber.c_str()
            gsmAt.HangUp();
        }

        if (gsm.simStatus == SimState::Locked)
        {
            ESP_LOGI(TAG, "Sim card is locked by PIN");
        }
        if (gsm.simStatus == SimState::NotInserted)
        {
            ESP_LOGI(TAG, "Sim card not inserted");
        }

        uint16_t smsIndex;
        if (gsmAt.GetLastSmsIndexForRead(smsIndex) == AtResultType::Success)
        {
            struct SMSMessage pSMS;
            if (gsmAt.ReadSms(smsIndex, pSMS.number, pSMS.message, pSMS.smsTime) == AtResultType::Success)
            {
                if (xQueueSend(this->smsQueue, (void *)&pSMS, (TickType_t)0) == pdTRUE)
                {
                    ESP_LOGI(TAG, "SMS sent to queue");
                }
            }
        }
    }
}