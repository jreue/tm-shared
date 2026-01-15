#pragma once

#include <Arduino.h>
#include <MessageStructs.h>
#include <WiFi.h>
#include <esp_now.h>

class EspNowHelper {
  public:
    EspNowHelper();

    void begin(uint8_t* hubMacAddress, int deviceId);
    void sendScannerConnected();

  private:
    uint8_t* receiverAddress;
    int deviceId;

    void sendMessage(EspNowHeader& message, size_t messageSize);
    static void handleESPNowDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
};
