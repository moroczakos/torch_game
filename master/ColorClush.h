#pragma once
#include "IGameMode.h"
#include "GameContext.h"

class ColorClush : public IGameMode {
public:
    explicit ColorClush(GameContext& ctx) : _ctx(ctx) {}

    void start(int colors, int duration) override;
    void stop()                          override;
    void handleSensor(int fromId)        override;
    void appendStatus(String& s)         override;
    void tick()                          override;

private:
    GameContext&  _ctx;
    int  _teamHolder[MAX_PLAYERS] = {0};
    int  _teamPasses[MAX_PLAYERS] = {0};
    int  _numTeams  = 2;
    int  _passes    = 0;
    int  _duration  = 30;
    unsigned long _startMillis = 0;

    void sendTeamLed(int buoy, int team, bool on);
    int  nextPlayer();
    bool isBuoyTaken(int buoy, int excludeTeam);
};