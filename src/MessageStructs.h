#pragma once

#include <Arduino.h>

// Device Types
#define DEVICE_TYPE_HUB 0
#define DEVICE_TYPE_DATE 1
#define DEVICE_TYPE_SCANNER 2
#define DEVICE_TYPE_MODULE 3

// Message types
#define MSG_TYPE_CONNECT 0
#define MSG_TYPE_DATA 1

// Common header for all messages
struct EspNowHeader {
    uint8_t deviceId;
    uint8_t deviceType;
    uint8_t messageType;
};

// Device message (connection/calibration status)
struct DeviceMessage {
    uint8_t deviceId;
    uint8_t deviceType;
    uint8_t messageType;

    bool isCalibrated;
};

// Scanner message
struct ScannerMessage {
    uint8_t deviceId;
    uint8_t deviceType;
    uint8_t messageType;

    // May have button states in future
};

// Date module message
struct DateMessage {
    uint8_t deviceId;
    uint8_t deviceType;
    uint8_t messageType;

    uint8_t month;
    uint8_t day;
    uint16_t year;
};

// Device state tracking
struct DeviceState {
    bool available = false;
    bool calibrated = false;
    unsigned long lastSeen = 0;
};