#include "EspNowHelper.h"

EspNowHelper* EspNowHelper::instance = nullptr;

EspNowHelper::EspNowHelper() {
  instance = this;
}

void EspNowHelper::begin(int id) {
  deviceId = id;

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());

  Serial.printf("Device ID: %d\n", deviceId);

  Serial.println("Initializing ESP-NOW...");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(handleESPNowDataReceived);
  esp_now_register_send_cb(handleESPNowDataSent);
}

void EspNowHelper::addPeer(uint8_t* macAddress) {
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, macAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  } else {
    Serial.println("Peer added successfully");
  }
}

void EspNowHelper::registerModuleMessageHandler(void (*handler)(const ShieldModuleMessage&)) {
  moduleMessageHandler = handler;
}
void EspNowHelper::registerDateMessageHandler(void (*handler)(const DateMessage&)) {
  dateMessageHandler = handler;
}
void EspNowHelper::registerScannerMessageHandler(void (*handler)(const ScannerMessage&)) {
  scannerMessageHandler = handler;
}
void EspNowHelper::registerOrientationMessageHandler(
    void (*handler)(const OrientationSubmissionMessage&)) {
  orientationMessageHandler = handler;
}

void EspNowHelper::callModuleMessageHandler(const ShieldModuleMessage& message) {
  if (moduleMessageHandler != nullptr) {
    moduleMessageHandler(message);
  }
}
void EspNowHelper::callDateMessageHandler(const DateMessage& message) {
  if (dateMessageHandler != nullptr) {
    dateMessageHandler(message);
  }
}
void EspNowHelper::callScannerMessageHandler(const ScannerMessage& message) {
  if (scannerMessageHandler != nullptr) {
    scannerMessageHandler(message);
  }
}
void EspNowHelper::callOrientationMessageHandler(const OrientationSubmissionMessage& message) {
  if (orientationMessageHandler != nullptr) {
    orientationMessageHandler(message);
  }
}

void EspNowHelper::handleESPNowDataReceived(const uint8_t* mac, const uint8_t* incomingDataRaw,
                                            int len) {
  // Small delay to prevent serial corruption when called from WiFi task
  delayMicroseconds(100);

  // Read message header to determine type
  EspNowHeader header;
  memcpy(&header, incomingDataRaw, sizeof(EspNowHeader));

  Serial.println("--- ESP-NOW Data Received ---");
  Serial.print("MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5)
      Serial.print(":");
  }
  Serial.println();

  Serial.printf("ESP ID: %d\n", header.deviceId);
  Serial.printf("Device Type: %d\n", header.deviceType);
  Serial.printf("Message Type: %d\n", header.messageType);

  if (instance == nullptr) {
    Serial.println("ERROR: No EspNowHelper instance available");
    return;
  }

  if (header.deviceType == DEVICE_TYPE_MODULE) {
    ShieldModuleMessage shieldModuleMsg;
    memcpy(&shieldModuleMsg, incomingDataRaw, sizeof(ShieldModuleMessage));
    instance->callModuleMessageHandler(shieldModuleMsg);
  } else if (header.deviceType == DEVICE_TYPE_DATE) {
    DateMessage dateMsg;
    memcpy(&dateMsg, incomingDataRaw, sizeof(DateMessage));
    instance->callDateMessageHandler(dateMsg);
  } else if (header.deviceType == DEVICE_TYPE_SCANNER) {
    ScannerMessage scannerMsg;
    memcpy(&scannerMsg, incomingDataRaw, sizeof(ScannerMessage));
    instance->callScannerMessageHandler(scannerMsg);
  } else if (header.deviceType == DEVICE_TYPE_ORIENTATION_SLAVE_SHIELD_MODULE) {
    OrientationSubmissionMessage orientationMsg;
    memcpy(&orientationMsg, incomingDataRaw, sizeof(OrientationSubmissionMessage));
    instance->callOrientationMessageHandler(orientationMsg);
  } else {
    Serial.println("Unknown message type received.");
  }

  Serial.println("-----------------------------");
}

void EspNowHelper::handleESPNowDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("  ✓ Delivery confirmed");
  } else {
    Serial.println("  ✗ Delivery failed");
  }
  Serial.println("------------------------");
}

