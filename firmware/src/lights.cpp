#include "lights.h"
#include "hardware.h"
#include "config.h"

namespace {
	constexpr int LED_COUNT = 5;

	// Write colours as normal RGB; the carrier's LEDs want green, red, blue
	uint32_t colour(uint8_t r, uint8_t g, uint8_t b) {
		return carrier.leds.Color(g, r, b);
	}

	void fillAll(uint32_t c) {
		for (int i = 0; i < LED_COUNT; i++) carrier.leds.setPixelColor(i, c);
		carrier.leds.show();
	}
}

void lightsBegin() {
	carrier.leds.setBrightness(LED_BRIGHTNESS);   // global cap for all colours
	lightsOff();
}

void lightsOff() {
	fillAll(0);
}

void lightsFor(Face face) {
	switch (face) {
		case Face::Happy:    fillAll(colour(255, 180, 0));  break;  // warm yellow
		case Face::Love:     fillAll(colour(255, 80, 120)); break;  // pink
		case Face::Sad:      fillAll(colour(40, 80, 255));  break;  // soft blue
		case Face::Stressed:
		case Face::Calm:     fillAll(colour(0, 200, 160));  break;  // teal
		case Face::Angry:    fillAll(colour(0, 200, 60));   break;  // calm green
		case Face::Tired:    fillAll(colour(255, 100, 0));  break;  // amber
		case Face::Bored: {                                         // rainbow
			const uint32_t c[LED_COUNT] = {
				colour(255, 0, 0), colour(255, 160, 0), colour(0, 200, 0),
				colour(0, 120, 255), colour(160, 0, 255) };
			for (int i = 0; i < LED_COUNT; i++) carrier.leds.setPixelColor(i, c[i]);
			carrier.leds.show();
			break;
		}
		default: lightsOff(); break;
	}
}

void lightsBreathBar(float remaining, bool inhale) {
	uint32_t on = inhale ? colour(0, 200, 160)      // teal
	                     : colour(60, 100, 255);    // soft blue
	int count = (int)ceilf(remaining * LED_COUNT);  // 1.0 -> 5 LEDs, 0.5 -> 3, 0.0 -> 0
	for (int i = 0; i < LED_COUNT; i++)
		carrier.leds.setPixelColor(i, i < count ? on : 0);
	carrier.leds.show();
}