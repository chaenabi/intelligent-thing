#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial sound(10,11);
DFRobotDFPlayerMini Satellite_Broadcast;

void setup() {
  sound.begin(9600);
  Serial.begin(9600); 
  sound.listen();
  while(!Satellite_Broadcast.begin(sound) == true) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    delay(1000);
  }
//  if (!Satellite_Broadcast.begin(sound)) {
//
//    while(true);
//  }

  delay(3000);
  Satellite_Broadcast.volume(30);  //볼륨조절 최대30
  Satellite_Broadcast.play(5); 
}

void loop() {
  // put your main code here, to run repeatedly:

}
