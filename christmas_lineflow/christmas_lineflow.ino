#include <M5Unified.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>

#define NUM_LEDS 300
#define DATA_PIN 26
#define MAX_BRIGHTNESS 32
#define CHANGETIME 1000

const CRGB COL[] = { CRGB::Red, CRGB::LightGreen, CRGB::LightSkyBlue, CRGB::Plum, CRGB::Aquamarine, CRGB::Olive };
const int COLsize = sizeof(COL) / sizeof(COL[0]);

CRGB leds[NUM_LEDS];
CRGB currentColor = CRGB::Black;
CRGB currentColor_blink = CRGB::Black;
char br[NUM_LEDS];
char br_delta[NUM_LEDS];
unsigned long startTime = 0;
int currentindex = 0;
int nextindex = 1;
int pattern = 0;  // 現在の発光パターン

const char* ssid = "shikidalab";
const char* password = "52890239";

WebServer server(80);

void handleRoot() {
  String html = "<html><body><h1>LED Control</h1>";
  html += "<p>This application allows you to control the color and lighting pattern of the LEDs.</p>";
  html += "<h2>How to Use</h2>";
  html += "<h3>Set Color</h3>";
  html += "<p>Enter a hexadecimal color code in the form below to set the LED color.</p>";
  html += "<form action=\"/setcolor\" method=\"GET\">";
  html += "Color: <input type=\"text\" name=\"color\" placeholder=\"e.g., FF0000\"><br>";
  html += "<input type=\"submit\" value=\"Set Color\">";
  html += "</form>";
  html += "<h3>Set Pattern</h3>";
  html += "<p>Select a lighting pattern from the dropdown menu below to set the LED lighting pattern.</p>";
  html += "<form action=\"/setpattern\" method=\"GET\">";
  html += "Pattern: <select name=\"pattern\">";
  html += "<option value=\"0\">Blend</option>";
  html += "<option value=\"1\">Blink</option>";
  html += "<option value=\"2\">Fade</option>";
  html += "</select><br>";
  html += "<input type=\"submit\" value=\"Set Pattern\">";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleSetColor() {
  if (server.hasArg("color")) {
    String color = server.arg("color");
    long int hexColor = strtol(color.c_str(), NULL, 16);
    currentColor = CRGB(hexColor);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = currentColor;
    }
    FastLED.show();
    server.send(200, "text/plain", "Color set to " + color);
  } else {
    server.send(400, "text/plain", "Color not specified");
  }
}

void handleSetPattern() {
  if (server.hasArg("pattern")) {
    pattern = server.arg("pattern").toInt();
    server.send(200, "text/plain", "Pattern set to " + String(pattern));
  } else {
    server.send(400, "text/plain", "Pattern not specified");
  }
}

void setup() {
  auto cfg = M5.config();
  cfg.internal_imu = false;
  cfg.internal_rtc = false;
  cfg.internal_spk = false;
  M5.begin(cfg);
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    br[i] = random(MAX_BRIGHTNESS);
    br_delta[i] = 1;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.println("WiFi connected");
  M5.Lcd.print("IP Address: ");
  M5.Lcd.println(WiFi.localIP());

  String ipAddress = WiFi.localIP().toString();
  M5.Lcd.println("Access the following URIs:");
  M5.Lcd.println("Root: http://" + ipAddress + "/");
  M5.Lcd.println("Set Color: http://" + ipAddress + "/setcolor?color=<hex>");
  M5.Lcd.println("Set Pattern: http://" + ipAddress + "/setpattern?pattern=<0-2>");

  server.on("/", handleRoot);
  server.on("/setcolor", handleSetColor);
  server.on("/setpattern", handleSetPattern);
  server.begin();
}

void blendPattern() {
  if (millis() - startTime > CHANGETIME) {
    startTime = millis();
    currentindex = (currentindex + 1) % COLsize;
    nextindex = (currentindex + 1) % COLsize;
  }
  uint8_t blendAmount = map(millis() - startTime, 0, CHANGETIME, 0, 255);
  CRGB C1 = COL[currentindex];
  CRGB C2 = COL[nextindex];
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = blend(C1, C2, blendAmount);
  }
}

void blinkPattern() {
  if (millis() - startTime > CHANGETIME) {
    startTime = millis();
    currentColor_blink = currentColor_blink == CRGB::Black ? currentColor : CRGB::Black;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = currentColor_blink;
    }
    Serial.println(currentColor.r);
  }
}

void fadePattern() {
  CHSV hsvColor = rgb2hsv_approximate(currentColor);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hsvColor.hue, hsvColor.sat, br[i]);
    br[i] += br_delta[i];
    if (br[i] >= MAX_BRIGHTNESS*3 || br[i] <= 0) br_delta[i] = -br_delta[i];
  }
}

void loop() {
  server.handleClient();

  switch (pattern) {
    case 0:
      blendPattern();
      break;
    case 1:
      blinkPattern();
      break;
    case 2:
      fadePattern();
      break;
  }

  FastLED.show();
}
