#pragma once
#include <Arduino.h>

void netBegin();
bool netOnline();
// Checks Wi-Fi and asks the server for a command.
// Returns the emotion ("happy", ...) or "" when there is nothing new.
String netPoll();