#include <WiFi.h>
#include <esp_now.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include "common.h"

// ================= CONFIG =================
#define MY_ID 0              // Master is player 0

const char* ssid = "ESP32-Game";
const char* pass = "12345678";

// ================ GLOBALS =================
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint8_t peers[MAX_PLAYERS][6] = {
  {0x58, 0x8C, 0x81, 0xA9, 0x19, 0xA0},
  {0x58, 0x8C, 0x81, 0xAB, 0xA2, 0xC0},
  {0x58, 0x8C, 0x81, 0xA8, 0xEB, 0xC4},
  {0x58, 0x8C, 0x81, 0xA6, 0xF8, 0xAC},
  {0x58, 0x8C, 0x81, 0xAA, 0x9C, 0x58}
};

bool    ledOn      = false;
uint8_t brightness = 40;
int     maxColors  = 3;

// =============== GAME STATE ===============
bool running = false;
bool prevDetect = false;
int torchHolder = 0;
int passCount = 0;
int duration = 30;

unsigned long startMillis = 0;
unsigned long lastSeen[MAX_PLAYERS] = {0};
bool active[MAX_PLAYERS] = {true, false, false, false, false};

WebServer server(80);

// ============== HELPER ==================
void printAllArgs(String endpoint) {
  Serial.printf("Endpoint called: %s\n", endpoint.c_str());

  int count = server.args();
  Serial.printf("Got %d args:\n", count);

  for (int i = 0; i < count; i++) {
    Serial.printf(
      "Arg %d -> Name: %s | Value: %s\n",
      i,
      server.argName(i).c_str(),
      server.arg(i).c_str()
    );
  }

  Serial.println();
}

// ============= SEND LED ===================
void sendLed(int id, bool on) {
  if (id == MY_ID) {
    if (on) lightRandom();
    else    clearLed();
    ledOn = on;
    return;
  }

  RGBColor rgb = palette(random(maxColors));
  Packet p{};
  if (on) p.type = MSG_LED;
  else {
    p.type = MSG_STOP;
  }

  p.target = id;
  p.r = rgb.r;
  p.g = rgb.g;
  p.b = rgb.b;
  p.brightness = brightness;

  printPacket(p);

  esp_now_send(peers[id], (uint8_t*)&p, sizeof(p));
}

// =========== NEXT PLAYER ==================
int nextPlayer() {
  int id;
  do {
    id = random(MAX_PLAYERS);
  } while (!active[id]);
  return id;
}

// =========== HANDLE PASS ==================
void handlePass(int from) {
  if (!running) return;

  sendLed(torchHolder, false);

  int next = nextPlayer();
  torchHolder = next;
  passCount++;

  sendLed(next, true);
  Serial.printf("Torch %d -> %d\n", from, next);
}

// ========== ESPNOW RECEIVE ================
void onRecv(const esp_now_recv_info*info,
            const uint8_t*data, int len) {

  Packet p;
  memcpy(&p, data, sizeof(p));

  printPacket(p);

  int sender = -1;
  for (int i = 0; i < MAX_PLAYERS; i++)
    if (!memcmp(info->src_addr, peers[i], 6))
      sender = i;

  if (sender >= 0) lastSeen[sender] = millis();

  if (p.type == MSG_SENSOR && sender >= 0)
    handlePass(sender);
}

// ============== WEB =======================
void sendStatus() {
  int left = running ?
             max(0, duration - (int)((millis() - startMillis) / 1000))
             : 0;

  String s = "{";
  s += "\"running\":" + String(running ? "true" : "false") + ",";
  s += "\"holder\":" + String(torchHolder) + ",";
  s += "\"passes\":" + String(passCount) + ",";
  s += "\"timeLeft\":" + String(left) + ",";
  //s += "\"brightness\":" + String(brightness) + ",";
  s += "\"online\":[";
  s += "true,";
  for (int i = 1; i < MAX_PLAYERS; i++) {
    bool peerOn = (millis() - lastSeen[i] < 3000);
    s += peerOn ? "true" : "false";
    if (i < MAX_PLAYERS - 1)s += ",";
    active[i] = peerOn;
  }
  s += "]}";
  server.send(200, "application/json", s);
}

void startGame() {
  running = true;
  startMillis = millis();
  passCount = 0;
  torchHolder = 0;

  sendLed(0, true);
}

void stopGame() {
  running = false;
  for (int i = 0; i < MAX_PLAYERS; i++)
    sendLed(i, false);
}

void setupWeb() {
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  server.serveStatic("/app.js", SPIFFS, "/app.js");
  server.serveStatic("/modes_data.js", SPIFFS, "/modes_data.js");
  server.serveStatic("/dashboard.jpg", SPIFFS, "/dashboard.jpg");

  server.on("/start", []() {
    printAllArgs("/start");

    if (server.hasArg("colors"))
      maxColors = server.arg("colors").toInt();

    if (server.hasArg("time"))
      duration = server.arg("time").toInt();

    startGame();
    Serial.println("🏁 Game started!");
  });

  server.on("/stop", []() {
    printAllArgs("/stop");

    stopGame();
    Serial.println("🛑 Game stopped.");
  });

  server.on("/restart", []() {
    printAllArgs("/restart");

    stopGame();
    delay(200);
    startGame();
    Serial.println("🔁 Game restarted!");
  });

  server.on("/brightness", HTTP_GET, []() {
    printAllArgs("/brightness");

    if (server.hasArg("value"))
      brightness = server.arg("value").toInt();
  });

  server.on("/test", HTTP_GET, []() {
    printAllArgs("/test");

    if (!server.hasArg("id")) {
      server.send(400, "text/plain", "Missing id");
      return;
    }

    int id = server.arg("id").toInt();

    if (id == MY_ID) {
      for (int i = 0; i < 3; i++) {
        lightRandom();
        delay(500);
        strip.clear();
        strip.show();
        delay(500);
      }
    }

    RGBColor rgb = palette(random(maxColors));

    Packet p{};
    p.type = MSG_TEST;
    p.target = id;
    p.r = rgb.r;
    p.g = rgb.g;
    p.b = rgb.b;
    p.brightness = brightness;

    printPacket(p);

    esp_now_send(peers[id], (uint8_t*)&p, sizeof(p));
  });

  server.on("/status", []() {
    printAllArgs("/status");

    sendStatus();
  });

  server.begin();
}

// ============== SETUP =====================
void setup() {
  initHardware();

  WiFi.mode(WIFI_AP_STA);
  initEspNow(onRecv);
  for (int i = 1; i < MAX_PLAYERS; i++) addPeer(i);

  WiFi.softAP(ssid, pass);
  delay(100);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP()); // Will print 192.168.4.1

  SPIFFS.begin(true);
  setupWeb();
  randomSeed(esp_random());
}

// =============== LOOP =====================
void loop() {
  server.handleClient();

  if (running &&
      millis() - startMillis > duration * 1000)
    stopGame();

  // Master IR
  int v = digitalRead(IR_PIN);
  if (ledOn && v == LOW && !prevDetect) {
    prevDetect = true;
    handlePass(0);
  }
  if (v == HIGH) prevDetect = false;
}
