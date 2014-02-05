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

int pinBrightness1 = A0;
float brightness1 = 0;
int pinTemperature1 = A1;
int temperature1 = 255;

int pinBrightness2 = A2;
float brightness2 = 0;
int pinTemperature2 = A3;
int temperature2 = 0;

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
}

void loop() {

  brightness1 *= 0.95;
  brightness1 += max(0,(0.05*map(analogRead(pinBrightness1), 1020, 0, 0, 255)));
  brightness2 = brightness1;

  int coldComponent1 = round(min(1.0,map(temperature1, 0, 127, 0, 1.))*brightness1);
  int warmComponent1 = round(min(1.0,map(temperature1, 127, 255, 1., 0))*brightness1);
  int coldComponent2 = round(min(1.0,map(temperature2, 0, 127, 0,1.))*brightness2);
  int warmComponent2 = round(min(1.0,map(temperature2, 127, 255, 1., 0))*brightness2);

  // only turn on the master switches if there's some brightness
  DmxMaster.write(1, (brightness1 > 0)?255:0);
  DmxMaster.write(13, (brightness2 > 0)?255:0);

  DmxMaster.write(24, coldComponent1);
  DmxMaster.write(25, warmComponent1);

  DmxMaster.write(26, coldComponent1);
  DmxMaster.write(27, warmComponent1);

  DmxMaster.write(28, coldComponent1);
  DmxMaster.write(29, warmComponent1);

  DmxMaster.write(30, coldComponent1);
  DmxMaster.write(31, warmComponent1);

  DmxMaster.write(32, coldComponent2);
  DmxMaster.write(33, warmComponent2);

  DmxMaster.write(34, coldComponent2);
  DmxMaster.write(35, warmComponent2);

  DmxMaster.write(36, coldComponent2);
  DmxMaster.write(37, warmComponent2);

  DmxMaster.write(38, coldComponent2);
  DmxMaster.write(39, warmComponent2);

  //delay(10);

}



