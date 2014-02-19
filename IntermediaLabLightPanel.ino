/* Welcome to DmxMaster. This library allows you to control DMX stage and
 ** architectural lighting and visual effects easily from Arduino. DmxMaster
 ** is compatible with the Tinker.it! DMX shield and all known DIY Arduino
 ** DMX control circuits.
 **
 ** DmxMaster is available from: http://code.google.com/p/tinkerit/
 ** Help and support: http://groups.google.com/group/DmxMaster */

/* To use DmxMaster, you will need the following line. Arduino will
 ** auto-insert it if you select Sketch > Import Library > DmxMaster. */

#include <DmxMaster.h>

int startChannel = 24;

int masterChannel1 = 1;
int masterChannel2 = 13;

int rawBrightness1 = 0;
float brightness1 = 0;
float brightness1smoothed = 0;
int rawTemperature1 = 127;
float temperature1 = 127.;


int rawBrightness2 = 0;
float brightness2 = 0;
float brightness2smoothed = 0;
int rawTemperature2 = 127;
float temperature2 = 127.;

int pinLEDright = A0;
int pinLEDleft = A1;

int pinToggleRight = 13;
boolean pinToggleRightState = HIGH;
int pinToggleLeft = 8;
boolean pinToggleLeftState = HIGH;

int pinDownRight = 12;
int pinDownLeft = 7;

int pinUpRight = 11;
int pinUpLeft = A2;

int pinColderRight = 10;
int pinColderLeft = A3;

int pinWarmerRight = 9;
int pinWarmerLeft = A4;

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

  pinMode( pinLEDright, OUTPUT);
  pinMode( pinLEDleft, OUTPUT);

  pinMode( pinToggleRight, INPUT_PULLUP);
  pinMode( pinToggleLeft, INPUT_PULLUP);

  pinMode( pinDownRight, INPUT_PULLUP);
  pinMode( pinDownLeft, INPUT_PULLUP);

  pinMode( pinUpRight, INPUT_PULLUP);
  pinMode( pinUpLeft, INPUT_PULLUP);

  pinMode( pinColderRight, INPUT_PULLUP);
  pinMode( pinColderLeft, INPUT_PULLUP);

  pinMode( pinWarmerRight, INPUT_PULLUP);
  pinMode( pinWarmerLeft, INPUT_PULLUP);

  //Serial.begin(9600);

}

