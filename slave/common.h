#pragma once
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

// ================= CONFIG =================
#define MAX_PLAYERS 5
#define LED_PIN     2
#define LED_COUNT   1
#define IR_PIN      3

// ================ PEERS ===================
extern uint8_t peers[MAX_PLAYERS][6];

// ============== PROTOCOL ==================
enum MsgType : uint8_t {
  MSG_SENSOR    = 0,
  MSG_LED       = 1,
  MSG_START     = 2,
  MSG_STOP      = 3,
  MSG_HEARTBEAT = 4,
  MSG_TEST      = 5
};

typedef struct {
  uint8_t type;
  uint8_t from;
  uint8_t target;
  uint8_t r, g, b;
  uint8_t brightness;
} Packet;

typedef struct {
  uint8_t r, g, b;
} RGBColor;

// ================ SHARED STATE ============
extern Adafruit_NeoPixel strip;
extern bool    ledOn;
extern uint8_t brightness;
extern int     maxColors;

// ============== COLORS ====================
inline RGBColor palette(int i) {
  RGBColor rgb{0, 0, 0};
  switch (i) {
    case 0: rgb.r = 255;                         break;
    case 1: rgb.g = 255;                         break;
    case 2: rgb.b = 255;                         break;
    case 3: rgb.r = 255; rgb.g = 255;            break;
    case 4: rgb.g = 255; rgb.b = 255;            break;
    case 5: rgb.r = 255; rgb.b = 255;            break;
    case 6: rgb.r = 255; rgb.g = 128;            break;
    case 7: rgb.r = 128; rgb.b = 255;            break;
    default: rgb.r = 255; rgb.g = 255; rgb.b = 255; break;
  }
  return rgb;
}

inline void lightRandom() {
  RGBColor rgb = palette(random(maxColors));
  strip.setBrightness(brightness);
  strip.setPixelColor(0, strip.Color(rgb.r, rgb.g, rgb.b));
  strip.show();
}

inline void setLed(uint8_t r, uint8_t g, uint8_t b, uint8_t br) {
  strip.setBrightness(br);
  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.show();
  ledOn = true;
}

inline void clearLed() {
  strip.clear();
  strip.show();
  ledOn = false;
}

// ============== DEBUG =====================
inline void printPacket(Packet p) {
  Serial.println("Message packet:");
  Serial.print("Type ");
  switch (p.type) {
    case MSG_SENSOR:    Serial.println("MSG_SENSOR");    break;
    case MSG_LED:       Serial.println("MSG_LED");       break;
    case MSG_START:     Serial.println("MSG_START");     break;
    case MSG_STOP:      Serial.println("MSG_STOP");      break;
    case MSG_HEARTBEAT: Serial.println("MSG_HEARTBEAT"); break;
    case MSG_TEST:      Serial.println("MSG_TEST");      break;
    default:            Serial.println("UNKNOWN");       break;
  }
  Serial.printf("From: %u\n", p.from);
  Serial.printf("Target: %u\n", p.target);
  Serial.printf("Color: R:%u G:%u B:%u\n", p.r, p.g, p.b);
  Serial.printf("Brightness: %u\n\n", p.brightness);
}

// ============== PEER HELPERS ==============
inline int getPeerIndex(const uint8_t* mac) {
  for (int i = 0; i < MAX_PLAYERS; i++)
    if (memcmp(mac, peers[i], 6) == 0) return i;
  return -1;
}

inline void initEspNow(esp_now_recv_cb_t recvCb) {
  esp_now_init();
  esp_now_register_recv_cb(recvCb);
}

inline void addPeer(int index, wifi_interface_t iface = WIFI_IF_STA, uint8_t channel = 0) {
  esp_now_peer_info_t p{};
  memcpy(p.peer_addr, peers[index], 6);
  p.ifidx   = iface;
  p.channel = channel;
  esp_now_add_peer(&p);
}

// ============= SHARED HW INIT =============
inline void initHardware() {
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT);
  strip.begin();
  strip.show();
}