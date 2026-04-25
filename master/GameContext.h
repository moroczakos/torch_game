#pragma once
#include "common.h"
#include <Adafruit_NeoPixel.h>

struct GameContext {
    bool*          active;        // active[MAX_PLAYERS]
    unsigned long* lastSeen;      // lastSeen[MAX_PLAYERS]
    uint8_t        brightness;
    uint8_t        (*peers)[6];   // peers[MAX_PLAYERS][6]
    Adafruit_NeoPixel* strip;
};