#pragma once

#include <Arduino.h>

// Device Types
#define DEVICE_TYPE_HUB 0
#define DEVICE_TYPE_DATE 1
#define DEVICE_TYPE_SCANNER 2
#define DEVICE_TYPE_MODULE 3
#define DEVICE_TYPE_ORIENTATION_SLAVE_SHIELD_MODULE 4
#define DEVICE_TYPE_ORIENTATION_MASTER_SHIELD_MODULE 5

// Message types
#define MSG_TYPE_CONNECT 0
#define MSG_TYPE_DATA 1

// Common header for all messages
struct EspNowHeader {
    uint8_t deviceId;
    uint8_t deviceType;
    uint8_t messageType;
};

// Shield Module message (connection/calibration status)
struct ShieldModuleMessage {
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

// Shield Module state tracking
struct ModuleState {
    bool available = false;
    bool calibrated = false;
};

// Orientation Shield Module submission message
struct OrientationSubmissionMessage {
    uint8_t deviceId;
    uint8_t deviceType;
    uint8_t messageType;

    uint16_t roll;
    uint16_t pitch;
    uint16_t yaw;

    uint8_t round;
    boolean success;
};

struct OrientationProgressMessage {
    uint8_t deviceId;
    uint8_t deviceType;
    uint8_t messageType;

    uint8_t round;
};