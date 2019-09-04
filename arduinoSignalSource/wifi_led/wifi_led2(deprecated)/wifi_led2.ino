#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define PIN_1        8	     // 1번 신호등 데이터 신호 포트 - 9번 포트 설정.
#define PIN_2        9      // 2번 신호등 데이터 신호 포트 - 10번 포트 설정.
#define PIN_3        10      // 3번 신호등 데이터 신호 포트 - 11번 포트 설정.
#define NUMPIXELS 60			// 제어되는 신호등 LED 갯수 (6 X 10 개)
#define DEBUG true				//

Adafruit_NeoPixel pixels_1(NUMPIXELS, PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_2(NUMPIXELS, PIN_2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_3(NUMPIXELS, PIN_3, NEO_GRB + NEO_KHZ800);

SoftwareSerial esp8266(2,3);
#define countPin  13


/* 자동화 모드 관련 필요 제원값 (신호등) */

int COUNT_MODEL_1 = 0;
int COUNT_MODEL_2 = 0;
int COUNT_MODEL_3 = 0;

int TIME_MODEL_1 = 0;
int TIME_MODEL_2 = 0;
int TIME_MODEL_3 = 0;

int TIME_RED = 5000;
int TIME_YELLOW = 700;
int TIME_GREEN = 2500;

int STATUS_MODEL_1 = 1;
int STATUS_MODEL_2 = 2;
int STATUS_MODEL_3 = 3;

int RED = 1;
int YELLOW = 2;
int GREEN = 3;

boolean isAutoMode = true;

void setup()
{  
	beginPixels();
	Serial.begin(9600);
	esp8266.begin(9600);

  String cwjap="AT+CWJAP=\"ad\",\"88888888\"\r\n"; // AT+CWJAP 커멘드 준비.
  sendData("AT+RST\r\n",5000,DEBUG);     // 아두이노 IDE 리셋 초기화 설정
  sendData(cwjap,6000,DEBUG);       // 아두이노 와이파이 모듈 접속
  sendData("AT+CIFSR\r\n",3000,DEBUG);          // 아두이노가 켜지면 ESP8266에 할당된 IP 주소를 받아옴.
  sendData("AT+CIPMUX=1\r\n",3000,DEBUG);       // 멀티 커넥션(다중 접속)을 위한 설정
  sendData("AT+CIPSERVER=1,80\r\n",3000,DEBUG); // 포트 번호 80번으로 설정
  pinMode(countPin,OUTPUT);
}

void loop()
{
  if(isAutoMode == true) {
    if(COUNT_MODEL_1 < TIME_MODEL_1) {
      COUNT_MODEL_1++;
      delay(1);
    }
    else {
      COUNT_MODEL_1 = 0;
      STATUS_MODEL_1 = STATUS_MODEL_1 == GREEN ? YELLOW : (STATUS_MODEL_1 == YELLOW ? RED : GREEN);
      TIME_MODEL_1 = STATUS_MODEL_1 == GREEN ? TIME_GREEN : (STATUS_MODEL_1 == YELLOW ? TIME_YELLOW : TIME_RED);
      turnOn(1,STATUS_MODEL_1);
    }
    if(COUNT_MODEL_2 < TIME_MODEL_2) {
      COUNT_MODEL_2++;
      delay(1);
    }
    else {
      COUNT_MODEL_2 = 0;
      STATUS_MODEL_2 = STATUS_MODEL_2 == GREEN ? YELLOW : (STATUS_MODEL_2 == YELLOW ? RED : GREEN);
      TIME_MODEL_2 = STATUS_MODEL_2 == GREEN ? TIME_GREEN : (STATUS_MODEL_2 == YELLOW ? TIME_YELLOW : TIME_RED);
      turnOn(2,STATUS_MODEL_2);
    }
    if(COUNT_MODEL_3 < TIME_MODEL_3) {
      COUNT_MODEL_3++;
      delay(1);
    }
    else {
      COUNT_MODEL_3 = 0;
      STATUS_MODEL_3 = STATUS_MODEL_3 == GREEN ? YELLOW : (STATUS_MODEL_3 == YELLOW ? RED : GREEN);
      TIME_MODEL_3 = STATUS_MODEL_3 == GREEN ? TIME_GREEN : (STATUS_MODEL_3 == YELLOW ? TIME_YELLOW : TIME_RED);
      turnOn(3, STATUS_MODEL_3);
    }
  }
 if(esp8266.available()) {
    if(esp8266.find("+IPD,"))
    {
      delay(1000); 
      int connectionId = esp8266.read()-48;

      if(esp8266.find("model=")) {
        int model = esp8266.read()-48;
        Serial.println(model);
        if(model > 3) {
          if (model == 4) {
            isAutoMode = false;
            makeX_model1();
            makeX_model2();
            makeX_model3();
          } else if (model == 6) {
            isAutoMode= !isAutoMode;
          } else {
            isAutoMode = true;
          }
        } else {
          esp8266.find("light=");
          int light = esp8266.read()-48;
          turnOn(model, light);
          String closeCommand = "AT+CIPCLOSE=";
          closeCommand+=connectionId;
          closeCommand+="\r\n";
          sendData(closeCommand,1000,DEBUG);
        }
      }
    }
  }
}

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  
  while((time+timeout) > millis())
  {
    while(esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
	
	if(debug) {
		Serial.print(response);
	}

  return response;
}

void turnOn(int model, int light) {
  uint16_t color;

  if(model == 1) {
    turnLight1(light);
  } else if (model == 2) {
    turnLight2(light);
  } else if (model == 3) {
    turnLight3(light);
  }
}

void turnLight1(int light) {
   pixels_1.clear();
   for(int i=0; i<NUMPIXELS; i++) {
    switch(light) {
      case 1 : pixels_1.setPixelColor(i, pixels_1.Color(255, 0, 0)); break;
      case 2 : pixels_1.setPixelColor(i, pixels_1.Color(255, 255, 0)); break;
      case 3 : pixels_1.setPixelColor(i, pixels_1.Color(0, 255, 0)); break;
    }
  }
  pixels_1.show();
}

void turnLight2(int light) {
   pixels_2.clear();
   for(int i=0; i<NUMPIXELS; i++) {
    switch(light) {
      case 1 : pixels_2.setPixelColor(i, pixels_2.Color(255, 0, 0)); digitalWrite(countPin, HIGH); break;
      case 2 : pixels_2.setPixelColor(i, pixels_2.Color(255, 255, 0)); digitalWrite(countPin, LOW); break;
      case 3 : pixels_2.setPixelColor(i, pixels_2.Color(0, 255, 0)); digitalWrite(countPin, LOW);break;
    }
  }
  pixels_2.show();
}

void turnLight3(int light) {
   pixels_3.clear();
   for(int i=0; i<NUMPIXELS; i++) {
    switch(light) {
      case 1 : pixels_3.setPixelColor(i, pixels_3.Color(255, 0, 0)); break;
      case 2 : pixels_3.setPixelColor(i, pixels_3.Color(255, 255, 0)); break;
      case 3 : pixels_3.setPixelColor(i, pixels_3.Color(0, 255, 0)); break;
    }
  }
  pixels_3.show();
}

void beginPixels() {
  pixels_1.begin();
  pixels_2.begin();
  pixels_3.begin();
}

void makeX_model1() {
  pixels_1.clear();
  pixels_1.setPixelColor(0, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(1, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(8, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(9, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(12, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(13, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(16, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(17, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(24, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(25, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(34, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(35, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(42, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(43, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(46, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(47, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(50, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(51, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(58, pixels_1.Color(255, 0, 0));
  pixels_1.setPixelColor(59, pixels_1.Color(255, 0, 0));
  pixels_1.show();
}

void makeX_model2() {
  pixels_2.clear();
  pixels_2.setPixelColor(0, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(1, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(8, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(9, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(12, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(13, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(16, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(17, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(24, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(25, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(34, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(35, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(42, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(43, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(46, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(47, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(50, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(51, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(58, pixels_2.Color(255, 0, 0));
  pixels_2.setPixelColor(59, pixels_2.Color(255, 0, 0));
  pixels_2.show(); 
  digitalWrite(countPin, HIGH);
}

void makeX_model3() {
  pixels_3.clear();
  pixels_3.setPixelColor(0, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(1, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(8, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(9, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(12, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(13, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(16, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(17, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(24, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(25, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(34, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(35, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(42, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(43, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(46, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(47, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(50, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(51, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(58, pixels_3.Color(255, 0, 0));
  pixels_3.setPixelColor(59, pixels_3.Color(255, 0, 0));
  pixels_3.show();
}
