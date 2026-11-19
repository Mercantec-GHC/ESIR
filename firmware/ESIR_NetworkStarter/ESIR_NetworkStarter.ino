// ESIR network starter (Arduino MKR WiFi 1010)
// Polls the ESIR website for the next command and reports each step back.
// NOT yet tested on real hardware. Libraries (Library Manager): WiFiNINA, ArduinoHttpClient, ArduinoJson (v7).
// Before uploading: copy arduino_secrets.example.h to arduino_secrets.h and fill it in.

#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

WiFiClient wifi;
HttpClient http(wifi, SECRET_SERVER_HOST, SECRET_SERVER_PORT);

const String API_BASE = String("/api/v1/devices/") + SECRET_DEVICE_ID;

String lastHandledId = "";   // dedup: never run the same command twice
unsigned long lastPoll = 0;
const unsigned long POLL_EVERY_MS = 2000;

void connectWifi() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to Wi-Fi... ");
    WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASS);
    delay(5000);
  }
  Serial.println("connected");
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
