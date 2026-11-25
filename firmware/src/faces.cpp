#include "faces.h"
#include "hardware.h"

namespace {
	// Convert normal RGB to the display's 16-bit colour format
	constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}

	// Colours from the website: black face, mint features, pink hearts
	constexpr uint16_t BG    = rgb565(0, 0, 0);
	constexpr uint16_t MINT  = rgb565(126, 217, 200);
	constexpr uint16_t PINK  = rgb565(240, 150, 170);
	constexpr uint16_t WHITE = rgb565(255, 255, 255);

	constexpr int CX = 120;        // centre of the display
	constexpr int EYE_Y = 100;
	constexpr int EYE_DX = 45;     // distance from the centre to each eye
	constexpr int MOUTH_Y = 165;
	constexpr int LEFT_EYE_X = CX - EYE_DX;
	constexpr int RIGHT_EYE_X = CX + EYE_DX;

	auto& d = carrier.display;

	// --- eyes ---
	void pillEye(int x, int height) {            // normal open eye
		d.fillRoundRect(x - 12, EYE_Y - height / 2, 24, height, 12, MINT);
	}
	void happyEye(int x) {                        // closed "^" eye
		d.fillCircle(x, EYE_Y, 22, MINT);
		d.fillCircle(x, EYE_Y + 10, 22, BG);
	}
	void relaxedEye(int x) {                      // closed, relaxed eye (upside-down arc)
		d.fillCircle(x, EYE_Y - 10, 18, MINT);
		d.fillCircle(x, EYE_Y - 18, 18, BG);
	}
	void roundEye(int x, int r) {
		d.fillCircle(x, EYE_Y, r, MINT);
	}
	void heart(int x, int y, int r, uint16_t colour) {
		d.fillCircle(x - r, y, r, colour);
		d.fillCircle(x + r, y, r, colour);
		d.fillTriangle(x - 2 * r, y + r / 3, x + 2 * r, y + r / 3, x, y + 2 * r + r / 2, colour);
	}

	// --- mouths ---
	void smile() {
		d.fillCircle(CX, MOUTH_Y - 15, 30, MINT);
		d.fillCircle(CX, MOUTH_Y - 25, 30, BG);
	}
	void frown() {
		d.fillCircle(CX, MOUTH_Y + 25, 30, MINT);
		d.fillCircle(CX, MOUTH_Y + 35, 30, BG);
	}
	void flatMouth(int width) {
		d.fillRoundRect(CX - width / 2, MOUTH_Y - 4, width, 8, 4, MINT);
	}
	void oMouth() {
		d.fillCircle(CX, MOUTH_Y, 14, MINT);
		d.fillCircle(CX, MOUTH_Y, 8, BG);
	}
	void cheeks() {
		d.fillCircle(CX - 70, 135, 10, PINK);
		d.fillCircle(CX + 70, 135, 10, PINK);
	}
}

void drawFace(Face face) {
	d.fillScreen(BG);

	switch (face) {
		case Face::Neutral:
			smile();
			pillEye(LEFT_EYE_X, 48); pillEye(RIGHT_EYE_X, 48);
			break;
		case Face::Happy:
			smile(); cheeks();
			happyEye(LEFT_EYE_X); happyEye(RIGHT_EYE_X);
			break;
		case Face::Love:
			smile(); cheeks();
			heart(LEFT_EYE_X, EYE_Y - 8, 12, PINK); heart(RIGHT_EYE_X, EYE_Y - 8, 12, PINK);
			break;
		case Face::Sad:
			frown();
			pillEye(LEFT_EYE_X, 40); pillEye(RIGHT_EYE_X, 40);
			// cut the outer top corners so the eyes droop
			d.fillTriangle(LEFT_EYE_X - 14, EYE_Y - 22, LEFT_EYE_X + 14, EYE_Y - 22, LEFT_EYE_X - 14, EYE_Y, BG);
			d.fillTriangle(RIGHT_EYE_X - 14, EYE_Y - 22, RIGHT_EYE_X + 14, EYE_Y - 22, RIGHT_EYE_X + 14, EYE_Y, BG);
			break;
		case Face::Angry:
			flatMouth(36);
			pillEye(LEFT_EYE_X, 40); pillEye(RIGHT_EYE_X, 40);
			// cut the inner top corners so the eyes slant
			d.fillTriangle(LEFT_EYE_X - 14, EYE_Y - 22, LEFT_EYE_X + 14, EYE_Y - 22, LEFT_EYE_X + 14, EYE_Y, BG);
			d.fillTriangle(RIGHT_EYE_X - 14, EYE_Y - 22, RIGHT_EYE_X + 14, EYE_Y - 22, RIGHT_EYE_X - 14, EYE_Y, BG);
			break;
		case Face::Stressed:
			oMouth();
			roundEye(LEFT_EYE_X, 16); roundEye(RIGHT_EYE_X, 16);
			break;
		case Face::Tired:
			flatMouth(30);
			d.fillRoundRect(LEFT_EYE_X - 16, EYE_Y - 4, 32, 8, 4, MINT);
			d.fillRoundRect(RIGHT_EYE_X - 16, EYE_Y - 4, 32, 8, 4, MINT);
			d.setTextColor(MINT); d.setTextSize(3);
			d.setCursor(RIGHT_EYE_X + 20, EYE_Y - 50); d.print("z");
			break;
		case Face::Bored:
			flatMouth(30);
			pillEye(LEFT_EYE_X + 12, 40); pillEye(RIGHT_EYE_X + 12, 40);   // looking to the side
			break;
		case Face::Calm:
			smile();
			relaxedEye(LEFT_EYE_X); relaxedEye(RIGHT_EYE_X);
			break;
		case Face::Surprised:
			oMouth();
			roundEye(LEFT_EYE_X, 20); roundEye(RIGHT_EYE_X, 20);
			break;
	}
}

void drawCaption(const char* text) {
	d.fillRect(40, 190, 160, 24, BG);              // clear the old caption
	d.setTextColor(WHITE);
	d.setTextSize(2);                              // 12 px per character
	int width = strlen(text) * 12;
	d.setCursor(CX - width / 2, 195);
	d.print(text);
}

Face faceForEmotion(const String& emotion) {
	if (emotion == "happy")    return Face::Happy;
	if (emotion == "sad")      return Face::Sad;
	if (emotion == "stressed") return Face::Stressed;
	if (emotion == "angry")    return Face::Angry;
	if (emotion == "tired")    return Face::Tired;
	if (emotion == "bored")    return Face::Bored;
	return Face::Neutral;
}
void drawNeutralEyes(bool open) {
	// Clear only the eye areas, so the mouth doesn't flicker
	d.fillRect(LEFT_EYE_X - 20, EYE_Y - 30, 40, 60, BG);
	d.fillRect(RIGHT_EYE_X - 20, EYE_Y - 30, 40, 60, BG);
	if (open) {
		pillEye(LEFT_EYE_X, 48); pillEye(RIGHT_EYE_X, 48);
	} else {
		d.fillRoundRect(LEFT_EYE_X - 16, EYE_Y - 4, 32, 8, 4, MINT);
		d.fillRoundRect(RIGHT_EYE_X - 16, EYE_Y - 4, 32, 8, 4, MINT);
	}
}