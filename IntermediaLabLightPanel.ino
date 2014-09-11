/**********************************************************************

 IntermediaLabLightPanel
 
 --- DMX Channel Overview ---
 
 01-31 Dimmers
 
       01-04 Dim A (Outer rig, outmost dimmer)
       05-08 Dim B (Outer rig, innermost dimmer)
       09-12 Dim C (Inner rig, outmost dimmer)
       13-16 Dim D (Inner rig, innermost dimmer)
        
 32-63 Relays / Switches
 
       Inner rig            Outer rig      |  Water 
                                           |  ~ ~   ~
       44-49   40-43        36-39   32-25  |   ~ ~ ~ ~
       D       C            B       A      |  ~ ~ ~
       Auto    Manual       Manual  Auto   |   ~ ~
                                           |  ~ ~   ~
 
 64-96 TV2000 Studio Led Floodlights

       Inner rig            Outer rig      |  Water 
                                           |  ~ ~   ~
       76-77   72-73        68-69   64-65  |   ~ ~ ~ ~
                                           |  ~ ~ ~
       78-79   74-75        70-71   66-76  |  ~  ~ ~ ~
                                           |   ~ ~
                                           
 --- MUX Shield ---
 
 M0 & M1 is input
 M2      is output
 
 --- MUX Panel patch ---
 
    panelRelayInnerOV1 > M0-15     M0-00 < panelRelayOuterOV1
    panelRelayInnerOH2 > M0-14  I  M0-01 < panelRelayOuterOH2   
    panelRelayInnerNV3 > M0-13  N  M0-02 < panelRelayOuterNV3
    panelRelayInnerNH4 > M0-12  P  M0-03 < panelRelayOuterNH4
                         M0-11  U  M0-04 
                         M0-10  T  M0-05  
                         M0-09     M0-06   
                         M0-08     M0-07  
                                           

 panelLightInnerToggle > M1-15     M1-00 < panelLightOuterToggle 
   panelLightInnerDown > M1-14  I  M1-01 < panelLightOuterDown
     panelLightInnerUp > M1-13  N  M1-02 < panelLightOuterUp
 panelLightInnerWarmer > M1-12  P  M1-03 < panelLightOuterWarmer
 panelLightInnerColder > M1-11  U  M1-04 < panelLightOuterColder
                         M1-10  T  M1-05   
                         M1-09     M1-06   
                         M1-08     M1-07   


    panelLightInnerLed < M2-15     M2-00 > panelLightOuterLed
                         M2-14  O  M2-01   
                         M2-13  U  M2-02   
                         M2-12  T  M2-03   
                         M2-11  P  M2-04   
                         M2-10  U  M2-05   
                         M2-09  T  M2-06   
                         M2-08     M2-07   


**********************************************************************/


#include <DmxMaster.h>

// MUX vars

// Give convenient names to the control pins
#define CONTROL0 11   
#define CONTROL1 10
#define CONTROL2 9
#define CONTROL3 8

// Create arrays for data from the the MUXs
int mux0array[16];
int mux1array[16];
int mux2array[16];


// DMX vars

int dmxOutPin = 3;
int dmxInPin = 4;
int manualRelayInnerStartChannel = 40;
int manualRelayOuterStartChannel = 36;
int ledStartChannel = 64;
int ledRelayChannelOuter = 32;
int ledRelayChannelInner = 44;
int maxDmxChannels = ledStartChannel + 16;


// LOGIC vars

int startupDelayMillis = 3000;
float touchStepSize = 0.25;
float smoothSpeedStartup = 0.005;
float smoothSpeedNormal = 0.03;

boolean ledRelayChannelOuterOn = false;
boolean ledRelayChannelInnerOn = false;

boolean manualRelayStatesInner[4];
boolean manualRelayStatesOuter[4];

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


// INT POINTERS to mux arrays for mapping variables to button panel inputs and outputs

int * panelRelayInnerOV1 = &mux0array[15];
int * panelRelayOuterOV1 = &mux0array[0];

int * panelRelayInnerOH2 = &mux0array[14];
int * panelRelayOuterOH2 = &mux0array[1];

int * panelRelayInnerNV3 = &mux0array[13];
int * panelRelayOuterNV3 = &mux0array[2];

int * panelRelayInnerNH4 = &mux0array[12];
int * panelRelayOuterNH4 = &mux0array[3];

int * panelLightInnerLed = &mux2array[15];
int * panelLightOuterLed = &mux2array[0];

