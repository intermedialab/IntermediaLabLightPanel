#include <DmxMaster.h>

int startupDelayMillis = 3000;
float touchStepSize = 0.25;
float smoothSpeedStartup = 0.005;
float smoothSpeedNormal = 0.03;

int startChannel = 24;

int masterChannelOuter = 1;
boolean masterChannelOuterOn = false;
int masterChannelInner = 13;
boolean masterChannelInnerOn = false;

float rawBrightnessInner = 0;
float brightnessInner = 0;
float brightnessInnerSmoothed = 0;
float rawTemperatureInner = 0;
float temperatureInner = 0.;

float rawBrightnessOuter = 0;
float brightnessOuter = 0;
float brightnessOuterSmoothed = 0;
float rawTemperatureOuter = 0;
float temperatureOuter = 0.;

int pinLEDinner = A0;
int pinLEDouter = A1;

int pinToggleInner = 13;
boolean pinToggleInnerState = HIGH;
long toggleInnerStartupTime = 0;

int pinToggleOuter = 8;
boolean pinToggleOuterState = HIGH;
long toggleOuterStartupTime = 0;

int pinDownInner = 12;
int pinDownOuter = 7;

int pinUpInner = 11;
int pinUpOuter = A2;

int pinColderInner = 10;
int pinColderOuter = A3;

int pinWarmerInner = 9;
int pinWarmerOuter = A4;

void setup() {
  /* The most common pin for DMX output is pin 3, which DmxMaster
   ** uses by default. If you need to change that, do it here. */
  //  DmxMaster.usePin(3);

  /* DMX devices typically need to receive a complete set of channels
   ** even if you only need to adjust the first channel. You can
   ** easily change the number of channels sent here. If you don't
   ** do this, DmxMaster will set the maximum channel number to the
   ** highest channel you DmxMaster.write() to. */
  DmxMaster.maxChannel(startChannel+16);

  pinMode( pinLEDinner, OUTPUT);
  pinMode( pinLEDouter, OUTPUT);

  pinMode( pinToggleInner, INPUT_PULLUP);
  pinMode( pinToggleOuter, INPUT_PULLUP);

  pinMode( pinDownInner, INPUT_PULLUP);
  pinMode( pinDownOuter, INPUT_PULLUP);

  pinMode( pinUpInner, INPUT_PULLUP);
  pinMode( pinUpOuter, INPUT_PULLUP);

  pinMode( pinColderInner, INPUT_PULLUP);
  pinMode( pinColderOuter, INPUT_PULLUP);

  pinMode( pinWarmerInner, INPUT_PULLUP);
  pinMode( pinWarmerOuter, INPUT_PULLUP);

  //Serial.begin(9600);

}

