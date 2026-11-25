#pragma once
#include <Arduino.h>

constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint8_t DISPLAY_ROTATION = 0;

// Touch pads
constexpr unsigned long HOLD_MS = 1000; 

// How long reactions show (ms)
constexpr unsigned long PET_MS      = 2000;
constexpr unsigned long PET_HOLD_MS = 3000;
constexpr unsigned long CHEER_MS    = 5000;
constexpr unsigned long EMOTION_MS  = 10000;   // a feeling picked on the website

// Blinking (only on the idle face)
constexpr unsigned long BLINK_MIN_MS = 2500;   // shortest wait between blinks
constexpr unsigned long BLINK_MAX_MS = 6000;   // longest wait between blinks
constexpr unsigned long BLINK_MS     = 150;    // how long the eyes stay closed

// Breathing: 6 cycles of 4 s in, 6 s out = 60 s
constexpr unsigned long BREATHE_IN_MS  = 4000;
constexpr unsigned long BREATHE_OUT_MS = 6000;
constexpr int BREATHE_CYCLES = 6;

// LEDs
constexpr uint8_t LED_BRIGHTNESS = 40;         // 0-255; keep it soft for the eyes

// Network
constexpr unsigned long POLL_EVERY_MS  = 2000;
constexpr unsigned long WIFI_RETRY_MS  = 30000;  // try Wi-Fi again every 30 s when offline
constexpr uint32_t HTTP_TIMEOUT_MS     = 2000;