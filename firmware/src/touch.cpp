#include "touch.h"
#include "hardware.h"
#include "config.h"

namespace {
	constexpr int PAD_COUNT = 5;
	bool wasDown[PAD_COUNT] = {};
	bool holdSent[PAD_COUNT] = {};
	unsigned long downSince[PAD_COUNT] = {};

	bool isDown(int pad) {
		switch (pad) {
			case 0: return carrier.Buttons.getTouch(TOUCH0);
			case 1: return carrier.Buttons.getTouch(TOUCH1);
			case 2: return carrier.Buttons.getTouch(TOUCH2);
			case 3: return carrier.Buttons.getTouch(TOUCH3);
			case 4: return carrier.Buttons.getTouch(TOUCH4);
			default: return false;
		}
	}
}

TouchEvent touchPoll() {
	carrier.Buttons.update();
	unsigned long now = millis();

	for (int pad = 0; pad < PAD_COUNT; pad++) {
		bool down = isDown(pad);
		TouchEvent event{TouchKind::None, -1};

		if (down && !wasDown[pad]) {                       // finger just landed
			downSince[pad] = now;
			holdSent[pad] = false;
		}
		if (down && !holdSent[pad] && now - downSince[pad] >= HOLD_MS) {
			holdSent[pad] = true;                          // held long enough
			event = {TouchKind::Hold, pad};
		} else if (!down && wasDown[pad] && !holdSent[pad]) {
			event = {TouchKind::Tap, pad};                 // released before hold time
		}

		wasDown[pad] = down;
		if (event.kind != TouchKind::None) return event;
	}
	return {TouchKind::None, -1};
}