// ESIR network starter (Arduino MKR WiFi 1010)
// Polls the ESIR website for the next command and reports each step back.
// NOT yet tested on real hardware. Libraries (Library Manager): WiFiNINA, ArduinoHttpClient, ArduinoJson (v7).

#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// ---- Fill these in ----
const char WIFI_SSID[] = "YOUR_WIFI_NAME";
const char WIFI_PASS[] = "YOUR_WIFI_PASSWORD";
const char SERVER_HOST[] = "192.168.1.50";  // LAN IP of the computer running the website
const int  SERVER_PORT = 5080;
// -----------------------

WiFiClient wifi;
HttpClient http(wifi, SERVER_HOST, SERVER_PORT);

String lastHandledId = "";   // dedup: never run the same command twice
unsigned long lastPoll = 0;
const unsigned long POLL_EVERY_MS = 2000;

void connectWifi() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to Wi-Fi... ");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(5000);
  }
  Serial.println("connected");
}

void reportStatus(const String& id, const char* status) {
  String body = "{\"commandId\":\"" + id + "\",\"status\":\"" + status + "\"}";
  http.post("/api/device/status", "application/json", body);
  http.responseStatusCode();
  http.responseBody();  // drain
}

// TODO: draw the face / LEDs here (Oplà display + RGB LEDs).
void showEmotion(const String& emotion) {
  Serial.print("Show emotion: ");
  Serial.println(emotion);
}

void pollOnce() {
  http.get("/api/device/commands/next");
  int code = http.responseStatusCode();
  String body = http.responseBody();
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
