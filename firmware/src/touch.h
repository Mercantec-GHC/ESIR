#pragma once

enum class TouchKind { None, Tap, Hold };

struct TouchEvent {
	TouchKind kind;
	int pad;          // 0-4, or -1 for none
};

TouchEvent touchPoll();   // call once per loop