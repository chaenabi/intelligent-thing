#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h> 
/* 
 #include <Adafruit_NeoPixel.h>
   NeoPixel은 adafruit에서 구할 수 있는 LED 제품으로 단일LED, Strip 타입, Ring 타입,
   Stick 타입, 쉴드 타입등 다양한 모양의 제품이 존재합니다.
   모든 제품들은 adafruit 에서 제공하는 Adafruit_NeoPixel 라이브러리를 사용하여 제어가
   가능합니다.
   2019년 경진대회 출품작에서는 Strip 타입을 사용하였습니다.
 
 #include <SoftwareSerial.h> 
   아두이노 보드에는 RX(0), TX(1) 포트가 있는데, 이것을 시리얼 포트라고 합니다.
   0번 포트와 1번 포트는 아두이노와 컴퓨터를 연결해주는 통신통로이며, 송신용/수신용으로 각각 하나씩만
   존재하기 때문에 1:1 통신, 즉 하나의 통신만이 가능합니다.
   따라서 필연적으로 0번과 1번을 이미 특정 용도로 사용하고 있을시
   아두이노 스케치에서 업로딩을 하지 못하는 등의 한계점이 존재하므로
   이러한 문제점(Serial 포트가 모자랄 때)을 해결하기 위해
   아두이노에서는 기본 라이브러리를 하나 마련해두었는데,
   그것이 바로 SoftwareSerial 라이브러리입니다.

   기본적으로 입출력 용도로 사용하도록 마련된 RX, TX를 하드웨어 시리얼이라고 부르는 것과 
   구분짓기 위하여 SoftwareSerial이라 칭하고 있습니다.
   2019년 경진대회 출품작에서는 SoftwareSerial 라이브러리를 사용하여 2, 3번 포트를 이용하였습니다. 
   SoftwareSerial esp8266(2,3);
   와 같이 초기화하여 사용할 수 있습니다.
*/


#define PIN_1        8	     // 1번 신호등 데이터 신호 포트 - 9번 포트 설정.
#define PIN_2        9       // 2번 신호등 데이터 신호 포트 - 10번 포트 설정.
#define PIN_3        10      // 3번 신호등 데이터 신호 포트 - 11번 포트 설정.
#define NUMPIXELS 60			// 제어되는 신호등 LED 갯수 (6 X 10 개)
#define DEBUG true				// 디버그를 위한 설정. true 값일시 특정 코드를 시리얼모니터에 출력한다.

