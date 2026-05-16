#include "MasterRush.h"

void MasterRush::start(int colors, int duration) {
    _maxColors    = colors;
    _duration     = duration;
    _confirmedPasses = 0;
    _pendingPasses   = 0;
    _startMillis  = millis();
    _phase = PHASE_TARGET;
    lightTarget();
}

void MasterRush::stop() {
    for (int i = 0; i < MAX_PLAYERS; i++) sendStop(i);
}

void MasterRush::handleSensor(int fromId) {
    if (_phase == PHASE_TARGET) {
        // only the holder of the lit target buoy scores
        if (fromId != _targetId) return;

        _pendingPasses++;
        Serial.printf("[MasterRush] Target %d touched -> pending=%d, light master %d\n",
                      fromId, _pendingPasses, _masterId);

        sendStop(_targetId);
        _phase = PHASE_MASTER;
        lightMaster();

    } else { // PHASE_MASTER
        // only the master buoy confirms
        if (fromId != _masterId) return;

        _confirmedPasses += _pendingPasses;
        _pendingPasses = 0;
        Serial.printf("[MasterRush] Master %d touched -> confirmed=%d\n",
                      _masterId, _confirmedPasses);

        sendStop(_masterId);
        _phase = PHASE_TARGET;
        lightTarget();
    }
}

void MasterRush::appendStatus(String& s) {
    int left = max(0, _duration - (int)((millis() - _startMillis) / 1000));
    s += ",\"confirmed\":"  + String(_confirmedPasses);
    s += ",\"pending\":"    + String(_pendingPasses);
    s += ",\"target\":"     + String(_targetId);
    s += ",\"master\":"     + String(_masterId);
    s += ",\"phase\":\""    + String(_phase == PHASE_TARGET ? "target" : "master") + "\"";
    s += ",\"timeLeft\":"   + String(left);
}

void MasterRush::tick() {
    if (_phase == PHASE_TARGET) {
        if (_targetId == 0) return;
        if (_ctx.active[_targetId]) return;

        // target buoy dropped — pick a new one
        Serial.printf("[MasterRush] Target %d offline, reassigning\n", _targetId);
        _targetId = -1;
        lightTarget();

    } else { // PHASE_MASTER
        if (_masterId == 0) return;
        if (_ctx.active[_masterId]) return;

        // master buoy dropped — promote a new master, go back to target phase
        Serial.printf("[MasterRush] Master %d offline, promoting new master\n", _masterId);
        _pendingPasses = 0;   // pending pass is lost — master is gone

        // pick new master from remaining active buoys
        for (int i = 1; i < MAX_PLAYERS; i++) {
            if (_ctx.active[i] && i != _targetId) {
                _masterId = i;
                break;
            }
        }

        _phase = PHASE_TARGET;
        lightTarget();
    }
}

// ── helpers ───────────────────────────────────────────────────────────────

void MasterRush::lightTarget() {
    // pick a random active buoy that is NOT the master
    int id;
    int tries = 0;
    do {
        id = random(MAX_PLAYERS);
        tries++;
    } while ((!_ctx.active[id] || id == _masterId) && tries < 20);

    if (tries >= 20) id = 0; // fallback to master device itself

    _targetId = id;

    RGBColor rgb = palette(random(_maxColors));
    Serial.printf("[MasterRush] Light target %d\n", id);

    if (id == 0) {
        // master device: use local LED helper
        lightRandom();
    } else {
        sendLed(id, rgb.r, rgb.g, rgb.b);
    }
}

void MasterRush::lightMaster() {
    Serial.printf("[MasterRush] Light master %d WHITE\n", _masterId);
    if (_masterId == 0) {
        // local white
        uint8_t bri = _ctx.brightness;
        _ctx.strip->fill(_ctx.strip->Color(bri, bri, bri));
        _ctx.strip->show();
    } else {
        sendLed(_masterId, 255, 255, 255); // white
    }
}

void MasterRush::sendLed(int id, uint8_t r, uint8_t g, uint8_t b) {
    Packet p{};
    p.type       = MSG_LED;
    p.target     = id;
    p.r          = r;
    p.g          = g;
    p.b          = b;
    p.brightness = _ctx.brightness;
    esp_now_send(_ctx.peers[id], (uint8_t*)&p, sizeof(p));
}

void MasterRush::sendStop(int id) {
    if (id == 0) {
        clearLed();
        return;
    }
    Packet p{};
    p.type   = MSG_STOP;
    p.target = id;
    esp_now_send(_ctx.peers[id], (uint8_t*)&p, sizeof(p));
}