#pragma once

#include <Arduino.h>
#include <MessageStructs.h>
#include <WiFi.h>
#include <esp_now.h>

class EspNowHelper {
  public:
    EspNowHelper();

    void begin(uint8_t* hubMacAddress, int deviceId);

    void registerModuleMessageHandler(void (*handler)(const DeviceMessage&));
    void registerDateMessageHandler(void (*handler)(const DateMessage&));
    void registerScannerMessageHandler(void (*handler)(const ScannerMessage&));

    void sendDateConnected();
    void sendScannerConnected();
    void sendModuleConnected();

    void sendDateUpdated(uint8_t month, uint8_t day, uint16_t year);
    void sendModuleUpdated(bool isCalibrated);

  private:
    static EspNowHelper* instance;

    uint8_t* receiverAddress;
    int deviceId;

    void (*deviceMessageHandler)(const DeviceMessage&) = nullptr;
    void (*dateMessageHandler)(const DateMessage&) = nullptr;
    void (*scannerMessageHandler)(const ScannerMessage&) = nullptr;

    void callDeviceMessageHandler(const DeviceMessage& message);
    void callDateMessageHandler(const DateMessage& message);
    void callScannerMessageHandler(const ScannerMessage& message);

    void sendMessage(EspNowHeader& message, size_t messageSize);

    static void handleESPNowDataReceived(const uint8_t* mac_addr, const uint8_t* data,
                                         int data_len);
    static void handleESPNowDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
};
