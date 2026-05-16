#include "ColorConquest.h"

void ColorConquest::start(int colors, int duration) {
    _duration    = duration;
    _startMillis = millis();

    for (int i = 0; i < MAX_PLAYERS; i++) {
        _buoyColor[i]     = -1;
        _buoyClaimedAt[i] = 0;
        sendClaimLed(i, -1); // turn off all buoys
    }

    _teamTime[0] = 0;
    _teamTime[1] = 0;
}

void ColorConquest::stop() {
    unsigned long now = millis();

    // flush remaining time for all currently claimed buoys
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (_buoyColor[i] >= 0 && _buoyClaimedAt[i] > 0) {
            _teamTime[_buoyColor[i]] += now - _buoyClaimedAt[i];
            _buoyClaimedAt[i] = 0;
        }
        sendClaimLed(i, -1); // turn off
    }
}

void ColorConquest::handleSensor(int fromId) {
    unsigned long now = millis();

    // accumulate time for previous owner
    if (_buoyColor[fromId] >= 0 && _buoyClaimedAt[fromId] > 0) {
        _teamTime[_buoyColor[fromId]] += now - _buoyClaimedAt[fromId];
    }

    // unlit -> random team, lit -> toggle to other team
    if (_buoyColor[fromId] < 0) {
        _buoyColor[fromId] = random(2);
    } else {
        _buoyColor[fromId] = 1 - _buoyColor[fromId];
    }

    _buoyClaimedAt[fromId] = now;
    sendClaimLed(fromId, _buoyColor[fromId]);

    Serial.printf("Buoy %d claimed by team %d\n", fromId, _buoyColor[fromId]);
}

void ColorConquest::appendStatus(String& s) {
    int left = max(0, _duration - (int)((millis() - _startMillis) / 1000));
    s += ",\"timeLeft\":" + String(left);

    // buoy ownership
    s += ",\"buoyColors\":[";
    for (int i = 0; i < MAX_PLAYERS; i++) {
        s += String(_buoyColor[i]);
        if (i < MAX_PLAYERS - 1) s += ",";
    }
    s += "]";

    // live team times — include currently running intervals
    unsigned long now = millis();
    unsigned long live[2] = {_teamTime[0], _teamTime[1]};
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (_buoyColor[i] >= 0 && _buoyClaimedAt[i] > 0) {
            live[_buoyColor[i]] += now - _buoyClaimedAt[i];
        }
    }

    s += ",\"teamTimes\":[";
    s += String(live[0]) + "," + String(live[1]);
    s += "]";
}

void ColorConquest::tick() {
    unsigned long now = millis();

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (i == 0) continue;                    // master never goes offline
        if (_buoyColor[i] < 0) continue;         // unclaimed, nothing to do
        if (_ctx.active[i]) continue;            // still online, nothing to do

        // buoy offline while claimed — flush its time and mark unclaimed
        Serial.printf("[Conquest] Buoy %d offline, flushing team %d time\n",
                      i, _buoyColor[i]);

        if (_buoyClaimedAt[i] > 0) {
            _teamTime[_buoyColor[i]] += now - _buoyClaimedAt[i];
            _buoyClaimedAt[i] = 0;
        }

        _buoyColor[i] = -1;  // mark as unclaimed
        // no sendClaimLed — buoy is offline, packet won't arrive
    }
}

// ─── private ───────────────────────────────────────────

void ColorConquest::sendClaimLed(int buoy, int team) {
    // team -1 means turn off
    bool on = (team >= 0);

    if (buoy == 0) { // MY_ID
        if (on) {
            RGBColor rgb = palette(team == 0 ? 0 : 2); // red or blue
            _ctx.strip->setBrightness(_ctx.brightness);
            _ctx.strip->fill(_ctx.strip->Color(rgb.r, rgb.g, rgb.b));
            _ctx.strip->show();
        } else {
            clearLed();
        }
        return;
    }

    Packet p{};
    p.target     = buoy;
    p.brightness = _ctx.brightness;

    if (on) {
        RGBColor rgb = palette(team == 0 ? 0 : 2); // 0=red, 2=blue
        p.type = MSG_LED;
        p.r    = rgb.r;
        p.g    = rgb.g;
        p.b    = rgb.b;
        p.team = team;
    } else {
        p.type = MSG_STOP;
        p.r = p.g = p.b = 0;
    }

    printPacket(p);
    esp_now_send(_ctx.peers[buoy], (uint8_t*)&p, sizeof(p));
}