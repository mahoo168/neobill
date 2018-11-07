#include "Wheel.h"
#include "Arduino.h"

void Wheel::Setup(int r, int l){
  pinR = r;
  pinL = l;
  pinMode(pinR, OUTPUT);
  pinMode(pinL, OUTPUT);
}
void Wheel::Go(){
  digitalWrite(pinR,HIGH);
  digitalWrite(pinL,HIGH);
}
void Wheel::Stop(){
  digitalWrite(pinR,LOW);
  digitalWrite(pinL,LOW);
}
