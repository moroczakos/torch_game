#include "ColorRush.h"

void ColorRush::start(int colors, int duration) {
    _maxColors  = colors;
    _duration   = duration;
    _passes     = 0;
    _holder     = 0;
    _startMillis = millis();
    sendLed(0, true);
}

void ColorRush::stop() {
    for (int i = 0; i < MAX_PLAYERS; i++) sendLed(i, false);
}

void ColorRush::handleSensor(int fromId) {
    if (_holder != fromId) return; // only holder can pass

    sendLed(_holder, false);
    _holder = nextPlayer();
    _passes++;
    sendLed(_holder, true);
    Serial.printf("Torch %d -> %d\n", fromId, _holder);
}

void ColorRush::appendStatus(String& s) {
    int left = max(0, _duration - (int)((millis() - _startMillis) / 1000));
    s += ",\"holder\":" + String(_holder);
    s += ",\"passes\":" + String(_passes);
    s += ",\"timeLeft\":" + String(left);
}

void ColorRush::sendLed(int id, bool on) {
    if (id == 0) { // MY_ID
        if (on) lightRandom();
        else    clearLed();
        return;
    }
    RGBColor rgb = palette(random(_maxColors));
    Packet p{};
    p.type = on ? MSG_LED : MSG_STOP;
    p.target = id;
    p.r = rgb.r; p.g = rgb.g; p.b = rgb.b;
    p.brightness = _ctx.brightness;
    esp_now_send(_ctx.peers[id], (uint8_t*)&p, sizeof(p));
}

int ColorRush::nextPlayer() {
    int id;
    do { id = random(MAX_PLAYERS); } while (!_ctx.active[id]);
    return id;
}