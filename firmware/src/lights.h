#pragma once
#include "faces.h"

void lightsBegin();
void lightsFor(Face face);       // a steady colour that fits the face
void lightsBreath(float level);  // 0.0 = dim, 1.0 = bright, teal, for breathing
void lightsOff();