void loop() {

  boolean innerState = digitalRead(pinToggleInner);

  if(innerState == LOW && pinToggleInnerState == HIGH){
    if(rawBrightnessInner == 0) {
      if(!masterChannelInnerOn){
        toggleInnerStartupTime = millis() + startupDelayMillis;
      }
      rawBrightnessInner = 255;
      rawTemperatureInner = 127;
    } 
    else {
      rawBrightnessInner = 0;
      rawTemperatureInner = 0;
    } 
    //Serial.println("Toggle Inner");
  }

  pinToggleInnerState = innerState;

  if(digitalRead(pinUpInner) == LOW){
    if(digitalRead(pinDownInner) == LOW){
      rawBrightnessInner = rawBrightnessOuter;
    } 
    else {
      rawBrightnessInner+=touchStepSize;
      rawBrightnessInner = min(rawBrightnessInner, 255);
    }
  } 
  else if(digitalRead(pinDownInner) == LOW){
    rawBrightnessInner-=touchStepSize;
    rawBrightnessInner = max(rawBrightnessInner, 0);
  }

  if(digitalRead(pinWarmerInner) == LOW){
    if(digitalRead(pinColderInner) == LOW){
      rawTemperatureInner = rawTemperatureOuter;
    } 
    else {
      rawTemperatureInner-=touchStepSize;
      rawTemperatureInner = max(rawTemperatureInner, 0);
    }
  } 
  else if(digitalRead(pinColderInner) == LOW){
    rawTemperatureInner+=touchStepSize;
    rawTemperatureInner = min(rawTemperatureInner, 255);
  }

  boolean outerState = digitalRead(pinToggleOuter);

  if(outerState == LOW && pinToggleOuterState == HIGH){
    if(rawBrightnessOuter == 0) {
      if(!masterChannelOuterOn){
        toggleOuterStartupTime = millis() + startupDelayMillis;
      }
      rawBrightnessOuter = 255;
      rawTemperatureOuter = 127;
    } 
    else {
      rawBrightnessOuter = 0;
      rawTemperatureOuter = 0;
    } 
    //Serial.println("Toggle Outer");
  }

  pinToggleOuterState = outerState;

  if(digitalRead(pinUpOuter) == LOW){
    if(digitalRead(pinDownOuter) == LOW){
      rawBrightnessOuter = rawBrightnessInner;
    } 
    else {
      rawBrightnessOuter+=touchStepSize;
      rawBrightnessOuter = min(rawBrightnessOuter, 255);
    }
  } 
  else if(digitalRead(pinDownOuter) == LOW){
    rawBrightnessOuter-=touchStepSize;
    rawBrightnessOuter = max(rawBrightnessOuter, 0);
  }

  if(digitalRead(pinWarmerOuter) == LOW){
    if(digitalRead(pinColderOuter) == LOW){
      rawTemperatureOuter = rawTemperatureInner;
    } 
    else {
      rawTemperatureOuter-=touchStepSize;
      rawTemperatureOuter = max(rawTemperatureOuter, 0);
    }
  } 
  else if(digitalRead(pinColderOuter) == LOW){
    rawTemperatureOuter+=touchStepSize;
    rawTemperatureOuter = min(rawTemperatureOuter, 255);
  }

  if(toggleInnerStartupTime < millis()){
    float smoothSpeed = smoothSpeedNormal;
    brightnessInner *= 1.0-smoothSpeed;
    brightnessInner += (smoothSpeed*rawBrightnessInner);
    brightnessInnerSmoothed *= 0.99;
    brightnessInnerSmoothed += brightnessInner * 0.01;
    temperatureInner *= 0.99;
    temperatureInner += 0.01*rawTemperatureInner;
  }

  if(toggleOuterStartupTime < millis()){
    float smoothSpeed = smoothSpeedNormal;
    brightnessOuter *= 1.0-smoothSpeed;
    brightnessOuter += (smoothSpeed*rawBrightnessOuter);
    brightnessOuterSmoothed *= 0.99;
    brightnessOuterSmoothed += brightnessOuter * 0.01;
    temperatureOuter *= 0.99;
    temperatureOuter += 0.01*rawTemperatureOuter;
  }
  
  //Serial.print("\t B1: ");
  //Serial.print(brightnessInner);
  //Serial.print("\t T1: ");
  //Serial.print(temperatureInner);
  //Serial.print("\t B2: ");
  //Serial.print(brightnessOuter);
  //Serial.print("\t T2: ");
  //Serial.println(temperatureOuter);

  int coldComponentOuter = round(constrain(map(temperatureOuter, 0.0, 127.0, 0.0, 255.0), 0.0,255.0)*brightnessOuter/255.);
  int warmComponentOuter = round(constrain(map(temperatureOuter, 127.0, 255.0, 255.0, 0.0), 0.0,255.0)*brightnessOuter/255.);
  int coldComponentInner = round(constrain(map(temperatureInner, 0.0, 127.0, 0.0,255.0), 0.0,255.0)*brightnessInner/255.);
  int warmComponentInner = round(constrain(map(temperatureInner, 127.0, 255.0, 255.0, 0.0), 0.0,255.0)*brightnessInner/255.);

  // only turn on the master switches if there's some brightness

  if(brightnessOuterSmoothed > 0.001){
    digitalWrite(pinLEDouter, HIGH);
    masterChannelOuterOn = true;
  } 
  else {
    if(toggleOuterStartupTime > millis() - startupDelayMillis){
      digitalWrite(pinLEDouter, (millis()%1000 < 500)?LOW:HIGH);
      masterChannelOuterOn = true;
    } 
    else {
      digitalWrite(pinLEDouter, HIGH);
      masterChannelOuterOn = false;
    }
  }

  if(brightnessInnerSmoothed > 0.001){
    digitalWrite(pinLEDinner, HIGH);
    masterChannelInnerOn = true;
  } 
  else {
    if(toggleInnerStartupTime > millis() - startupDelayMillis){
      digitalWrite(pinLEDinner, (millis()%1000 < 500)?LOW:HIGH);
      masterChannelInnerOn = true;
    } 
    else {
      digitalWrite(pinLEDinner, HIGH);
      masterChannelInnerOn = false;
    }
  }

  DmxMaster.write(masterChannelOuter, masterChannelOuterOn?255:0);
  DmxMaster.write(masterChannelInner, masterChannelInnerOn?255:0);

  DmxMaster.write(24, coldComponentOuter);
  DmxMaster.write(25, warmComponentOuter);

  DmxMaster.write(26, coldComponentOuter);
  DmxMaster.write(27, warmComponentOuter);

  DmxMaster.write(28, coldComponentOuter);
  DmxMaster.write(29, warmComponentOuter);

  DmxMaster.write(30, coldComponentOuter);
  DmxMaster.write(31, warmComponentOuter);

  DmxMaster.write(32, coldComponentInner);
  DmxMaster.write(33, warmComponentInner);

  DmxMaster.write(34, coldComponentInner);
  DmxMaster.write(35, warmComponentInner);

  DmxMaster.write(36, coldComponentInner);
  DmxMaster.write(37, warmComponentInner);

  DmxMaster.write(38, coldComponentInner);
  DmxMaster.write(39, warmComponentInner);

  //delay(10);

}

