void loop() {

  boolean rightState = digitalRead(pinToggleRight);

  if(rightState == LOW && pinToggleRightState == HIGH){
    if(rawBrightness1 == 0) {
      rawBrightness1 = 255;
      rawTemperature1 = 127;
    } 
    else {
      rawBrightness1 = 0;
      rawTemperature1 = 0;
    } 
    //Serial.println("Toggle Right");
  }

  pinToggleRightState = rightState;

  if(digitalRead(pinUpRight) == LOW){
    if(digitalRead(pinDownRight) == LOW){
      rawBrightness1 = rawBrightness2;
    } 
    else {
      rawBrightness1++;
      rawBrightness1 = min(rawBrightness1, 255);
    }
  } 
  else if(digitalRead(pinDownRight) == LOW){
    rawBrightness1--;
    rawBrightness1 = max(rawBrightness1, 0);
  }

  if(digitalRead(pinWarmerRight) == LOW){
    if(digitalRead(pinColderRight) == LOW){
      rawTemperature1 = rawTemperature2;
    } 
    else {
      rawTemperature1--;
      rawTemperature1 = max(rawTemperature1, 0);
    }
  } 
  else if(digitalRead(pinColderRight) == LOW){
    rawTemperature1++;
    rawTemperature1 = min(rawTemperature1, 255);
  }

  boolean leftState = digitalRead(pinToggleLeft);

  if(leftState == LOW && pinToggleLeftState == HIGH){
    if(rawBrightness2 == 0) {
      rawBrightness2 = 255;
      rawTemperature2 = 127;
    } 
    else {
      rawBrightness2 = 0;
      rawTemperature2 = 0;
    } 
    //Serial.println("Toggle Left");
  }

  pinToggleLeftState = leftState;

  if(digitalRead(pinUpLeft) == LOW){
    if(digitalRead(pinDownLeft) == LOW){
      rawBrightness2 = rawBrightness1;
    } 
    else {
      rawBrightness2++;
      rawBrightness2 = min(rawBrightness2, 255);
    }
  } 
  else if(digitalRead(pinDownLeft) == LOW){
    rawBrightness2--;
    rawBrightness2 = max(rawBrightness2, 0);
  }

  if(digitalRead(pinWarmerLeft) == LOW){
    if(digitalRead(pinColderLeft) == LOW){
      rawTemperature2 = rawTemperature1;
    } 
    else {
      rawTemperature2--;
      rawTemperature2 = max(rawTemperature2, 0);
    }
  } 
  else if(digitalRead(pinColderLeft) == LOW){
    rawTemperature2++;
    rawTemperature2 = min(rawTemperature2, 255);
  }

  brightness1 *= 0.95;
  brightness1 += (0.05*rawBrightness1);

  brightness2 *= 0.95;
  brightness2 += (0.05*rawBrightness2);

  brightness1smoothed *= 0.99;
  brightness1smoothed += brightness1 * 0.01;

  brightness2smoothed *= 0.99;
  brightness2smoothed += brightness2 * 0.01;

  temperature1 *= 0.99;
  temperature1 += 0.01*rawTemperature1;

  temperature2 *= 0.99;
  temperature2 += 0.01*rawTemperature2;

  //Serial.print("\t B1: ");
  //Serial.print(brightness1);
  //Serial.print("\t T1: ");
  //Serial.print(temperature1);
  //Serial.print("\t B2: ");
  //Serial.print(brightness2);
  //Serial.print("\t T2: ");
  //Serial.println(temperature2);

  int coldComponentRight = round(constrain(map(temperature2, 0.0, 127.0, 0.0, 255.0), 0.0,255.0)*brightness2/255.);
  int warmComponentRight = round(constrain(map(temperature2, 127.0, 255.0, 255.0, 0.0), 0.0,255.0)*brightness2/255.);
  int coldComponentLeft = round(constrain(map(temperature1, 0.0, 127.0, 0.0,255.0), 0.0,255.0)*brightness1/255.);
  int warmComponentLeft = round(constrain(map(temperature1, 127.0, 255.0, 255.0, 0.0), 0.0,255.0)*brightness1/255.);

  // only turn on the master switches if there's some brightness

  if(brightness2smoothed > 0.001){
    digitalWrite(pinLEDright, LOW);
    DmxMaster.write(masterChannel1, 255);
  } 
  else {
    digitalWrite(pinLEDright, HIGH);
    DmxMaster.write(masterChannel1, 0);
  }

  if(brightness1smoothed > 0.001){
    digitalWrite(pinLEDleft, LOW);
    DmxMaster.write(masterChannel2,255);
  } 
  else {
    digitalWrite(pinLEDleft, HIGH);
    DmxMaster.write(masterChannel2, 0);
  }

  DmxMaster.write(24, coldComponentRight);
  DmxMaster.write(25, warmComponentRight);

  DmxMaster.write(26, coldComponentRight);
  DmxMaster.write(27, warmComponentRight);

  DmxMaster.write(28, coldComponentRight);
  DmxMaster.write(29, warmComponentRight);

  DmxMaster.write(30, coldComponentRight);
  DmxMaster.write(31, warmComponentRight);

  DmxMaster.write(32, coldComponentLeft);
  DmxMaster.write(33, warmComponentLeft);

  DmxMaster.write(34, coldComponentLeft);
  DmxMaster.write(35, warmComponentLeft);

  DmxMaster.write(36, coldComponentLeft);
  DmxMaster.write(37, warmComponentLeft);

  DmxMaster.write(38, coldComponentLeft);
  DmxMaster.write(39, warmComponentLeft);

  //delay(10);

}










