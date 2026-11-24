#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

WiFiClient wifi;
HttpClient http(wifi, SECRET_SERVER_HOST, SECRET_SERVER_PORT);

const String API_BASE = String("/api/v1/devices/") + SECRET_DEVICE_ID;

String lastHandledId = "";   
unsigned long lastPoll = 0;
const unsigned long POLL_EVERY_MS = 2000;

void connectWifi() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Wi-Fi module not detected");
    while (true) delay(1000);
  }

  Serial.print("Wi-Fi firmware: ");
  Serial.println(WiFi.firmwareVersion());

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.println(SECRET_WIFI_SSID); // Do not print the password

    int result = WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASS);

    Serial.print("WiFi.begin result: ");
    Serial.println(result);
    Serial.print("Current Wi-Fi status: ");
    Serial.println(WiFi.status());

    if (result == WL_NO_SSID_AVAIL) {
      Serial.println("Network not found");
    } else if (result == WL_CONNECT_FAILED) {
      Serial.println("Connection failed; check password and network settings");
    }

    delay(10000);
  }

  Serial.print("Connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void reportStatus(const String& id, const char* status) {
  String body = "{\"commandId\":\"" + id + "\",\"status\":\"" + status + "\"}";
  http.beginRequest();
  http.post(API_BASE + "/status");
  http.sendHeader("Content-Type", "application/json");
  http.sendHeader("Content-Length", body.length());
  http.sendHeader("X-Device-Key", SECRET_DEVICE_KEY);
  http.beginBody();
  http.print(body);
  http.endRequest();
  http.responseStatusCode();
  http.responseBody();  // drain
}

// TODO: draw the face / LEDs here (Opla display + RGB LEDs).
void showEmotion(const String& emotion) {
  Serial.print("Show emotion: ");
  Serial.println(emotion);
}

void pollOnce() {
  http.beginRequest();
  http.get(API_BASE + "/commands/next");
  http.sendHeader("X-Device-Key", SECRET_DEVICE_KEY);
  http.endRequest();
  int code = http.responseStatusCode();
  String body = http.responseBody();
  if (code == 401) { Serial.println("Server rejected the device key"); return; }
  if (code != 200) return;  // 204 = nothing waiting

  JsonDocument doc;
  if (deserializeJson(doc, body)) return;
  String id = doc["id"].as<String>();
  String emotion = doc["emotion"].as<String>();
  if (id == lastHandledId) return;

  lastHandledId = id;
  reportStatus(id, "accepted");
  reportStatus(id, "running");
  showEmotion(emotion);
  reportStatus(id, "completed");
}

void setup() {
  Serial.begin(115200);
  connectWifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWifi();
  if (millis() - lastPoll >= POLL_EVERY_MS) {
    lastPoll = millis();
    pollOnce();
  }
}
