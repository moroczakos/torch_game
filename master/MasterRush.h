#pragma once
#include "IGameMode.h"
#include "GameContext.h"

class MasterRush : public IGameMode {
public:
    explicit MasterRush(GameContext& ctx) : _ctx(ctx) {}

    void start(int colors, int duration) override;
    void stop()                          override;
    void handleSensor(int fromId)        override;
    void appendStatus(String& s)         override;

private:
    GameContext&  _ctx;

    int           _masterId    = 0;   // which buoy is the Master
    int           _targetId    = -1;  // currently lit coloured buoy
    int           _confirmedPasses = 0;
    int           _pendingPasses   = 0; // scored but not yet master-confirmed
    int           _duration    = 30;
    int           _maxColors   = 3;
    unsigned long _startMillis = 0;

    // state machine
    enum Phase { PHASE_TARGET, PHASE_MASTER } _phase = PHASE_TARGET;

    void lightTarget();
    void lightMaster();
    void sendLed(int id, uint8_t r, uint8_t g, uint8_t b);
    void sendStop(int id);
};