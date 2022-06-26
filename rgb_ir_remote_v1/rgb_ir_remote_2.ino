#include <Adafruit_NeoPixel.h>
#include "TinyIRReceiver.hpp"

#define IR_INPUT_PIN    2
#define IR_FEEDBACK_LED_PIN 4

#define LED_COUNT 8
#define LED_PIN 3
#define sc 65535/360


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


uint16_t Address;
uint8_t Command;
bool isRepeated;
bool isNew;

short h = 0;
short bri = 200;
byte st = 255; // 0 -> white , 255 - > hue color
byte sp = 30;
bool on = false;

enum mode {
  NONE,
  FLASH,
  FADE,
  SMOOTH,
  STROBE
} mode = NONE;

const byte remoteCmd[6][4]={
   {0, 1, 2, 3},
   {4, 5, 6, 7},
   {8, 9,10,11},
  {12,13,14,15},
  {16,17,18,19},
  {20,21,22,23}
};

const int remoteAdr = 0xEF00;

void setup() {
  Serial.begin(115200);

  initPCIInterruptForTinyReceiver();


      
  strip.begin();
  strip.clear();
  strip.show();
  delay(20);
}

void loop() {
  if(isNew){
    isNew = false;

    if(Address == remoteAdr){
        
      if(Command == remoteCmd[0][3] && !on){       //on
        of(false);
      }
      else if(on){
        
        if(Command == remoteCmd[0][2]){  //off
          of(true);
        }

        else if(Command == remoteCmd[0][1]){  //down
          if(isRepeated){
            changebri(-30);
          }
          else{
            changebri(-15);
          }
        }
        
        else if(Command == remoteCmd[0][0]){  //up
          if(isRepeated){
            changebri(30);
          }
          else{
            changebri(15);
          }
        }

        else if(Command == remoteCmd[1][0])  //red 1
          setH(0*sc);
        else if(Command == remoteCmd[2][0])  //red 2
          setH(15*sc);
        else if(Command == remoteCmd[3][0])  //red 3
          setH(30*sc);
        else if(Command == remoteCmd[4][0])  //red 4
          setH(45*sc);
        else if(Command == remoteCmd[5][0])  //red 5
          setH(60*sc);

        else if(Command == remoteCmd[1][1])  //green 1
          setH(120*sc);
        else if(Command == remoteCmd[2][1])  //green 2
          setH(90*sc);
        else if(Command == remoteCmd[3][1])  //green 3
          setH(150*sc);
        else if(Command == remoteCmd[4][1])  //green 4
          setH(180*sc);
        else if(Command == remoteCmd[5][1])  //green 5
          setH(210*sc);

        else if(Command == remoteCmd[1][2])  //blue 1
          setH(240*sc);
        else if(Command == remoteCmd[2][2])  //blue 2
          setH(255*sc);
        else if(Command == remoteCmd[3][2])  //blue 3
          setH(270*sc);
        else if(Command == remoteCmd[4][2])  //blue 4
          setH(330*sc);
        else if(Command == remoteCmd[5][2])  //blue 5
          setH(300*sc);

        else if(Command == remoteCmd[1][3])  //white
          setWhite(true);
//        else if(Command == remoteCmd[2][3])  //blue 2
//          setH(255*sc);
//        else if(Command == remoteCmd[3][3])  //blue 3
//          setH(270*sc);
//        else if(Command == remoteCmd[4][3])  //blue 4
//          setH(330*sc);
//        else if(Command == remoteCmd[5][3])  //blue 5
//          setH(300*sc);

      }
    }
  }
}


void of(bool isOff){
  //strip.clear();
  on = !isOff;
  if(isOff){
//    bri = 255;
//    sp = 30;
//    mode = NONE;
    strip.clear();
    strip.show();
    delay(20);
  }
  else 
    updateStrip();
}


void changebri(short val){
  bri += val;
  if(bri > 200) bri = 200;
  if(bri < 20) bri = 20;
    
  if(mode != STROBE) updateStrip();
}


void setH(int hue){
  h = hue;
  Serial.println(h);
  setWhite(false);
}

void setWhite(bool isWhite){
  isWhite ? st = 0 : st = 255;
  updateStrip();
}



void updateStrip(){
  strip.fill(strip.ColorHSV(h,st,bri),0,LED_COUNT);
  strip.show();
  delay(20);
}


void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat){
    
    Address = aAddress;
    Command = aCommand;
    isRepeated = isRepeat;
    isNew = true;
}
