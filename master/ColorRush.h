#pragma once
#include "IGameMode.h"
#include "GameContext.h"

class ColorRush : public IGameMode {
public:
    explicit ColorRush(GameContext& ctx) : _ctx(ctx) {}

    void start(int colors, int duration) override;
    void stop()                          override;
    void handleSensor(int fromId)        override;
    void appendStatus(String& s)         override;

private:
    GameContext&  _ctx;
    int           _holder   = 0;
    int           _passes   = 0;
    int           _duration = 30;
    int           _maxColors = 3;
    unsigned long _startMillis = 0;

    void sendLed(int id, bool on);
    int  nextPlayer();
};