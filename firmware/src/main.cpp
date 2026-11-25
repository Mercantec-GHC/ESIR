#include <Arduino.h>
#include "config.h"
#include "hardware.h"
#include "faces.h"
#include "lights.h"
#include "touch.h"
#include "net.h"

MKRIoTCarrier carrier;

enum class Mode { Idle, Reaction, Breathing, Paused };
Mode mode = Mode::Idle;
unsigned long reactionEndsAt = 0;

// Breathing
unsigned long breathStartedAt = 0;
unsigned long pausedAt = 0;
int lastPhase = -1;               // 0 = breathe in, 1 = breathe out
unsigned long lastLedUpdate = 0;

// Blinking
unsigned long nextBlinkAt = 0;
unsigned long eyesOpenAt = 0;
bool eyesClosed = false;

void scheduleBlink() {
	nextBlinkAt = millis() + random(BLINK_MIN_MS, BLINK_MAX_MS);
}

void updateBlink() {
	if (mode != Mode::Idle) return;
	unsigned long now = millis();
	if (!eyesClosed && long(now - nextBlinkAt) >= 0) {
		drawNeutralEyes(false);
		eyesClosed = true;
		eyesOpenAt = now + BLINK_MS;
	} else if (eyesClosed && long(now - eyesOpenAt) >= 0) {
		drawNeutralEyes(true);
		eyesClosed = false;
		scheduleBlink();
	}
}

void goIdle() {
	mode = Mode::Idle;
	drawFace(Face::Neutral);
	lightsOff();
	eyesClosed = false;
	scheduleBlink();
}

void react(Face face, unsigned long durationMs, const char* caption = nullptr) {
	mode = Mode::Reaction;
	reactionEndsAt = millis() + durationMs;
	drawFace(face);
	if (caption) drawCaption(caption);
	lightsFor(face);
}

void startBreathing() {
	mode = Mode::Breathing;
	breathStartedAt = millis();
	lastPhase = -1;
	drawFace(Face::Calm);
}

void togglePause() {
	if (mode == Mode::Breathing) {
		mode = Mode::Paused;
		pausedAt = millis();
		drawCaption("Paused");
	} else if (mode == Mode::Paused) {
		breathStartedAt += millis() - pausedAt;   // continue where we stopped
		mode = Mode::Breathing;
		lastPhase = -1;
	}
}

void updateBreathing() {
	const unsigned long cycle = BREATHE_IN_MS + BREATHE_OUT_MS;
	unsigned long elapsed = millis() - breathStartedAt;
	if (elapsed >= cycle * BREATHE_CYCLES) { goIdle(); return; }

	unsigned long t = elapsed % cycle;
	int phase = t < BREATHE_IN_MS ? 0 : 1;
	if (phase != lastPhase) {
		lastPhase = phase;
		drawCaption(phase == 0 ? "Breathe in" : "Breathe out");
	}
	if (millis() - lastLedUpdate >= 40) {         // about 25 light updates per second
		lastLedUpdate = millis();
		float level = phase == 0 ? float(t) / BREATHE_IN_MS
		                         : 1.0f - float(t - BREATHE_IN_MS) / BREATHE_OUT_MS;
		lightsBreath(level);
	}
}

void handleTouch(const TouchEvent& e) {
	if (e.kind == TouchKind::None) return;
	Serial.print(e.kind == TouchKind::Hold ? "Hold pad " : "Tap pad ");
	Serial.println(e.pad);

	bool calmMoment = (mode == Mode::Idle || mode == Mode::Reaction);

	switch (e.pad) {
		case 0:   // Pet
			if (!calmMoment) break;
			if (e.kind == TouchKind::Hold) react(Face::Love, PET_HOLD_MS);
			else react(Face::Happy, PET_MS, "Hehe!");
			break;
		case 1:   // Breathe
			if (e.kind == TouchKind::Tap) startBreathing();
			break;
		case 2:   // Pause / Resume
			if (e.kind == TouchKind::Tap) togglePause();
			break;
		case 3:   // Cheer me up
			if (e.kind == TouchKind::Tap && calmMoment) react(Face::Love, CHEER_MS, "You got this!");
			break;
		case 4:   // Stop: always wins
			goIdle();
			break;
	}
}

void handleEmotion(const String& emotion) {
	if (emotion.length() == 0) return;
	Serial.print("Website says: ");
	Serial.println(emotion);
	if (emotion == "stressed") startBreathing();
	else react(faceForEmotion(emotion), EMOTION_MS);
}

void setup() {
	Serial.begin(SERIAL_BAUD);
	while (!Serial && millis() < 3000) {}   // wait up to 3 s for the Serial Monitor

	carrier.noCase();   // turn on when the board sits inside the 3D-printed case
	if (!carrier.begin()) {
		Serial.println("Opla carrier not found. Check that the board is seated properly.");
		while (true) {}
	}
	carrier.display.setRotation(DISPLAY_ROTATION);
	lightsBegin();
	randomSeed(micros());
	goIdle();
	netBegin();
}

void loop() {
	handleTouch(touchPoll());

	if (mode == Mode::Reaction && long(millis() - reactionEndsAt) >= 0) goIdle();
	// if (mode == Mode::Breathing) updateBreathing();   
	updateBlink();

	// handleEmotion(netPoll());                          
}