#pragma once
#include <Arduino.h>

class IGameMode {
public:
    virtual ~IGameMode() = default;
    virtual void start(int colors, int duration) = 0;
    virtual void stop()                          = 0;
    virtual void handleSensor(int fromId)        = 0;
    virtual void appendStatus(String& s)         = 0;
    virtual void tick() {}
    virtual bool sensorAlwaysActive() const { return false; }
};