#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define PIN        8       // 1번 신호등 데이터 신호 포트 - 9번 포트 설정.
#define NUMPIXELS 60      // 제어되는 신호등 LED 갯수 (6 X 10 개)
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
SoftwareSerial esp8266(2,3);

void setup() {
  pixels.begin();
  Serial.begin(9600);
  esp8266.begin(9600);

  pixels.clear();

  for(int i=0;i<NUMPIXELS;i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
  pixels.show();
}

void loop() {}
