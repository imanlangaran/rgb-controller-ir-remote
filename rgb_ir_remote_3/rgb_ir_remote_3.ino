#include <Adafruit_NeoPixel.h>
#include "TinyIRReceiver.hpp"
#include "ColorConverterLib.h"

#define IR_INPUT_PIN    2
#define IR_FEEDBACK_LED_PIN 4

#define LED_COUNT 20
#define LED_PIN 3
#define sc 65535/360
#define remoteAdr 0xEF00

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_BRG + NEO_KHZ800);


uint16_t Address;
uint8_t Command;
bool isRepeated;
bool isNew;

float red = 0;
float grn = 0;
float blu = 0;

byte prev_red = 255;
byte prev_grn = 0;
byte prev_blu = 0;

double step_red = 0;
double step_grn = 0;
double step_blu = 0;

byte bri = 200;
byte prev_bri = 0;
byte sp = 30;
bool on = false;


double h = 0;


long nw;
long last_millis;

enum mode {
  NONE,
  FLASH,
  FADE,
  SMOOTH,
  STROBE
} mode = NONE;

enum animation {
  going_on,
  going_of,
  change_color,
  change_bri,
  none
} animate = none;

const byte remoteCmd[6][4]={
   {0, 1, 2, 3},
   {4, 5, 6, 7},
   {8, 9,10,11},
  {12,13,14,15},
  {16,17,18,19},
  {20,21,22,23}
};


void setup() {
//  Serial.begin(115200);

  initPCIInterruptForTinyReceiver();


      
  strip.begin();
  strip.setBrightness(bri);
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
          changebri(isRepeated?-30:-15);
        }
        
        else if(Command == remoteCmd[0][0]){  //up
          changebri(isRepeated?30:15);
        }

        else if(Command == remoteCmd[1][0])  //red 1
//          setH(0*sc);
          setH(255,0,0);
        else if(Command == remoteCmd[2][0])  //red 2
//          setH(15*sc);
          setH(255,64,0);
        else if(Command == remoteCmd[3][0])  //red 3
//          setH(30*sc);
          setH(255,128,0);
        else if(Command == remoteCmd[4][0])  //red 4
//          setH(45*sc);
          setH(255,191,0);
        else if(Command == remoteCmd[5][0])  //red 5
//          setH(60*sc);
          setH(255,255,0);

        else if(Command == remoteCmd[1][1])  //green 1
//          setH(120*sc);
          setH(0,255,0);
        else if(Command == remoteCmd[2][1])  //green 2
//          setH(90*sc);
          setH(128,255,0);
        else if(Command == remoteCmd[3][1])  //green 3
//          setH(150*sc);
          setH(0,255,128);
        else if(Command == remoteCmd[4][1])  //green 4
//          setH(180*sc);
          setH(0,255,255);
        else if(Command == remoteCmd[5][1])  //green 5
//          setH(210*sc);
          setH(0,128,255);

        else if(Command == remoteCmd[1][2])  //blue 1
//          setH(240*sc);
          setH(0,0,255);
        else if(Command == remoteCmd[2][2])  //blue 2
//          setH(255*sc);
          setH(64,0,255);
        else if(Command == remoteCmd[3][2])  //blue 3
//          setH(270*sc);
          setH(128,0,255);
        else if(Command == remoteCmd[4][2])  //blue 4
//          setH(330*sc);
          setH(255,0,128);
        else if(Command == remoteCmd[5][2])  //blue 5
//          setH(300*sc);
          setH(255,0,255);

        else if(Command == remoteCmd[1][3])  //white
//          setWhite(true);
          setH(255,255,255);
//        else if(Command == remoteCmd[2][3])  //blue 2
//          setH(255*sc);
        else if(Command == remoteCmd[3][3])  //blue 3
          setMode(STROBE);
