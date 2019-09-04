/*
 #include "Arduino.h"
   Arduino.h는 파일이 실행되는데 필요한 특정 소스를 담고 있습니다. 이 파일의 경우 Arduino.h는 DFRobotDFPlayerMini 라이브러리와 연관이 있습니다.
   Arduino 라이브러리는 기본적으로 Arduino.app/Contents/Resources/Java/Hardware/Arduino/Cores/Arduino
   에 위치하고 있으며, 인식이 불가할시 arduino github 에서 Arduino Library 를 zip 파일로 다운로드하여
   스케치 프로그램 상단의 파일/편집/스케치/툴/도움말 탭 중에서
   스케치 - 라이브러리 포함하기 - zip 라이브러리 추가를 통해 Arduino Library를 추가할 수 있습니다.
   
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

 #include "DFRobotDFPlayerMini.h"
  오디오 코덱과 관련된 라이브러리입니다.
  Mp3(또는 WMV)를 재생시켜줍니다.
  스케치 프로그램 상단의 파일/편집/스케치/툴/도움말 탭 중에서
  스케치 - 라이브러리 포함하기 - 라이브러리 관리 클릭, dfplay 를 검색하여 설치할 수 있습니다.
*/

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial esp8266(2,3); // esp8266 wifi 모듈의 통신포트는 2, 3으로 설정하였음.
SoftwareSerial sound(10,11); // 사운드 데이터를 받기 위한 시리얼통신 포트를 10, 11로 추가하였음.
DFRobotDFPlayerMini Satellite_Broadcast; //오디오 라이브러리를 Satellite_Broadcast라는 변수로 지정한다.

String ssid = "ad";      // SSID 아이디 = AP(공유기) 아이디 
String password = "";   // SSID 패스워드 = AP(공유기) 비밀번호

#define DEBUG true // DEBUG가 true 일시 특정 문자열을 시리얼 모니터에 출력한다.


/* 아두이노에서 가장 먼저 실행되는 setup 메소드. 최초 한번만 실행되며 각종 초기 세팅을 할 수 있다. */
void setup() {
  /*
   초당 9600 bit로 직렬 연결을 초기화하는 메소드. Arduino와 컴퓨터 양쪽 모두 동일한 속도의
   직렬 연결을 사용하도록 설정해야한다. 속도 불일치시 데이터가 왜곡되며(문자깨짐 등), Serial Monitor에서도 확인이 불가능할 수 있습니다.
   9600는 Arduino의 기본값입니다.
  */
  Serial.begin(9600); 
  esp8266.begin(9600);  // esp8266의 시리얼포트 (2, 3번)에도 동일한 속도를 설정합니다. 
  sound.begin(9600);   // sound의 시리얼포트 (2, 3번)에도 동일한 속도를 설정합니다.
  Serial.println("start!");

  
  
  /* 방송 부분 */
  
  while(!Satellite_Broadcast.begin(sound) == true) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    delay(1000);
  }
  delay(3000);
  Satellite_Broadcast.volume(24);  //볼륨조절 최대30
  Satellite_Broadcast.play(3); // 방송을 시작하겠다는 사운드가 출력되는 번호.
}

void loop()
{
  // 기본적으로 아두이노는 한번에 하나의 시리얼 통신만 세팅이 가능합니다.
  // 두개 이상의 시리얼 통신을 해야할 필요가 생겼을 경우 해결가능한 listen() 메소드.
  // esp8266.listen()은
  // esp8266 시리얼(즉 2, 3번포트) 에서 수신을 시작하겠다는 선언입니다. 
  esp8266.listen(); 
  
 if(esp8266.available()) /* esp8266 통신이 여전히 가능할 경우 실행되는 (즉 esp8266.available() 이 true일시) 코드. */ 
 {
    //'+IPD'는 네트워크를 통해 데이터를 받을시 함께 넘어오는 기본문자열입니다. 
    // esp8266 통신이 아직 가능하지만 수신된 네트워크 데이터가 없으면 (+IPD 미발견시) 실행되지 않습니다. 
    if(esp8266.find("+IPD,"))
    {
      delay(1000); // 지연시간을 설정합니다. 1000은 1초.
      int connectionId = esp8266.read()-48; //esp8266이 읽어들이는 데이터는 ASCII 코드이며,
                                           //받을 데이터는 숫자로 팀원 모두와 약속했으므로
                                           // 받은 데이터에 -48 하여 원하는 데이터를 파싱합니다.
      esp8266.find("sound="); // http get 통신을 통해 전달받는 sound parameter의 숫자 데이터는 총 4개입니다. // 예시) GET /arduino/servlet?sound=4 HTTP/1.1
                                                                                                          //      Host: localhost
      int event_Broadcast = esp8266.read()-48; 
      Serial.println(event_Broadcast);
      
      switch(event_Broadcast) {
        case 1 : Satellite_Broadcast.play(1); break; 
        case 2 : Satellite_Broadcast.play(2); break;
        case 4 : Satellite_Broadcast.play(4); break;
        case 5 : Satellite_Broadcast.play(5); break;
      }
      event_Broadcast = 0; // 0으로 초기화하여 loop 재실행시 사운드가 반복되지 않도록 설정합니다.
      String closeCommand = "AT+CIPCLOSE="; // esp8266와의 통신을 종료하도록 설정합니다.
      closeCommand+=connectionId; // 명령어 붙이기 즉 "AT+CIPCLOSE=connectionId"
      closeCommand+="\r\n"; //종료 코드가 가장 앞에 배치되도록 합니다.
  
      sendData(closeCommand,1000,DEBUG);  // 종료 실행.
                                          // 종료가 되더라도 arduino 는 또다시 loop 메소드를 재시작하므로 arduino는 종료되지 않고 esp8266의 통신만 종료됩니다.
  
    }
 }
 delay(1000); // loop 가 재실행되기전까지 지연시간 1초
}

/* 자동으로 통신 연결을 종료하는 메소드  */
String sendData(String command, const int timeout, boolean debug) //오른쪽부터 각각 명령어, 대기시간, debug 여부에 해당하는 파라메터
{
  String response = "";
  esp8266.print(command); // command 문자열을 ESP8266에 보냅니다.
  long int time = millis(); // millis() : 현재시간(아두이노가 시작된 후부터 경과된 시간)을 밀리초 단위로(1/1000 초) 반환하는 메소드.
  
  while((time+timeout) > millis()) //대기시간만큼만 동작하도록 설정하였음.
  {
    while(esp8266.available()) //esp8266 통신이 아직 가능하다면 실행한다.
    {
      char c = esp8266.read(); // 수신된 byte 데이터를 읽는 read() 메소드.
      response += c;    // byte 크기이므로 char 로 받아서 String 에 기입합니다.
    }
  }
   
   if(debug) {
      Serial.print(response);
   }

  return response;
}
