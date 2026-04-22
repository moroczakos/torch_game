#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>
#include <string.h>
#include "common.h"

// ================ GLOBALS =================
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint8_t peers[MAX_PLAYERS][6] = {
  {0x58, 0x8C, 0x81, 0xA9, 0x19, 0xA0}, // Master
  {0x58, 0x8C, 0x81, 0xAB, 0xA2, 0xC0},
  {0x58, 0x8C, 0x81, 0xA8, 0xEB, 0xC4},
  {0x58, 0x8C, 0x81, 0xA6, 0xF8, 0xAC},
  {0x58, 0x8C, 0x81, 0xAA, 0x9C, 0x58}
};

bool    ledOn      = false;
uint8_t brightness = 40;
int     maxColors  = 3;

uint8_t MY_ID = 255;

// ============== STATE ==================
bool prevDetect = false;
unsigned long lastHeartbeat = 0;

// ============ RECEIVE ===================
void onRecv(const esp_now_recv_info*info,
            const uint8_t*data, int len) {

  Packet p;
  memcpy(&p, data, sizeof(p));

  printPacket(p);

  if (p.target != MY_ID) return;
  
  switch (p.type) {
    case MSG_LED:
      setLed(p.r, p.g, p.b, p.brightness);
      break;
    case MSG_STOP:
      clearLed();
      break;
    case MSG_TEST:
      for (int i = 0; i < 3; i++) {
        setLed(p.r, p.g, p.b, p.brightness);
        delay(500);
        clearLed();
        delay(500);
      }
      break;
    default:
      break;
  }
}

// ============== SETUP ===================
void setup() {
  initHardware();

  WiFi.mode(WIFI_STA);
  delay(100);

  uint8_t myMac[6];
  WiFi.macAddress(myMac);
  int detectedIndex = getPeerIndex(myMac);

  if (detectedIndex != -1) {
    MY_ID = (uint8_t)detectedIndex; // Overwrite MY_ID with the correct index (0-4)
    Serial.print("Auto-assigned MY_ID: ");
    Serial.println(MY_ID);
  } else {
    Serial.println("Error: This MAC is not in the peers list!");
  }

  initEspNow(onRecv);
  addPeer(0); // Master only
}

// =============== LOOP ===================
void loop() {

  // ---- IR SENSOR ----
  int v = digitalRead(IR_PIN);

  if (v == LOW && !prevDetect) {
    prevDetect = true;

    Packet p{};
    p.type = MSG_SENSOR;
    p.from = MY_ID;

    printPacket(p);

    esp_now_send(peers[0], (uint8_t*)&p, sizeof(p));
  }

  if (v == HIGH) prevDetect = false;

  // ---- HEARTBEAT ----
  if (millis() - lastHeartbeat > 2000) {
    lastHeartbeat = millis();

    Packet hb{};
    hb.type = MSG_HEARTBEAT;
    hb.from = MY_ID;

    printPacket(hb);

    Serial.println(esp_now_send(peers[0], (uint8_t*)&hb, sizeof(hb)) == ESP_OK ? "HB OK" : "HB FAIL");
  }

  delay(30);
}
