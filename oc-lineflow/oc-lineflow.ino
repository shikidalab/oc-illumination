#include <FastLED.h>  // FastLED library

#define DATA_PIN 6
#define NUM_LEDS 300
#define BRIGHTNESS 64
#define CHANGETIME 10000

const CRGB COL[] = { CRGB::Red, CRGB::LightGreen, CRGB::LightSkyBlue, CRGB::Plum, CRGB::Aquamarine, CRGB::Olive };
const int COLsize = sizeof(COL) / sizeof(COL[0]);

CRGB leds[NUM_LEDS];
char br[NUM_LEDS];
char br_delta[NUM_LEDS];
unsigned long time1 = 0;
int currentindex = 0;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = COL[0];
    br[i] = random(BRIGHTNESS);
    br_delta[i] = 1;
  }
}

void loop() {
  if (millis() - time1 > CHANGETIME) {
    time1 = millis();
    currentindex = (currentindex + 1) % COLsize;
  }

  CHSV hsvColor = rgb2hsv_approximate(COL[currentindex]);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hsvColor.hue, hsvColor.sat, br[i]);

    br[i] += br_delta[i];
    if (br[i] >= BRIGHTNESS || br[i] <= 0) br_delta[i] = -br_delta[i];
    constrain(br[i], 30, BRIGHTNESS);
  }

  FastLED.show();
  delay(10);
}
