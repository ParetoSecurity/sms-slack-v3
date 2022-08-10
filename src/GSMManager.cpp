#include <GSMManager.h>

namespace gsm {
SimcomAtCommandsEsp32 gsmAt(Serial2, SMS_TX, SMS_RX, -1);
void OnLog(const char *logLine, bool flush) { ESP_LOGI(TAG, "%s", logLine); }
// The getter for the instantiated singleton instance
GSMManager_ &GSMManager_::getInstance() {
  static GSMManager_ instance;
  return instance;
}

// Initialize the global shared instance
GSMManager_ &manager = manager.getInstance();

void GSMManager_::setup() {
  // gsmAt.Logger().LogAtCommands = true;
  // gsmAt.Logger().OnLog(OnLog);
  this->smsQueue = xQueueCreate(40, sizeof(struct SMSMessage));
}

void GSMManager_::loop() {
  if (!gsmAt.EnsureModemConnected(115200)) {
    ESP_LOGI(TAG, "No modem found");
    delay(500);
    return;
  }
  int16_t signalQuality;
  GsmRegistrationState registrationStatus;
  FixedString32 imei;

  gsmAt.EnableLTE(true);
  gsmAt.GetSignalQuality(signalQuality);
  gsmAt.GetImei(imei);

  ESP_LOGI(TAG, "Signal: %d", signalQuality);
  ESP_LOGI(TAG, "IMEI: %s", imei.c_str());

  if (gsmAt.GetRegistrationStatus(registrationStatus) == AtResultType::Success) {
    ESP_LOGI(TAG, "Status: %s", RegStatusToStr(registrationStatus));
  } else {
    return;
  }

  uint16_t smsIndex = -1;
  if (gsmAt.GetLastSmsIndexForRead(smsIndex) == AtResultType::Success) {
    // New SMS received
    if (smsIndex < 1) {
      ESP_LOGI(TAG, "SMS: 0");
      delay(1000);
      return;
    }

    struct SMSMessage pSMS;
    if (gsmAt.ReadSms(smsIndex - 1, pSMS.number, pSMS.message, pSMS.smsTime) == AtResultType::Success) {
      ESP_LOGI(TAG, "SMS: %d", smsIndex);
      if (xQueueSend(this->smsQueue, (void *) &pSMS, (TickType_t) 0) == pdTRUE) {
        ESP_LOGI(TAG, "SMS: sent to queue");
        gsmAt.DeleteSms(smsIndex - 1);
        return;
      }
    }
  }
  delay(1000);
}
}  // namespace gsm