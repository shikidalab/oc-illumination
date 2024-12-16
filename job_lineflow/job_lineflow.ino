// 大学説明会用の装飾
// 300LEDのLEDテープを使って、色のグラデーションを行う

#include <FastLED.h>  // FastLED library

#define DATA_PIN 6
#define NUM_LEDS 300
#define MAX_BRIGHTNESS 32
#define CHANGETIME 20000

const CRGB COL[] = { CRGB::Red, CRGB::LightGreen, CRGB::LightSkyBlue, CRGB::Plum, CRGB::Aquamarine, CRGB::Olive };
const int COLsize = sizeof(COL) / sizeof(COL[0]);

CRGB leds[NUM_LEDS];
char br[NUM_LEDS];
char br_delta[NUM_LEDS];
unsigned long startTime = 0;
int currentindex = 0;
int nextindex = 1;                      // 初期化を修正して次の色が異なるように設定

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = COL[0];
    br[i] = random(MAX_BRIGHTNESS);
    br_delta[i] = 1;
  }
}

void loop() {
  // 色の遷移が一定時間ごとに起こる
  if (millis() - startTime > CHANGETIME) {
    startTime = millis();
    currentindex = (currentindex + 1) % COLsize;
    nextindex = (currentindex + 1) % COLsize;  // 次の色へと移行
  }

  // 現在の経過時間からブレンド率を計算0-CHANGETIME -> 0-255
  uint8_t blendAmount = map(millis() - startTime, 0, CHANGETIME, 0, 255);  // 0から255まで変化

  // 現在の色と次の色を取得
  CRGB C1 = COL[currentindex];
  CRGB C2 = COL[nextindex];

  // 全LEDに対してグラデーションを適用
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = blend(C1, C2, blendAmount);
  }

  FastLED.show();
  //delay(10);
}
