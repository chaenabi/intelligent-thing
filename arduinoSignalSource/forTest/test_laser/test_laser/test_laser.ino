#include <SoftwareSerial.h> //ESP8266 정보 연결
#define DEBUG true //디버그 실행
SoftwareSerial esp8266(2,3); //ESP8266 아두이노 연결 포트 2,3선언

int cds = 0; // CDS 변수 선언
int light = 800; // 레이저 변수 선언
int count = 3; // 센서 인식 후 유지시간(초)
int state = 0;
int stateCount = 0;

byte RedLightOn = 0;
byte GrnLightOn = 0;
int  RedLightCntr =0;


void setup() {
  Serial.begin(9600); // 아두이노 보드레이트 고정
  esp8266.begin(9600); // 8266 보드레이트 고정
  
  pinMode(7, OUTPUT); // 확인용 LED 핀 할당
  digitalWrite(7, LOW); // 시작시 확인용 LED OFF
}

void loop() {
  cds = analogRead(A0);// 변수 CDS에 아날로그 A0에 입력되는 신호를 대입
  Serial.println(cds); // 시리얼 모니터에 변수 CDS 출력

    
        if(cds<light)
        {
            if(RedLightOn == 0)
            {      
                for(int i=0; i<count; i++)
                {
                    Serial.println(i); // 인식 유지시간 출력
                    delay(500); // 단위 1초로 고정
                }
                
                digitalWrite(7, HIGH); // 950이상의 신호시 LED 점등
                Serial.println("OFF");  // LED 상태 확인을 위한 출력
                String temp = "GET /arduino/sendData?sound=1 HTTP/1.1\r\nHost: localhost\r\n\r\n";  //빨간등 신호 전송
                espsend(temp); //펑션 불러오기
    
                RedLightOn = 1;
                delay(100);
            }
        }
            
        else
        {
            if(GrnLightOn == 0)
            {
                for(int i=0; i<count; i++)
                {
                   Serial.println(i); // 인식 유지시간 출력
                   delay(500); // 단위 1초로 고정
                }
        
                digitalWrite(7, LOW); // 949이하의 신호시 LED 소등
                Serial.println("ON"); // LED 상태 확인을 위한 출력
                String temp = "GET /arduino/sendData?sound=2 HTTP/1.1\r\nHost: localhost\r\n\r\n"; //파란등 신호 전송
                espsend(temp); //펑션 불러오기
    
                GrnLightOn = 1;
                delay(100); // 1초 대기
            }
        }



        if(RedLightOn == 1)
        {
            if(RedLightCntr > 1000)
            {
                RedLightOn    = 0;
                RedLightCntr  = 0;
                GrnLightOn    = 0;
                Serial.println("Red Light Reset Time Over");
            }
            else
            {
                RedLightCntr++;
            }
        }
}

void espsend(String d) { //접속을 위한 펑션
  String cmd = "AT+CIPSTART=\"TCP\",\"192.168.0.50\",8080\r\n";
  sendData(cmd, 1000, true);  
  String cipSend = " AT+CIPSEND=";
  cipSend += d.length();
  cipSend += "\r\n";
  sendData(cipSend, 1000, DEBUG);
  sendData(d, 1000, DEBUG);
}

String sendData(String command, const int timeout, boolean debug) //펀션의 데이터 규격
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
  
  Serial.print(response);

  return response;
}
