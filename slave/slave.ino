#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>
#include <string.h>


// ============== PROTOCOL ===============
enum MsgType {
  MSG_SENSOR,
  MSG_LED,
  MSG_START,
  MSG_STOP,
  MSG_HEARTBEAT,
  MSG_TEST
};

typedef struct {
  uint8_t type;
  uint8_t from;
  uint8_t target;
  uint8_t r, g, b;
  uint8_t brightness;
} Packet;

// ============== CONFIG =================
#define MAX_PLAYERS 5

#define LED_PIN 2
#define LED_COUNT 1
#define IR_PIN 3

uint8_t MY_ID = 255; // Default to an "invalid" ID until detected

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// ============== PEERS ==================
uint8_t peers[MAX_PLAYERS][6] = {
  {0x58, 0x8C, 0x81, 0xA9, 0x19, 0xA0}, // Master
  {0x58, 0x8C, 0x81, 0xAB, 0xA2, 0xC0},
  {0x58, 0x8C, 0x81, 0xA8, 0xEB, 0xC4},
  {0x58, 0x8C, 0x81, 0xA6, 0xF8, 0xAC},
  {0x58, 0x8C, 0x81, 0xAA, 0x9C, 0x58}
};

int getPeerIndex(const uint8_t* currentMac) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    // memcmp returns 0 if the 6 bytes match perfectly
    if (memcmp(currentMac, peers[i], 6) == 0) {
      return i; // Return the matching index (0, 1, 2...)
    }
  }
  return -1; // Return -1 if the MAC isn't in the list
}

// ============== HELPER ==================

void printPacket(Packet p) {
  Serial.println("Message packet:");
  Serial.print("Type ");

  switch (p.type) {
    case MSG_SENSOR:
      Serial.println("MSG_SENSOR");
      break;
    case MSG_LED:
      Serial.println("MSG_LED");
      break;
    case MSG_START:
      Serial.println("MSG_START");
      break;
    case MSG_STOP:
      Serial.println("MSG_STOP");
      break;
    case MSG_HEARTBEAT:
      Serial.println("MSG_HEARTBEAT");
      break;
    case MSG_TEST:
      Serial.println("MSG_TEST");
      break;
    default:
      Serial.println("UNKNOWN");
      break;
  }

  Serial.print("From ");
  Serial.println(p.from);
  Serial.print("Target ");
  Serial.println(p.target);
  Serial.print("Color ");
  Serial.print(p.r);
  Serial.print(" ");
  Serial.print(p.g);
  Serial.print(" ");
  Serial.println(p.b);
  Serial.print("Brightness ");
  Serial.println(p.brightness);
}

// ============== STATE ==================
bool ledOn = false;
bool prevDetect = false;
unsigned long lastHeartbeat = 0;

// ============== LED ====================
void setLed(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  strip.setBrightness(brightness);
  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.show();
  ledOn = true;
}

void clearLed() {
  strip.clear();
  strip.show();
  ledOn = false;
}

// ============ RECEIVE ===================
void onRecv(const esp_now_recv_info*info,
            const uint8_t*data, int len) {

  Packet p;
  memcpy(&p, data, sizeof(p));

  printPacket(p);

  if (p.target == MY_ID) {
    if (p.type == MSG_LED)
    {
      setLed(p.r, p.g, p.b, p.brightness);
    } else if (p.type == MSG_STOP) {
      clearLed();
    }
  }

  if (p.type == MSG_TEST && p.target == MY_ID) {
    for (int i = 0; i < 3; i++) {
      setLed(p.r, p.g, p.b, p.brightness);
      delay(500);
      clearLed();
      delay(500);
    }
  }
}

// ============== SETUP ===================
void setup() {
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT);

  strip.begin();
  strip.show();

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

  esp_now_init();
  esp_now_register_recv_cb(onRecv);

  // Add master as peer
  esp_now_peer_info_t peer{};
  memcpy(peer.peer_addr, peers[0], 6);
  esp_now_add_peer(&peer);
}

// =============== LOOP ===================
void loop() {

  // ---- IR SENSOR ----
  int v = digitalRead(IR_PIN);

  if (ledOn && v == LOW && !prevDetect) {
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
