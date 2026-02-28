#pragma once

#include <Arduino.h>
#include <MessageStructs.h>
#include <WiFi.h>
#include <esp_now.h>

class EspNowHelper {
  public:
    EspNowHelper();

    void begin(int deviceId);
    void addPeer(uint8_t* macAddress);

    void registerModuleMessageHandler(void (*handler)(const ShieldModuleMessage&));
    void registerDateMessageHandler(void (*handler)(const DateMessage&));
    void registerScannerMessageHandler(void (*handler)(const ScannerMessage&));
    void registerOrientationMessageHandler(void (*handler)(const OrientationSubmissionMessage&));

    void sendDateConnected(uint8_t* targetAddress);
    void sendScannerConnected(uint8_t* targetAddress);
    void sendModuleConnected(uint8_t* targetAddress);

    void sendDateUpdated(uint8_t* targetAddress, uint8_t month, uint8_t day, uint16_t year);
    void sendModuleUpdated(uint8_t* targetAddress, bool isCalibrated);
    void sendOrientationUpdated(uint8_t* targetAddress, uint16_t roll, uint16_t pitch, uint16_t yaw,
                                uint8_t round, boolean success);

  private:
    static EspNowHelper* instance;

    int deviceId;

    void (*moduleMessageHandler)(const ShieldModuleMessage&) = nullptr;
    void (*dateMessageHandler)(const DateMessage&) = nullptr;
    void (*scannerMessageHandler)(const ScannerMessage&) = nullptr;
    void (*orientationMessageHandler)(const OrientationSubmissionMessage&) = nullptr;

    void callModuleMessageHandler(const ShieldModuleMessage& message);
    void callDateMessageHandler(const DateMessage& message);
    void callScannerMessageHandler(const ScannerMessage& message);
    void callOrientationMessageHandler(const OrientationSubmissionMessage& message);

    void sendMessage(uint8_t* targetAddress, EspNowHeader& message, size_t messageSize);

    static void handleESPNowDataReceived(const uint8_t* mac_addr, const uint8_t* data,
                                         int data_len);
    static void handleESPNowDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
};
