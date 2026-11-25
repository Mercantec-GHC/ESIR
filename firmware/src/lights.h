#pragma once
#include "faces.h"

void lightsBegin();
void lightsFor(Face face);       // a steady colour that fits the face 
void lightsOff();
void lightsBreathBar(float remaining, bool inhale);