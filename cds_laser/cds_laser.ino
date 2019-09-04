/*
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

#include <SoftwareSerial.h> //ESP8266 정보 연결
#define DEBUG true // debug true일시 특정 데이터 시리얼 모니터에 출력
SoftwareSerial esp8266(2,3); //ESP8266 아두이노 연결 포트 2,3선언

int cds = 0; // CDS 변수 선언
int light = 800; // 레이저 변수 선언
int count = 1; // 센서 인식 후 유지시간(초)
int state = 0;
int stateCount = 0;

byte isLocking = 1;         // 데이터 전송 확인 여부
byte isSensorWorking = 0;   // 센서 동작 확인 여부


void setup() {
  Serial.begin(9600); // 아두이노 보드레이트 고정
  esp8266.begin(9600); // 8266 보드레이트 고정
  
  pinMode(7, OUTPUT); // 확인용 LED 핀 할당
  digitalWrite(7, LOW); // 시작시 확인용 LED OFF
 
  String cwjap="AT+CWJAP=\"Bikoo\",\"12345678\"\r\n"; // AT+CWJAP 커멘드 준비.
  sendData("AT+RST\r\n",5000,DEBUG);     // 아두이노 IDE 리셋 초기화 설정
  sendData(cwjap,6000,DEBUG);       // 아두이노 와이파이 모듈 접속
  sendData("AT+CIFSR\r\n",3000,DEBUG);          // 아두이노가 켜지면 ESP8266에 할당된 IP 주소를 받아옴.
  sendData("AT+CIPMUX=1\r\n",3000,DEBUG);       // 멀티 커넥션(다중 접속)을 위한 설정
  sendData("AT+CIPSERVER=1,80\r\n",3000,DEBUG); // 포트 번호 80번으로 설정
}

void loop() {
  cds = analogRead(A0);// 변수 CDS에 아날로그 A0에 입력되는 신호를 대입
  Serial.println(cds); // 시리얼 모니터에 변수 CDS 출력

    
  if(cds<light)
  {
      digitalWrite(7, HIGH); // 950이상의 신호시 LED 점등
      if(isSensorWorking == 0) {  // 센서가 동작하지 않았다면
        isSensorWorking = 1;      // 센서가 동작하며
        isLocking = 0;            // 데이터 전송 준비 완료됨
      }
  }
      
  else
  {
      digitalWrite(7, LOW); // 949이하의 신호시 LED 소등
      if(isSensorWorking == 1) {  // 센서가 동작하고 있다면
          isSensorWorking = 0;    // 센서가 동작하지 않으며
          isLocking = 0;          // 데이터 전송 준비 완료됨
      }
  }


  if(isLocking == 0) {
    if(isSensorWorking == 1) {
          String temp = "GET /arduino/LaserSensorEvent?sensor=1 HTTP/1.1\r\nHost: localhost\r\n\r\n"; //서버에 비상 신호 On 전송 준비
          espsend(temp); //펑션 불러오기
          sendSignal(4);  // LED 아두이노에 데이터 전송
    } else {
          String temp = "GET /arduino/LaserSensorEvent?sensor=0 HTTP/1.1\r\nHost: localhost\r\n\r\n"; //서버에 비상 신호 Off 전송 준비
          espsend(temp); //펑션 불러오기
          sendSignal(5);  // LED 아두이노에 데이터 전송
    }
    isLocking = 1;        // 데이터 전송 후, 신호가 바뀔 때까지 데이터를 다시 보내지 않음
  }
}

void espsend(String d) { //접속을 위한 펑션
  String cmd = "AT+CIPSTART=0,\"TCP\",\"192.168.0.50\",8080\r\n";
  sendData(cmd, 500, true);  
  String cipSend = " AT+CIPSEND=0,";
  cipSend += d.length();
  cipSend += "\r\n";
  sendData(cipSend, 500, DEBUG);
  sendData(d, 500, DEBUG);

  String closeCommand = "AT+CIPCLOSE=0\r\n";
  sendData(closeCommand,1000,DEBUG);
}

void sendSignal(int model) { //접속을 위한 펑션
  String cmd = "AT+CIPSTART=1,\"TCP\",\"192.168.0.13\",80\r\n";
  sendData(cmd, 500, true);  
  String cipSend = "AT+CIPSEND=1,";
  String d;
  if(model == 4) {
    d = "GET /?model=4 HTTP/1.1\r\nHost: localhost\r\n\r\n";
  } else {
    d = "GET /?model=5 HTTP/1.1\r\nHost: localhost\r\n\r\n";
  }
  cipSend += d.length();
  cipSend += "\r\n";
  sendData(cipSend, 500, DEBUG);
  sendData(d, 500, DEBUG);

  String closeCommand = "AT+CIPCLOSE=1\r\n";  // 채널 Close 신호 전송
  sendData(closeCommand,1000,DEBUG);
}

/* setup() 실행시 자동으로 esp8266와 AP(공유기)를 세팅하는 메소드, 또한 esp8266 통신 자동종료 메소드 */
String sendData(String command, const int timeout, boolean debug) //펑션의 데이터 규격 //오른쪽부터 각각 명령어, 대기시간, debug 여부에 해당하는 파라메터
{
  String response = "";

  esp8266.print(command); // command 문자열을 ESP8266에 보냅니다.
  long int time = millis(); // millis() : 현재시간(아두이노가 시작된 후부터 경과된 시간)을 밀리초 단위로(1/1000 초) 반환하는 메소드.

  while((time+timeout) > millis()) //대기시간만큼만 동작하도록 설정하였음.
  {
    while(esp8266.available())  //esp8266 통신이 가능하다면 실행한다.
    {
      char c = esp8266.read();  // 수신된 byte 데이터를 읽는 read() 메소드.           
      response += c; // byte 크기이므로 char 로 받아서 String 에 기입합니다.
    
    } 
  }
  
  Serial.print(response);

  return response;
}
