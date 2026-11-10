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
int     maxColors  = 4;

uint8_t MY_ID = 255; // will be auto-assigned in setup() based on MAC address

// ============== STATE ==================
bool prevDetect                 = false;
volatile bool sensorArmed       = false;   // read sensor even when LED is off
unsigned long lastHeartbeat     = 0;
unsigned long lastVoltageCheck  = 0;
float         batteryMv         = 0.0f;

#ifdef USE_ULTRASONIC
  unsigned long lastSonarRead   = 0;
  #define SONAR_INTERVAL        50
  #define SONAR_CONFIRM      2      
  #define SONAR_MIN_CM       2.0f
  #define SONAR_CLEAR_CM     (TOUCH_DIST_CM + 4)
  int  sonarHits   = 0;
  bool sonarClear  = false;
  bool lastActive  = false;
#endif

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
  case MSG_LED_OFF:
    clearLed();
    sensorArmed = true;
    break;
  case MSG_STOP:
    clearLed();
    sensorArmed = false;
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

// =========== SENSOR FIRE ==================
void fireSensor() {
  Packet p{};
  p.type = MSG_SENSOR;
  p.from = MY_ID;
  printPacket(p);
  esp_now_send(peers[0], (uint8_t*)&p, sizeof(p));
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

  batteryMv = readBatteryVoltage();

  initEspNow(onRecv);
  addPeer(0); // Master only
}

// =============== LOOP ===================
void loop() {
  unsigned long now = millis();

  // ---- SENSOR ----
  #ifdef USE_ULTRASONIC
    // ---- ULTRASONIC SENSOR ----
    if (now - lastSonarRead >= SONAR_INTERVAL) {
      lastSonarRead = now;

      bool sonarActive = ledOn || sensorArmed;

      if (sonarActive && !lastActive) {
        sonarHits = 0; sonarClear = false; prevDetect = false;
      }
      lastActive = sonarActive;

      if (sonarActive) {
        float d = readDistanceCm();

        if (d > SONAR_CLEAR_CM) {
          sonarClear = true;
          sonarHits  = 0;
          prevDetect = false;
        } else if (sonarClear && d >= SONAR_MIN_CM && d <= TOUCH_DIST_CM) {
          if (++sonarHits >= SONAR_CONFIRM && !prevDetect) {
            prevDetect = true;
            Serial.printf("[SONAR] Triggered at %.1f cm\n", d);
            fireSensor();
          }
        }
      } else {
        sonarHits = 0; sonarClear = false; prevDetect = false;
      }
    }
  #else
    // ---- IR SENSOR ----
    if (ledOn || sensorArmed) {  
      int v = digitalRead(IR_PIN);
      if (v == LOW && !prevDetect) {
        prevDetect = true;
        fireSensor();
      }
      if (v == HIGH) prevDetect = false;
      } else {
      prevDetect = false;
    }
  #endif

  // ── Voltage check (non-blocking, every 10s) ─────────────────
  if (now - lastVoltageCheck > VOLTAGE_INTERVAL) {
    lastVoltageCheck = now;
    batteryMv   = readBatteryVoltage();

    Packet v{};
    v.type   = MSG_VOLTAGE;
    v.from   = MY_ID;
    v.voltMv = (uint16_t)constrain(batteryMv, 0, 65535);

    Serial.printf("[BATT]: %.0f mV\n", batteryMv);
    esp_now_send(peers[0], (uint8_t*)&v, sizeof(v));
  }

  // ---- HEARTBEAT ----
  if (now - lastHeartbeat > 2000) {
    lastHeartbeat = now;

    Packet hb{};
    hb.type = MSG_HEARTBEAT;
    hb.from = MY_ID;

    printPacket(hb);

    Serial.println(esp_now_send(peers[0], (uint8_t*)&hb, sizeof(hb)) == ESP_OK ? "HB OK" : "HB FAIL");
  }

  delay(30);
}
