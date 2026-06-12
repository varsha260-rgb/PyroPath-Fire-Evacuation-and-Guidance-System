#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>   // 🔥 IMPORTANT

// 🔐 WiFi
const char* ssid = "RSN";
const char* password = "Bball@12";

// 🌐 Server
String serverName = "http://10.27.18.206:5000/data";

// LED Pins (GREEN + RED)
int p1_green = 26, p1_red = 25; // GF Apartment
int p2_green = 22, p2_red = 23; // Entrance
int p3_green = 19, p3_red = 21; // Stair
int p4_green = 5,  p4_red = 18; // F1 Apartment
int p5_green = 2,  p5_red = 4;  // Corridor

void setup() {
  Serial.begin(115200);

  pinMode(p1_green, OUTPUT); pinMode(p1_red, OUTPUT);
  pinMode(p2_green, OUTPUT); pinMode(p2_red, OUTPUT);
  pinMode(p3_green, OUTPUT); pinMode(p3_red, OUTPUT);
  pinMode(p4_green, OUTPUT); pinMode(p4_red, OUTPUT);
  pinMode(p5_green, OUTPUT); pinMode(p5_red, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected");
}

// 🔧 LED control
void controlPath(bool safe, int greenPin, int redPin) {
  digitalWrite(greenPin, safe ? HIGH : LOW);
  digitalWrite(redPin,   safe ? LOW  : HIGH);
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(serverName);

    int code = http.GET();

    if (code > 0) {

      String payload = http.getString();
      Serial.println(payload);

      // Parse JSON
      StaticJsonDocument<200> doc;
      deserializeJson(doc, payload);

      int gf = doc["f1"];  // Ground Floor
      int f1 = doc["f2"];  // First Floor

      // ---------- LOGIC ----------

      // DEFAULT: ALL SAFE
      bool p1 = true, p2 = true, p3 = true, p4 = true, p5 = true;

      if (gf == 0) {
        Serial.println("🔥 FIRE ON GROUND FLOOR");

        p1 = false; // GF Apt
        p2 = false; // Entrance

        p3 = true;  // Stair
        p5 = true;  // Corridor
        p4 = true;  // F1 Apt
      }

      else if (f1 == 0) {
        Serial.println("🔥 FIRE ON FIRST FLOOR");

        p4 = false; // F1 Apt
        p5 = false; // Corridor

        p3 = true;  // Stair
        p2 = true;  // Entrance
        p1 = true;  // GF Apt
      }

      else {
        Serial.println("✅ NORMAL CONDITION");
      }

      // ---------- APPLY TO LEDs ----------
      controlPath(p1, p1_green, p1_red);
      controlPath(p2, p2_green, p2_red);
      controlPath(p3, p3_green, p3_red);
      controlPath(p4, p4_green, p4_red);
      controlPath(p5, p5_green, p5_red);
    }

    http.end();
  }

  delay(2000);
}
