#pragma once
#include <Arduino.h>

enum class Face { Neutral, Happy, Love, Sad, Stressed, Angry, Tired, Bored, Calm, Surprised };

void drawFace(Face face);
void drawNeutralEyes(bool open);
void drawCaption(const char* text);          // one short line under the face, max 13 characters
Face faceForEmotion(const String& emotion);  // "happy" -> Face::Happy, ...