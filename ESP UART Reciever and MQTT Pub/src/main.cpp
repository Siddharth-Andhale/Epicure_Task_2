#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "config.h"

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
uint8_t uartRxBuffer[UART_BUFFER_SIZE];
uint16_t uartRxIndex = 0;
uint32_t messageCounter = 0;
uint32_t lastWifiCheck = 0, lastDebugPrint = 0, lastMqttReconnect = 0;

void debugPrint(const char* msg) {
#if ENABLE_DEBUG
  char buf[256];
  snprintf(buf, sizeof(buf), "%s%s", DEBUG_PREFIX, msg);
  Serial.println(buf);
#endif
}

void getTimestamp(char* buf, size_t len) {
  time_t now = time(nullptr);
  strftime(buf, len, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  
  debugPrint("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_TIMEOUT_MS) {
    delay(100);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    debugPrint("WiFi connected!");
    char msg[80];
    snprintf(msg, sizeof(msg), "IP: %s", WiFi.localIP().toString().c_str());
    debugPrint(msg);
    
    configTime(0, 0, NTP_SERVER);
    time_t now = time(nullptr);
    start = millis();
    while (now < 24 * 3600 && millis() - start < 5000) {
      delay(100);
      now = time(nullptr);
    }
  } else {
    debugPrint("WiFi failed!");
  }
}

void handleUART() {
  while (Serial2.available()) {
    uint8_t data = Serial2.read();
    if (uartRxIndex < UART_BUFFER_SIZE - 1) {
      uartRxBuffer[uartRxIndex++] = data;
    }
    
    if (data == UART_TERMINATOR || uartRxIndex >= UART_BUFFER_SIZE - 1) {
      // Remove \r\n
      while (uartRxIndex > 0 && (uartRxBuffer[uartRxIndex - 1] == '\r' || uartRxBuffer[uartRxIndex - 1] == '\n')) {
        uartRxIndex--;
      }
      uartRxBuffer[uartRxIndex] = '\0';
      
      messageCounter++;
      
      char timestamp[32], jsonPayload[256];
      getTimestamp(timestamp, sizeof(timestamp));
      
      DynamicJsonDocument doc(256);
      doc["device"] = DEVICE_NAME;
      doc["message"] = (char*)uartRxBuffer;
      doc["timestamp"] = timestamp;
      doc["sequence"] = messageCounter;
      serializeJson(doc, jsonPayload, sizeof(jsonPayload));
      
      if (WiFi.status() == WL_CONNECTED && mqttClient.connected()) {
        mqttClient.publish(MQTT_TOPIC, jsonPayload, true);
      }
      
#if ENABLE_DEBUG
      char msg[256];
      snprintf(msg, sizeof(msg), "Message #%lu: %s", messageCounter, jsonPayload);
      debugPrint(msg);
#endif
      
      uartRxIndex = 0;
    }
  }
}

void reconnectMQTT() {
  if (millis() - lastMqttReconnect < MQTT_RECONNECT_DELAY) return;
  lastMqttReconnect = millis();
  
  if (WiFi.status() != WL_CONNECTED) return;
  
  if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
    debugPrint("MQTT connected!");
  } else {
    char msg[80];
    snprintf(msg, sizeof(msg), "MQTT failed, rc=%d", mqttClient.state());
    debugPrint(msg);
  }
}

void setup() {
#if ENABLE_DEBUG
  Serial.begin(DEBUG_BAUD_RATE);
  delay(1000);
  debugPrint("========================================");
  debugPrint("ESP32 STM32 UART to MQTT Bridge v1.0");
  debugPrint("========================================");
#endif

  connectWiFi();
  
  Serial2.begin(UART_BAUD_RATE, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  debugPrint("UART ready on GPIO16(RX)/GPIO17(TX)");
  
  // Configure TLS/SSL certificate
  espClient.setCACert(ROOT_CA);
  debugPrint("TLS certificate configured");
  
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setKeepAlive(MQTT_KEEP_ALIVE);
  debugPrint("Setup complete!");
}

void loop() {
  handleUART();
  
  if (millis() - lastWifiCheck > 5000) {
    lastWifiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      connectWiFi();
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();
  }
  
#if ENABLE_DEBUG
  if (millis() - lastDebugPrint > 30000) {
    lastDebugPrint = millis();
    char msg[100];
    snprintf(msg, sizeof(msg), "Messages: %lu | WiFi: %s | MQTT: %s",
             messageCounter,
             WiFi.status() == WL_CONNECTED ? "OK" : "DOWN",
             mqttClient.connected() ? "OK" : "DOWN");
    debugPrint(msg);
  }
#endif
  
  yield();
}
