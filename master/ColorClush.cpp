#include "ColorClush.h"

void ColorClush::start(int colors, int duration) {
    _numTeams  = colors;
    _duration  = duration;
    _passes    = 0;
    _startMillis = millis();

    for (int i = 0; i < MAX_PLAYERS; i++) _teamPasses[i] = 0;

    // cap teams to available active buoys
    int activeCnt = 0;
    for (int i = 0; i < MAX_PLAYERS; i++)
        if (_ctx.active[i]) activeCnt++;
    if (_numTeams > activeCnt) _numTeams = activeCnt;

    // assign each team a unique starting buoy
    bool used[MAX_PLAYERS] = {false};
    for (int t = 0; t < _numTeams; t++) {
        int buoy;
        do { buoy = nextPlayer(); } while (used[buoy]);
        used[buoy]      = true;
        _teamHolder[t]  = buoy;
        sendTeamLed(buoy, t, true);
    }
}

void ColorClush::stop() {
    for (int t = 0; t < _numTeams; t++)
        sendTeamLed(_teamHolder[t], t, false);
}

void ColorClush::handleSensor(int fromId) {
    // find which team owns this buoy
    int team = -1;
    for (int t = 0; t < _numTeams; t++) {
        if (_teamHolder[t] == fromId) { team = t; break; }
    }
    if (team == -1) return; // buoy not holding any team's torch

    _teamPasses[team]++;
    _passes++;

    sendTeamLed(fromId, team, false);

    // pick next buoy: active AND not held by another team
    int next;
    int attempts = 0;
    do {
        next = nextPlayer();
        attempts++;
        if (attempts > 100) { next = fromId; break; } // fallback
    } while (next == fromId || isBuoyTaken(next, team));

    _teamHolder[team] = next;
    sendTeamLed(next, team, true);
    Serial.printf("Team %d: buoy %d -> %d\n", team, fromId, next);
}

void ColorClush::appendStatus(String& s) {
    int left = max(0, _duration - (int)((millis() - _startMillis) / 1000));
    s += ",\"passes\":"  + String(_passes);
    s += ",\"timeLeft\":" + String(left);
    s += ",\"teamPasses\":[";
    for (int i = 0; i < _numTeams; i++) {
        s += String(_teamPasses[i]);
        if (i < _numTeams - 1) s += ",";
    }
    s += "]";
}

void ColorClush::tick() {
    for (int t = 0; t < _numTeams; t++) {
        int holder = _teamHolder[t];

        // buoy 0 is the master device — never goes offline
        if (holder == 0) continue;
        if (_ctx.active[holder]) continue;

        // holder dropped — reassign this team to a new active buoy
        Serial.printf("[ColorClush] Team %d holder %d offline, reassigning\n", t, holder);

        sendTeamLed(holder, t, false); // attempt cleanup

        int next;
        int attempts = 0;
        do {
            next = nextPlayer();
            attempts++;
            if (attempts > 100) { next = holder; break; } // fallback
        } while (next == holder || isBuoyTaken(next, t));

        _teamHolder[t] = next;
        sendTeamLed(next, t, true);
        Serial.printf("[ColorClush] Team %d reassigned to buoy %d\n", t, next);
    }
}

// ─── private ───────────────────────────────────────────

void ColorClush::sendTeamLed(int buoy, int team, bool on) {
    RGBColor rgb = palette(team);

    if (buoy == 0) { // MY_ID
        if (on) {
            _ctx.strip->setBrightness(_ctx.brightness);
            _ctx.strip->fill(_ctx.strip->Color(rgb.r, rgb.g, rgb.b));
            _ctx.strip->show();
        } else {
            clearLed();
        }
        return;
    }

    Packet p{};
    p.type       = on ? MSG_LED : MSG_STOP;
    p.target     = buoy;
    p.r          = rgb.r;
    p.g          = rgb.g;
    p.b          = rgb.b;
    p.brightness = _ctx.brightness;
    p.team       = team;

    printPacket(p);
    esp_now_send(_ctx.peers[buoy], (uint8_t*)&p, sizeof(p));
}

int ColorClush::nextPlayer() {
    int id;
    do { id = random(MAX_PLAYERS); } while (!_ctx.active[id]);
    return id;
}

bool ColorClush::isBuoyTaken(int buoy, int excludeTeam) {
    for (int t = 0; t < _numTeams; t++) {
        if (t == excludeTeam) continue;
        if (_teamHolder[t] == buoy) return true;
    }
    return false;
}