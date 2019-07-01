#include "MyBle.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("APP SETUP");

  //ble 
  setupBle();
}

void loop() {
  // put your main code here, to run repeatedly:

  //ble
  loopBle();
}
