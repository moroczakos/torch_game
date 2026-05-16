#pragma once
#include "IGameMode.h"
#include "GameContext.h"

class ColorConquest : public IGameMode {
public:
    explicit ColorConquest(GameContext& ctx) : _ctx(ctx) {}

    void start(int colors, int duration) override;
    void stop()                          override;
    void handleSensor(int fromId)        override;
    void appendStatus(String& s)         override;
    void tick()                          override;

private:
    GameContext&  _ctx;
    int           _buoyColor[MAX_PLAYERS];
    unsigned long _buoyClaimedAt[MAX_PLAYERS];
    unsigned long _teamTime[2];
    int           _duration    = 30;
    unsigned long _startMillis = 0;

    void sendClaimLed(int buoy, int team);
};