#include <WiFi.h>
#include <HTTPClient.h>

// ── WiFi Credentials ──
const char* ssid     = "RSN";
const char* password = "Bball@12";

// ── Server URL ──
const char* serverURL = "http://10.27.18.206:5000/update";

// ── Pin Definitions ──
#define FLAME_GF   34   // Ground Floor
#define FLAME_F1   32   // First Floor
#define SMOKE_AO   36   // Analog

#define SEND_INTERVAL 2000
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("\n🔥 PYROPATH SYSTEM STARTING...\n");

  // ⚠️ IMPORTANT FIX
  pinMode(FLAME_GF, INPUT_PULLUP);
  pinMode(FLAME_F1, INPUT_PULLUP);

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected");
  Serial.println(WiFi.localIP());
}

void loop() {

  if (millis() - lastSend < SEND_INTERVAL) return;
  lastSend = millis();

  // ── Read sensors ──
  int gf_raw = digitalRead(FLAME_GF);
  int f1_raw = digitalRead(FLAME_F1);
  int smoke  = analogRead(SMOKE_AO);

  // 🔥 CORRECT LOGIC
  bool gfFire = (gf_raw == LOW);
  bool f1Fire = (f1_raw == LOW);

  // ── Serial Debug ──
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━");
  Serial.print("GF Sensor Raw : "); Serial.println(gf_raw);
  Serial.print("F1 Sensor Raw : "); Serial.println(f1_raw);

  Serial.print("GF Status     : ");
  Serial.println(gfFire ? "🔥 FIRE" : "✅ CLEAR");

  Serial.print("F1 Status     : ");
  Serial.println(f1Fire ? "🔥 FIRE" : "✅ CLEAR");

  Serial.print("Smoke Value   : ");
  Serial.println(smoke);

  // ── Send to server ──
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    // ⚠️ IMPORTANT: send RAW values (0 = fire, 1 = clear)
    String url = String(serverURL) +
                 "?f1=" + String(gf_raw) +
                 "&f2=" + String(f1_raw) +
                 "&smoke=" + String(smoke);

    Serial.print("Sending → ");
    Serial.println(url);

    http.begin(url);
    int code = http.GET();

    if (code > 0) {
      Serial.println("✅ Data sent");
    } else {
      Serial.println("❌ Send failed");
    }

    http.end();
  }

  Serial.println();
}
