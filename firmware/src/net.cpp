#include "net.h"
#include "config.h"
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

#if __has_include("arduino_secrets.h")
#include "arduino_secrets.h"
#else
#error "Copy include/secrets.example.h to include/secrets.h and fill in your values"
#endif

namespace {
	WiFiClient wifi;
	HttpClient http(wifi, SECRET_SERVER_HOST, SECRET_SERVER_PORT);
	const String API_BASE = String("/api/v1/devices/") + SECRET_DEVICE_ID;

	String lastHandledId;
	unsigned long lastPoll = 0;
	unsigned long lastWifiTry = 0;

	void tryWifi() {
		Serial.println("Connecting to Wi-Fi...");
		WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASS);   // blocks a few seconds
		if (WiFi.status() == WL_CONNECTED) {
			Serial.print("Wi-Fi connected, IP ");
			Serial.println(WiFi.localIP());
		} else {
			Serial.println("No Wi-Fi yet. Touch still works; retrying later.");
		}
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
		http.responseBody();
	}
}

void netBegin() {
	if (WiFi.status() == WL_NO_MODULE) {
		Serial.println("Wi-Fi module not found");
		return;
	}
	http.setHttpResponseTimeout(HTTP_TIMEOUT_MS);
	http.setTimeout(HTTP_TIMEOUT_MS);
	tryWifi();
	lastWifiTry = millis();
}

bool netOnline() {
	return WiFi.status() == WL_CONNECTED;
}

String netPoll() {
	unsigned long now = millis();

	if (!netOnline()) {
		if (now - lastWifiTry >= WIFI_RETRY_MS) {
			lastWifiTry = now;
			tryWifi();
		}
		return "";
	}
	if (now - lastPoll < POLL_EVERY_MS) return "";
	lastPoll = now;

	http.beginRequest();
	http.get(API_BASE + "/commands/next");
	http.sendHeader("X-Device-Key", SECRET_DEVICE_KEY);
	http.endRequest();
	int code = http.responseStatusCode();
	String body = http.responseBody();
	if (code == 401) { Serial.println("Server rejected the device key"); return ""; }
	if (code != 200) return "";                     // 204 = nothing waiting

	JsonDocument doc;
	if (deserializeJson(doc, body)) return "";
	String id = doc["id"].as<String>();
	String emotion = doc["emotion"].as<String>();
	if (id == lastHandledId) return "";

	lastHandledId = id;
	reportStatus(id, "accepted");
	reportStatus(id, "running");
	reportStatus(id, "completed");   // TODO: report "completed" when the reaction really ends
	return emotion;
}