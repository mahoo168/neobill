#include "MyBle.h"

#define MOTER_GO 0
#define MOTER_RIGHT 1
#define MOTER_LEFT 2
#define MOTER_BACK 3

void setup() {
  // put your sestup code here, to run once:
  Serial.begin(9600);
  Serial.println("APP SETUP");

  //ble 
  setupBle();
}

void loop() {
  // put your main code here, to run repeatedly:

  //ble
  loopBle();

  //引数に以下を指定する
  //MOTER_GO 
  //MOTER_RIGHT 
  //MOTER_LEFT
  //MOTER_BACK
  moterControll(MOTER_GO);
}