// 한 개의 신호등에 3개의 Strip 타입 네오픽셀을 한 세트로 사용하였으므로 
// 각각의 Strip 타입 네오픽셀에 대하여 모두 초기화하는 코드입니다.
Adafruit_NeoPixel pixels_1(NUMPIXELS, PIN_1, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel pixels_2(NUMPIXELS, PIN_2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_3(NUMPIXELS, PIN_3, NEO_GRB + NEO_KHZ800);

SoftwareSerial esp8266(2,3); // 2, 3번 포트를 serial 통신을 위한 포트로 사용함을 설정하는 코드.
#define countPin  13 // 13번 핀번호 명시

/* 자동화 모드 관련 필요 제원값 (신호등)
   기본 값은 모두 0으로 초기화되어 있습니다.
*/

int COUNT_MODEL_1 = 0;
int COUNT_MODEL_2 = 0;
int COUNT_MODEL_3 = 0;

int TIME_MODEL_1 = 0;
int TIME_MODEL_2 = 0;
int TIME_MODEL_3 = 0;

/* 자동 모드시 LED의 유지시간을 결정하는 초기화 코드. 5000은 5초를, 700는 0.7초를 의미합니다.*/
int TIME_RED = 5000;
int TIME_YELLOW = 700;
int TIME_GREEN = 2500;

int STATUS_MODEL_1 = 1;
int STATUS_MODEL_2 = 2;
int STATUS_MODEL_3 = 3;

int RED = 1;
int YELLOW = 2;
int GREEN = 3;

/* 자동모드 조건코드 초기화. 기본값은 true */
boolean isAutoMode = true;

/* 아두이노에서 가장 먼저 실행되는 setup 메소드. 최초 한번만 실행되며 각종 초기 세팅을 할 수 있다. */
void setup()
{  
  // 
	beginPixels();
  // 초당 9600 bit로 직렬 연결을 초기화하는 메소드. Arduino와 컴퓨터 양쪽 모두 동일한 속도의
  // 직렬 연결을 사용하도록 설정해야합니다. 속도 불일치시 데이터가 왜곡되며(문자깨짐 등), Serial Monitor에서도 확인이 불가능할 수 있습니다.
	  // Serial 통신속도는 v0.9.2.2 AT Frimware.bin 업데이트 기준 9600 으로 맞추어져 있습니다.
	Serial.begin(9600);
  // esp8266의 시리얼포트 (2, 3번)에도 동일한 속도를 설정합니다.
	esp8266.begin(9600);

  /* Esp8266 모듈과 AP(공유기)와 연결하는 자동화 메소드들. */
  String cwjap="AT+CWJAP=\"ad\",\"88888888\"\r\n"; // AT+CWJAP 커맨드 준비. AT(공유기) 접속 명령어.
  sendData("AT+RST\r\n",5000,DEBUG);     // 아두이노 IDE 리셋 초기화 설정
  sendData(cwjap,6000,DEBUG);       // 아두이노 와이파이 모듈 접속
  sendData("AT+CIFSR\r\n",3000,DEBUG);          // 아두이노가 켜지면 ESP8266에 할당된 IP 주소를 받아옴.
  sendData("AT+CIPMUX=1\r\n",3000,DEBUG);       // 멀티 커넥션(다중 접속)을 위한 설정
  sendData("AT+CIPSERVER=1,80\r\n",3000,DEBUG); // 포트 번호 80번으로 설정
  pinMode(countPin,OUTPUT); // define counter 13번으로 지정된 핀을 출력한다. 
}

/* 스케치에서 아두이노로 소스를 업로딩하면 무한 실행 반복되는 메소드.
   조건문 등으로 상태를 제어할 수 있습니다. */
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

 /* esp8266 통신이 여전히 가능할 경우 실행되는 (즉 esp8266.available() 이 true일시) 코드. */ 
 if(esp8266.available()) {
  //'+IPD'는 네트워크를 통해 데이터를 받을시 함께 넘어오는 기본문자열입니다. 
  // esp8266 통신이 아직 가능하지만 수신된 네트워크 데이터가 없으면 (+IPD 미발견시) 실행되지 않습니다.
    if(esp8266.find("+IPD,"))
    {
      delay(1000); // 지연시간을 설정합니다. 1000은 1초.
      int connectionId = esp8266.read()-48; //esp8266이 읽어들이는 데이터는 ASCII 코드이며,
                                           //받을 데이터는 숫자로 팀원 모두와 약속했으므로
                                           // 받은 데이터에 -48 하여 원하는 데이터를 파싱합니다.

      if(esp8266.find("model=")) { // http get 통신을 통해 전달받는 model parameter의 숫자 데이터는 총 3개입니다. // 예시) GET /arduino/servlet?model=4 HTTP/1.1
                                                                                                          //      Host: localhost
                                   //  3 미만의 숫자. 수/자동 전환을 하지 않으면서도 LED의 색을 변환할때 사용됩니다.
                                   //  3 미만의 숫자는 수동모드일때만 넘기며, 자동모드일시 넘기지 않습니다.                                   
                                   //  4 : 수동모드로 전환한다.
                                   //  6 : 자동모드로 전환한다.
                                   
        int model = esp8266.read()-48;  // 수신된 ASCII 코드에 -48 하여 원하는 데이터를 파싱합니다.
        
        Serial.println(model); // 시리얼 모니터에서 숫자를 확인하도록 설정합니다.
        
        if(model > 3) {
          if (model == 4) { // 4 : 수동모드 전환.
            isAutoMode = false;
            makeX_model1();
            makeX_model2();
            makeX_model3();
          } else if (model == 6) { // 모드 toggle. 기본이 자동이므로 수동으로 변환, 수동변환 이후 자동으로 변환할 수 있다.
            isAutoMode= !isAutoMode;
          } else {
            isAutoMode = true;
          }
        } else {
                    // else는 수동모드일때만 작동한다.

          esp8266.find("light="); // http get 통신을 통해 전달받는 light의 숫자 데이터는 총 3개입니다. 예시) GET /arduino/servlet?light=4 HTTP/1.1
                                  // 1 : LED RED
                                  // 2 : LED YELLOW
                                  // 3 : LED GREEN
                                  
          int light = esp8266.read()-48;
          turnOn(model, light);
          
          String closeCommand = "AT+CIPCLOSE="; // esp8266와의 통신을 종료하도록 설정합니다.
          closeCommand+=connectionId; 명령어 붙이기 즉 "AT+CIPCLOSE=connectionId"
          closeCommand+="\r\n"; //종료 코드가 가장 앞에 배치되도록 합니다.
          sendData(closeCommand,1000,DEBUG); // 종료 실행.
                                              // 종료가 되더라도 arduino 는 또다시 loop 메소드를 재시작하므로 arduino는 종료되지 않고 esp8266의 통신만 종료됩니다.
        }
      }
    }
  }
}

/* setup() 실행시 자동으로 esp8266와 AP(공유기)를 세팅하는 메소드, 또한 esp8266 통신 자동종료 메소드 */
String sendData(String command, const int timeout, boolean debug) //오른쪽부터 각각 명령어, 대기시간, debug 여부에 해당하는 파라메터
{
  String response = "";
  esp8266.print(command); // command 문자열을 ESP8266에 보냅니다.
  long int time = millis(); // millis() : 현재시간(아두이노가 시작된 후부터 경과된 시간)을 밀리초 단위로(1/1000 초) 반환하는 메소드.
  
  while((time+timeout) > millis()) //대기시간만큼만 동작하도록 설정하였음.
  {
    while(esp8266.available()) //esp8266 통신이 가능하다면 실행한다.
    {
      char c = esp8266.read(); // 수신된 byte 데이터를 읽는 read() 메소드.                          
      response += c;   // byte 크기이므로 char 로 받아서 String 에 기입합니다.
    }
  }
	
	if(debug) {
		Serial.print(response);
	}

  return response;
}

/* 수동 모드일시 사용되는 메소드 */
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
/* 자동모드일때 사용되는 turnLight1, turnLight2, turnLight3 메소드*/
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

/* Strip 타입 네오픽셀은 각각의 LED에 대하여 색깔을 모두 지정해줄 수 있다. RGB로 지정할 수 있다. */
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