//        else if(Command == remoteCmd[4][3])  //blue 4
//          setH(330*sc);
        else if(Command == remoteCmd[5][3]){  //strobe
          setMode(SMOOTH);
          RGBConverter::RgbToHsv(red,grn,blu,h);
          h = h*360;
//          h = h * 65535;
//          Serial.println(h);
//          Serial.println((short)h*sc);
        }

      }
    }
  }

  nw = millis();
  if(mode == STROBE && on){
    if(nw - last_millis > 75){
      last_millis = nw;
      updateStrobe();
    }
  }

  else if(mode == SMOOTH && on){
    if(nw - last_millis > 75){
      last_millis = nw;
      updateSmooth();
    }
  }

  if(animate == going_of && nw - last_millis > 10){
    last_millis = nw;
    red-=step_red;
    blu-=step_blu;
    grn-=step_grn;
    if(0 > red-step_red || 0 > blu-step_blu || 0 > grn-step_grn){
      red = 0;
      blu = 0;
      grn = 0;
      animate = none;
    }
    updateStrip();
  }

  if(animate == going_on && nw - last_millis > 10){
    last_millis = nw;
    red+=step_red;
    blu+=step_blu;
    grn+=step_grn;
    if(prev_red < red+step_red || prev_blu < blu+step_blu || prev_grn < grn+step_grn){
      red = prev_red;
      blu = prev_blu;
      grn = prev_grn;
      animate = none;
    }
    updateStrip();
  }


  if(animate == change_bri && nw - last_millis > 10){
    last_millis = nw;

    if(prev_bri > bri){
      prev_bri-=2;
      if(prev_bri <= bri){
        prev_bri = bri;
        animate = none;
      }
    }

    if(prev_bri < bri){
      prev_bri+=2;
      if(prev_bri >= bri){
        prev_bri = bri;
        animate = none;
      }
    }
    
    strip.setBrightness(prev_bri);
    updateStrip();
  }

  
  if(animate == change_color && nw - last_millis > 10){
    last_millis = nw;

    red+=step_red;
    grn+=step_grn;
    blu+=step_blu;
    

    if(abs(prev_red-red) < abs(step_red)){
      red = prev_red;
      step_red = 0;
    }

    if(abs(prev_grn-grn) < abs(step_grn)){
      grn = prev_grn;
      step_grn = 0;
    }

    
    if(abs(prev_blu-blu) < abs(step_blu)){
      blu = prev_blu;
      step_blu = 0;
    }

    if(red == prev_red && grn == prev_grn && blu == prev_blu) animate = none;
    updateStrip();
  }
  
}

void of(bool isOff){
  //strip.clear();
  on = !isOff;
  if(isOff){
    animate = going_of;
    step_red = (double)prev_red / 100;
    step_grn = (double)prev_grn / 100;
    step_blu = (double)prev_blu / 100;
//    Serial.println(step_red);
//    Serial.println(step_grn);
//    Serial.println(step_blu);
  }
  else {
    animate = going_on;
    step_red = (double)prev_red / 100;
    step_grn = (double)prev_grn / 100;
    step_blu = (double)prev_blu / 100;
//    Serial.println(step_red);
//    Serial.println(step_grn);
//    Serial.println(step_blu);
  }
}



void changebri(short val){
  if(animate != change_bri){
    prev_bri = bri;
  }
  bri += val;
  if(bri > 200) bri = 200;
  else if(bri < 30) bri = 30;
  else  animate = change_bri;

    
  if(mode != STROBE && mode != SMOOTH) updateStrip();
}


void setH(byte redd, byte green, byte blue){
  step_red = (redd - red)/100;
  step_grn = (green - grn)/100;
  step_blu = (blue - blu)/100;

  prev_red = redd; // final color is prev_red !!!
  prev_grn = green; // final color is prev_grn !!!
  prev_blu = blue; // final color is prev_blu !!!


  animate = change_color;
  
  setMode(NONE);
  updateStrip();
}



void setMode(byte m){
  if(!isRepeated){
    if(mode == m){
      if(mode == SMOOTH){
        RGBConverter::HsvToRgb(h/360,1.0,1.0,red,grn,blu);
//        Serial.println(h/360);
//        Serial.println(red);
//        Serial.println(grn);
//        Serial.println(blu);
      }
      mode = NONE;
      updateStrip();
    }
    else
      mode = m;
  }
  
}

int firstPixelHue = 0;
void updateStrobe() {
  firstPixelHue += 256;
  firstPixelHue = (firstPixelHue%65536);  
  for(byte i=0; i<LED_COUNT; i++) {
    int pixelHue = firstPixelHue + (i * 65536L / LED_COUNT);
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue,255,255)));
  }
  strip.show();
  delay(20);
}

void updateSmooth(){
  strip.fill(strip.ColorHSV(((short)h*sc),255,255),0,LED_COUNT);
  strip.show();
  h+=1;
  if(h > 360) h = 0;
}

void updateStrip(){
  
//    Serial.print("r:");
//    Serial.print(red);
//    Serial.print("-g:");
//    Serial.print(grn);
//    Serial.print("-b:");
//    Serial.print(blu);
//    Serial.println();
  strip.fill(strip.Color(red,grn,blu),0,LED_COUNT);
  strip.show();
  delay(10);
}


void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat){
    
    Address = aAddress;
    Command = aCommand;
    isRepeated = isRepeat;
    isNew = true;
}
