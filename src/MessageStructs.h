#pragma once

#include <Arduino.h>

// Message types
#define MSG_TYPE_CONNECT 0
#define MSG_TYPE_STATUS 1
#define MSG_TYPE_DISCONNECT 2
#define MSG_TYPE_DATE_UPDATE 3
#define MSG_TYPE_SCANNER_CONNECTED 4

// Common header for all messages
struct EspNowHeader {
    uint8_t id;
    uint8_t messageType;
};

// Device message (connection/calibration status)
struct DeviceMessage {
    uint8_t id;
    uint8_t messageType;
    bool isCalibrated;
};

// Scanner message
struct ScannerMessage {
    uint8_t id;
    uint8_t messageType;
};

// Date module message
struct DateMessage {
    uint8_t id;
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