int * panelLightInnerToggle = &mux1array[15];
int * panelLightOuterToggle = &mux1array[0];

int * panelLightInnerDown = &mux1array[14];
int * panelLightOuterDown = &mux1array[1];

int * panelLightInnerUp = &mux1array[13];
int * panelLightOuterUp = &mux1array[2];

int * panelLightInnerWarmer = &mux1array[12];
int * panelLightOuterWarmer = &mux1array[3];

int * panelLightInnerColder = &mux1array[11];
int * panelLightOuterColder = &mux1array[4];

// OTHER VARS

boolean panelRelayInnerOV1Former = LOW;
boolean panelRelayOuterOV1Former = LOW;
boolean panelRelayInnerOH2Former = LOW;
boolean panelRelayOuterOH2Former = LOW;
boolean panelRelayInnerNV3Former = LOW;
boolean panelRelayOuterNV3Former = LOW;
boolean panelRelayInnerNH4Former = LOW;
boolean panelRelayOuterNH4Former = LOW;

boolean panelLightInnerToggleFormer = HIGH;
long toggleInnerStartupTime = 0;

boolean panelLightOuterToggleFormer = HIGH;
long toggleOuterStartupTime = 0;


void setup() {
  
  // MUX SETUP
  // Set MUX control pins to output
  pinMode(CONTROL0, OUTPUT);
  pinMode(CONTROL1, OUTPUT);
  pinMode(CONTROL2, OUTPUT);
  pinMode(CONTROL3, OUTPUT);

  //Set analog pins to 2 digital inputs and 1 digital output
  pinMode(14, INPUT);         
  pinMode(15, INPUT);
  pinMode(16, OUTPUT);

  //Turn on pullup resistors
  digitalWrite(14, HIGH);       
  digitalWrite(15, HIGH);
  digitalWrite(16, LOW);

  //Initialise Arrays
  for (int i=0; i<16; i++)
  {
    mux0array[i] = LOW;
    mux1array[i] = LOW;
    mux2array[i] = HIGH;
  }
  
  for (int i=0;i<4;i++){
    manualRelayStatesInner[i] = LOW;
    manualRelayStatesOuter[i] = LOW;
  }

  /* The most common pin for DMX output is pin 3, which DmxMaster
   ** uses by default. If you need to change that, do it here. */
  DmxMaster.usePin(dmxOutPin);

  /* DMX devices typically need to receive a complete set of channels
   ** even if you only need to adjust the first channel. You can
   ** easily change the number of channels sent here. If you don't
   ** do this, DmxMaster will set the maximum channel number to the
   ** highest channel you DmxMaster.write() to. */
  DmxMaster.maxChannel(maxDmxChannels);

  //Open the serial port at 28800 bps
  //Serial.begin(28800);
}

