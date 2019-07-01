#include "Wheel.h"
#include "Arduino.h"

void Wheel::Setup(int r0, int r1, int l0,int l1, int r_pwm, int l_pwm){
  pinR0 = r0;
  pinL0 = l0;
  pinR1 = r1;
  pinL1 = l1;
  pinR_pwm = r_pwm;
  pinL_pwm = l_pwm;
  pinMode(pinR0, OUTPUT);
  pinMode(pinL0, OUTPUT);
  pinMode(pinR1, OUTPUT);
  pinMode(pinL1, OUTPUT);
  pinMode(pinR_pwm, OUTPUT);
  pinMode(pinL_pwm, OUTPUT);
}
void Wheel::Go(){
  digitalWrite(pinR0,HIGH);
  digitalWrite(pinR1,LOW);
  digitalWrite(pinL0,HIGH);
  digitalWrite(pinL1,LOW);
}
void Wheel::Stop(){
  digitalWrite(pinR0,LOW);
  digitalWrite(pinR1,LOW);
  digitalWrite(pinL0,LOW);
  digitalWrite(pinL1,LOW);
}

void Wheel::Right(){
  digitalWrite(pinR0,HIGH);
  digitalWrite(pinR1,LOW);
  digitalWrite(pinL0,LOW);
  digitalWrite(pinL1,HIGH);
}

void Wheel::Left(){
  digitalWrite(pinR0,LOW);
  digitalWrite(pinR1,HIGH);
  digitalWrite(pinL0,HIGH);
  digitalWrite(pinL1,LOW);
}

void Wheel::Speed(int pwm){
  analogWrite(pinR_pwm,pwm);
  analogWrite(pinL_pwm,pwm);
}
void Wheel::Back(){
  digitalWrite(pinR0,LOW);
  digitalWrite(pinR1,HIGH);
  digitalWrite(pinL0,LOW);
  digitalWrite(pinL1,HIGH);
}
