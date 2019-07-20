#include "MyBle.h"
#include "HeartBeat.h"
#include "MyPWM.h"
#include "MyHR.h"
#define MOTER_GO 0
#define MOTER_RIGHT 1
#define MOTER_LEFT 2
#define MOTER_BACK 3

#define PIN_R D6
#define PIN_G D7
#define PIN_B D8
#define PIN_FREC_MOTER D9

Ticker ticker;
MyPWM ledR;
MyPWM ledG;
MyPWM ledB;
MyPWM frec_Moter;
int t = 1;
int tcount = 0;
bool tState = false;
void callback(){
  //ble
  loopBle();
}

void setup() {
  // put your sestup code here, to run once:
  //Serial.begin(9600);
  //ble 
  setupBle();

  //heartBeat
  int h = HeartBeatSetup();

  //LED
  //pinMode(13,OUTPUT);
  
  ledR.setup(PIN_R,0);
  ledG.setup(PIN_G,0);
  ledB.setup(PIN_B,0);
  frec_Moter.setup(PIN_FREC_MOTER,0);
  //frec moter
  //pinMode(PIN_FREC_MOTER, OUTPUT);
  ticker.attach(callback, 1);
  //Serial.println("BLE START");
}

void loop() {
  int h,s,v;
  h = HeartBeatLoop() ;
  if(isDemoMode){
    demo();
  }
  else{
      tcount++;
      if(tcount % 100 == 0){
        t++;
        if(t > 1024){
          t = 0;
          tState = !tState;
        }
        //heatbeat:

        //h = t;
        s = 200;
        v = 200;
    
        int r,g,b = 0;
        HSV2RGB(h *255 / 1024,s,v,r,g,b);
        
        ledR.write(r );
        ledG.write(g );
        ledB.write(b ); 
        frec_Moter.write(h );
        tcount = 0;
      }     
      //pwm
      if(isLED){
        ledR.loop();
        ledG.loop();
        ledB.loop();      
      }
      else{
        ledR.stop(); ledG.stop(); ledB.stop();
      }
      if(isFrec){
        frec_Moter.loop();
      }
      else{
        frec_Moter.stop();
      }
  }
}