void loop() {

  /*****************
  * MUX SHIELD I/O *
  ******************/
  
  //This for loop is used to scroll through and store the 16 inputs on the FIRST and SECOND multiplexer
  for (int i=0; i<16; i++)
  {
    muxSelect(i);
    //Read and store the input
    //Since internal pullup is on, the pin goes low on changing, so the value needs to be flipped from 0 to 1 (!)
    mux0array[i] = !digitalRead(14);
    mux1array[i] = !digitalRead(15);
  }
  
  //This for loop is used to scroll through the THIRD multiplexer
  for (int i=0; i<16; i++)
  {
    muxSelect(i);
    digitalWrite(16, mux2array[i]);
  }

  //The following lines are for printing out results of array0
  //Serial.print("mux0array: ");
  for (int i=0; i<16; i++)
  {
    //Serial.print(mux0array[i]);
    //Serial.print("-");
  }
  //Serial.println();  //line feed

  //The following lines are for printing out results of array1
  //Serial.print("mux1array: ");
  for (int i=0; i<16; i++)
  {
    //Serial.print(mux1array[i]);
    //Serial.print("-");
  }
  //Serial.println();

  /************************
  * REACT TO INPUT PANELS *
  ************************/
  
  // inner panel

  if(*panelRelayInnerOV1 == HIGH && panelRelayInnerOV1Former == LOW){
    manualRelayStatesInner[0] = !manualRelayStatesInner[0];
  }
  panelRelayInnerOV1Former = *panelRelayInnerOV1;
  
  if(*panelRelayInnerOH2 == HIGH && panelRelayInnerOH2Former == LOW){
    manualRelayStatesInner[1] = !manualRelayStatesInner[1];
  }
  panelRelayInnerOH2Former = *panelRelayInnerOH2;
  
  if(*panelRelayInnerNV3 == HIGH && panelRelayInnerNV3Former == LOW){
    manualRelayStatesInner[2] = !manualRelayStatesInner[2];
  }
  panelRelayInnerNV3Former = *panelRelayInnerNV3;
  
  if(*panelRelayInnerNH4 == HIGH && panelRelayInnerNH4Former == LOW){
    manualRelayStatesInner[3] = !manualRelayStatesInner[3];
  }
  panelRelayInnerNH4Former = *panelRelayInnerNH4;
  
  boolean innerState = *panelLightInnerToggle;
  if(innerState == HIGH && panelLightInnerToggleFormer == LOW){
    if(rawBrightnessInner == 0) {
      if(!ledRelayChannelInnerOn){
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
  panelLightInnerToggleFormer = innerState;
  
  if(*panelLightInnerUp == HIGH){
    if(*panelLightInnerDown == HIGH){
      rawBrightnessInner = rawBrightnessOuter;
    } 
    else {
      rawBrightnessInner+=touchStepSize;
      rawBrightnessInner = min(rawBrightnessInner, 255);
    }
  } 
  else if(*panelLightInnerDown == HIGH){
    rawBrightnessInner-=touchStepSize;
    rawBrightnessInner = max(rawBrightnessInner, 0);
  }

  if(*panelLightInnerWarmer == HIGH){
    if(*panelLightInnerColder == HIGH){
      rawTemperatureInner = rawTemperatureOuter;
    } 
    else {
      rawTemperatureInner-=touchStepSize;
      rawTemperatureInner = max(rawTemperatureInner, 0);
    }
  } 
  else if(*panelLightInnerColder == HIGH){
    rawTemperatureInner+=touchStepSize;
    rawTemperatureInner = min(rawTemperatureInner, 255);
  }


  // outer panel

  if(*panelRelayOuterOV1 == HIGH && panelRelayOuterOV1Former == LOW){
    manualRelayStatesOuter[0] = !manualRelayStatesOuter[0];
  }
  panelRelayOuterOV1Former = *panelRelayOuterOV1;
  
  if(*panelRelayOuterOH2 == HIGH && panelRelayOuterOH2Former == LOW){
    manualRelayStatesOuter[1] = !manualRelayStatesOuter[1];
  }
  panelRelayOuterOH2Former = *panelRelayOuterOH2;
  
  if(*panelRelayOuterNV3 == HIGH && panelRelayOuterNV3Former == LOW){
    manualRelayStatesOuter[2] = !manualRelayStatesOuter[2];
  }
  panelRelayOuterNV3Former = *panelRelayOuterNV3;
  
  if(*panelRelayOuterNH4 == HIGH && panelRelayOuterNH4Former == LOW){
    manualRelayStatesOuter[3] = !manualRelayStatesOuter[3];
  }
  panelRelayOuterNH4Former = *panelRelayOuterNH4;

  boolean outerState = *panelLightOuterToggle;

  if(outerState == HIGH && panelLightOuterToggleFormer == LOW){
    if(rawBrightnessOuter == 0) {
      if(!ledRelayChannelOuterOn){
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
  panelLightOuterToggleFormer = outerState;

  if(*panelLightOuterUp == HIGH){
    if(*panelLightOuterDown == HIGH){
      rawBrightnessOuter = rawBrightnessOuter;
    } 
    else {
      rawBrightnessOuter+=touchStepSize;
      rawBrightnessOuter = min(rawBrightnessOuter, 255);
    }
  } 
  else if(*panelLightOuterDown == HIGH){
    rawBrightnessOuter-=touchStepSize;
    rawBrightnessOuter = max(rawBrightnessOuter, 0);
  }

  if(*panelLightOuterWarmer == HIGH){
    if(*panelLightOuterColder == HIGH){
      rawTemperatureOuter = rawTemperatureOuter;
    } 
    else {
      rawTemperatureOuter-=touchStepSize;
      rawTemperatureOuter = max(rawTemperatureOuter, 0);
    }
  } 
  else if(*panelLightOuterColder == HIGH){
    rawTemperatureOuter+=touchStepSize;
    rawTemperatureOuter = min(rawTemperatureOuter, 255);
  }

  /*************************
  * UPDATE SMOOTHED VALUES *
  *************************/

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


  /*************************************
  * CALCULATE COLD/VARM CHANNEL VALUES *
  * from brightness and temperature    *
  * prioritising light output.         *
  *                                    *   
  *      ___                           *
  *     /                              *
  *    /     CW * brightness           *
  *   /                                *
  *                                    *   
  *          +                         *
  *   ___                              *
  *      \                             *
  *       \  WW * brightness           *
  *        \                           *
  *                                    *   
  *************************************/
  
  int coldComponentOuter = round(constrain(map(temperatureOuter, 0.0, 127.0, 0.0, 255.0), 0.0,255.0)*brightnessOuter/255.);
  int warmComponentOuter = round(constrain(map(temperatureOuter, 127.0, 255.0, 255.0, 0.0), 0.0,255.0)*brightnessOuter/255.);
  int coldComponentInner = round(constrain(map(temperatureInner, 0.0, 127.0, 0.0,255.0), 0.0,255.0)*brightnessInner/255.);
  int warmComponentInner = round(constrain(map(temperatureInner, 127.0, 255.0, 255.0, 0.0), 0.0,255.0)*brightnessInner/255.);

  /*******************
  * AUTOMATED RELAYS *
  *******************/

  // only turn on the master switches if there's some brightness
  
  if(brightnessOuterSmoothed > 0.001){
    *panelLightOuterLed = HIGH;
    ledRelayChannelOuterOn = true;
  } 
  else {
    if(toggleOuterStartupTime > millis() - startupDelayMillis){
      *panelLightOuterLed = ((millis()%1000 < 500)?LOW:HIGH);
      ledRelayChannelOuterOn = true;
    } 
    else {
      *panelLightOuterLed = HIGH;
      ledRelayChannelOuterOn = false;
    }
  }

  if(brightnessInnerSmoothed > 0.001){
    *panelLightInnerLed = HIGH;
    ledRelayChannelInnerOn = true;
  } 
  else {
    if(toggleInnerStartupTime > millis() - startupDelayMillis){
      *panelLightInnerLed = ((millis()%1000 < 500)?LOW:HIGH);
      ledRelayChannelInnerOn = true;
    } 
    else {
      *panelLightInnerLed = HIGH;
      ledRelayChannelInnerOn = false;
    }
  }

  /*************
  * DMX UPDATE *
  *************/

  // TV2000 Studio Led Floodlights
  
  DmxMaster.write(ledStartChannel, coldComponentOuter);
  DmxMaster.write(ledStartChannel+1, warmComponentOuter);

  DmxMaster.write(ledStartChannel+2, coldComponentOuter);
  DmxMaster.write(ledStartChannel+3, warmComponentOuter);

  DmxMaster.write(ledStartChannel+4, coldComponentOuter);
  DmxMaster.write(ledStartChannel+5, warmComponentOuter);

  DmxMaster.write(ledStartChannel+6, coldComponentOuter);
  DmxMaster.write(ledStartChannel+7, warmComponentOuter);

  DmxMaster.write(ledStartChannel+8, coldComponentInner);
  DmxMaster.write(ledStartChannel+9, warmComponentInner);

  DmxMaster.write(ledStartChannel+10, coldComponentInner);
  DmxMaster.write(ledStartChannel+11, warmComponentInner);

  DmxMaster.write(ledStartChannel+12, coldComponentInner);
  DmxMaster.write(ledStartChannel+13, warmComponentInner);

  DmxMaster.write(ledStartChannel+14, coldComponentInner);
  DmxMaster.write(ledStartChannel+15, warmComponentInner);

  // AUTOMATED RELAYS

  // TV2000 Studio Led Floodlights
  DmxMaster.write(ledRelayChannelOuter, ledRelayChannelOuterOn?255:0);
  DmxMaster.write(ledRelayChannelInner, ledRelayChannelInnerOn?255:0);

  // MANUAL RELAYS
  
  for(int i=0;i<4;i++){
    DmxMaster.write(manualRelayOuterStartChannel+i, manualRelayStatesOuter[i]?255:0);
    DmxMaster.write(manualRelayInnerStartChannel+i, manualRelayStatesInner[i]?255:0);
  }

}

void muxSelect(int i){
    //The following 4 commands set the correct logic for the control pins to select the desired input
    //See the Arduino Bitwise AND Reference: http://www.arduino.cc/en/Reference/BitwiseAnd
    //See the Aruino Bitshift Reference: http://www.arduino.cc/en/Reference/Bitshift
    digitalWrite(CONTROL0, (i&15)>>3); 
    digitalWrite(CONTROL1, (i&7)>>2);  
    digitalWrite(CONTROL2, (i&3)>>1);  
    digitalWrite(CONTROL3, (i&1));   
}