void EspNowHelper::sendMessage(uint8_t* targetAddress, EspNowHeader& message, size_t messageSize) {
  Serial.println("  → Preparing message:");
  Serial.print("    Target MAC: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", targetAddress[i]);
    if (i < 5)
      Serial.print(":");
  }
  Serial.println();
  Serial.print("      Device ID: ");
  Serial.println(message.deviceId);
  Serial.print("      Device Type: ");
  Serial.println(message.deviceType);
  Serial.print("      Message Type: ");
  Serial.println(message.messageType);

  esp_err_t result = esp_now_send(targetAddress, (uint8_t*)&message, messageSize);

  if (result == ESP_OK) {
    Serial.println("  → Message queued for sending");
  } else {
    Serial.print("  ✗ Error queueing message: ");
    switch (result) {
      case ESP_ERR_ESPNOW_NOT_INIT:
        Serial.println("ESP_ERR_ESPNOW_NOT_INIT: ESP-NOW not initialized");
        break;
      case ESP_ERR_ESPNOW_ARG:
        Serial.println("ESP_ERR_ESPNOW_ARG: Invalid argument");
        break;
      case ESP_ERR_ESPNOW_INTERNAL:
        Serial.println("ESP_ERR_ESPNOW_INTERNAL: Internal error");
        break;
      case ESP_ERR_ESPNOW_NO_MEM:
        Serial.println("ESP_ERR_ESPNOW_NO_MEM: Out of memory");
        break;
      case ESP_ERR_ESPNOW_NOT_FOUND:
        Serial.println("ESP_ERR_ESPNOW_NOT_FOUND: Peer not found");
        break;
      case ESP_ERR_ESPNOW_IF:
        Serial.println("ESP_ERR_ESPNOW_IF: WiFi interface error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
}

void EspNowHelper::sendDateConnected(uint8_t* targetAddress) {
  Serial.println("Sending Date Connected Message...");

  DateMessage message;
  message.deviceId = deviceId;
  message.deviceType = DEVICE_TYPE_DATE;
  message.messageType = MSG_TYPE_CONNECT;
  message.month = 0;
  message.day = 0;
  message.year = 0;

  sendMessage(targetAddress, (EspNowHeader&)message, sizeof(message));
}

void EspNowHelper::sendScannerConnected(uint8_t* targetAddress) {
  Serial.println("Sending Scanner Connected Message...");

  ScannerMessage message;
  message.deviceId = deviceId;
  message.deviceType = DEVICE_TYPE_SCANNER;
  message.messageType = MSG_TYPE_CONNECT;

  sendMessage(targetAddress, (EspNowHeader&)message, sizeof(message));
}

void EspNowHelper::sendModuleConnected(uint8_t* targetAddress) {
  Serial.println("Sending Shield Module Connected Message");

  ShieldModuleMessage message;
  message.deviceId = deviceId;
  message.deviceType = DEVICE_TYPE_MODULE;
  message.messageType = MSG_TYPE_CONNECT;
  message.isCalibrated = false;

  sendMessage(targetAddress, (EspNowHeader&)message, sizeof(message));
}

void EspNowHelper::sendDateUpdated(uint8_t* targetAddress, uint8_t month, uint8_t day,
                                   uint16_t year) {
  Serial.println("Sending Date Updated Message...");

  DateMessage message;
  message.deviceId = deviceId;
  message.deviceType = DEVICE_TYPE_DATE;
  message.messageType = MSG_TYPE_DATA;
  message.month = month;
  message.day = day;
  message.year = year;

  Serial.print("  → Date: ");
  Serial.print(month);
  Serial.print("/");
  Serial.print(day);
  Serial.print("/");
  Serial.println(year);
  sendMessage(targetAddress, (EspNowHeader&)message, sizeof(message));
}

void EspNowHelper::sendModuleUpdated(uint8_t* targetAddress, bool isCalibrated) {
  Serial.println("Sending Shield Module Updated Message...");

  ShieldModuleMessage message;
  message.deviceId = deviceId;
  message.deviceType = DEVICE_TYPE_MODULE;
  message.messageType = MSG_TYPE_DATA;
  message.isCalibrated = isCalibrated;

  Serial.print("  → isCalibrated: ");
  Serial.println(isCalibrated ? "true" : "false");

  sendMessage(targetAddress, (EspNowHeader&)message, sizeof(message));
}

void EspNowHelper::sendOrientationUpdated(uint8_t* targetAddress, uint16_t roll, uint16_t pitch,
                                          uint16_t yaw, uint8_t round, boolean success) {
  Serial.println("Sending Orientation Updated Message...");

  OrientationSubmissionMessage message;
  message.deviceId = deviceId;
  message.deviceType = DEVICE_TYPE_ORIENTATION_SLAVE_SHIELD_MODULE;
  message.messageType = MSG_TYPE_DATA;
  message.roll = roll;
  message.pitch = pitch;
  message.yaw = yaw;
  message.round = round;
  message.success = success;

  Serial.print("  → Roll: ");
  Serial.println(roll);
  Serial.print("  → Pitch: ");
  Serial.println(pitch);
  Serial.print("  → Yaw: ");
  Serial.println(yaw);
  Serial.print("  → Round: ");
  Serial.println(round);
  Serial.print("  → Success: ");
  Serial.println(success ? "true" : "false");

  sendMessage(targetAddress, (EspNowHeader&)message, sizeof(message));
}