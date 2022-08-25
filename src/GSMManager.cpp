#include <GSMManager.h>

namespace gsm {
#ifdef DEBUG_SLACK
bool sent = false;
#endif
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
  TickType_t xLastWakeTime = xTaskGetTickCount();
#ifdef DEBUG_SLACK
  if (!sent) {
    sent = true;
    struct SMSMessage pSMS {};
    pSMS.message = "foo bar";
    pSMS.number = "123123123";
    if (xQueueSend(this->smsQueue, (void *) &pSMS, (TickType_t) 0) == pdTRUE) {
      ESP_LOGI(TAG, "SMS: sent to queue");
      return;
    }
  }
#endif
  if (!gsmAt.EnsureModemConnected(115200)) {
    ESP_LOGI(TAG, "No modem found");
    vTaskDelayUntil(&xLastWakeTime, pdTICKS_TO_MS(500));
    return;
  }
  FixedString256 ops;
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
  gsmAt.GetOperatorName(ops);
  ESP_LOGI(TAG, "Operator: %s", ops.c_str());

  uint16_t smsIndex = -1;
  if (gsmAt.GetLastSmsIndexForRead(smsIndex) == AtResultType::Success) {
    // New SMS received
    if (smsIndex < 1) {
      ESP_LOGI(TAG, "SMS: 0");
      vTaskDelayUntil(&xLastWakeTime, pdTICKS_TO_MS(1500));
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
  vTaskDelayUntil(&xLastWakeTime, pdTICKS_TO_MS(2000));
}
}  // namespace gsm