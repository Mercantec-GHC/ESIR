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
int lastPhase = -1;               // 0 = breathe in, 1 = breathe out
int lastNumber = -1;              // for countdown
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
	lastNumber = -1;
	lightsOff();
}

void updateBreathing() {
	const unsigned long countdownMs = COUNTDOWN_STEPS * COUNTDOWN_STEP_MS;
	const unsigned long cycleMs = BREATHE_IN_MS + BREATHE_HOLD_MS + BREATHE_OUT_MS + BREATHE_HOLD_MS;
	unsigned long elapsed = millis() - breathStartedAt;

	// Part 1: countdown 3, 2, 1
	if (elapsed < countdownMs) {
		int number = COUNTDOWN_STEPS - int(elapsed / COUNTDOWN_STEP_MS);
		if (number != lastNumber) {
			lastNumber = number;
			drawCountdown(number);
		}
		return;
	}

	// Part 2: breathing cycles
	unsigned long sinceStart = elapsed - countdownMs;
	if (sinceStart >= cycleMs * BREATHE_CYCLES) 
	{ 
		react(Face::Happy, 3000, "Well done!");
		return; 
	}

	unsigned long t = sinceStart % cycleMs;               // time inside this cycle
	int phase;                                            // 0 = inhale, 1 = hold, 2 = exhale
	float progress;                                       // 0.0 -> 1.0 inside the phase
	if (t < BREATHE_IN_MS) {
		phase = 0;
		progress = float(t) / BREATHE_IN_MS;
	} else if (t < BREATHE_IN_MS + BREATHE_HOLD_MS) {
		phase = 1;
		progress = float(t - BREATHE_IN_MS) / BREATHE_HOLD_MS;
	} else {
		phase = 2;
		progress = float(t - BREATHE_IN_MS - BREATHE_HOLD_MS) / BREATHE_OUT_MS;
	}

	if (phase != lastPhase) {
		if (lastPhase == -1) drawBreathingFace();         // first time: replace the countdown
		lastPhase = phase;
		const char* names[] = { "Inhale", "", "Exhale", "" };
		drawCaption(names[phase]);
	}

	if (millis() - lastLedUpdate >= 40) {                 // about 25 updates per second
		lastLedUpdate = millis();
		if (phase == 0) {
			lightsBreathBar(1.0f - progress, true);       // teal, going off one by one
			drawBreathMouth(progress);                    // growing
		} else if (phase == 1) {
			lightsBreathBar(0.0f, true);                  // all off
			drawBreathMouth(1.0f);                        // stays full
		} else {
			lightsBreathBar(1.0f - progress, false);      // blue, going off one by one
			drawBreathMouth(1.0f - progress);             // shrinking
		}
	}
}

void handleTouch(const TouchEvent& e) {
	if (e.kind == TouchKind::None) return;
	Serial.print(e.kind == TouchKind::Hold ? "Hold pad " : "Tap pad ");
	Serial.println(e.pad);

	if (e.pad == 4) { goIdle(); return; }      // Stop always wins
	if (mode == Mode::Breathing) return;       // during breathing, ignore every other pad

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
		case 2:   // Cheer me up
			if (e.kind == TouchKind::Tap && calmMoment) react(Face::Love, CHEER_MS, "You got this!");
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
	if (mode == Mode::Breathing) updateBreathing();   
	updateBlink();

	// handleEmotion(netPoll());                          
}