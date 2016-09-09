/**********************************************************************

  IntermediaLabLightPanel
  version 2.0

  --- DMX Channel Overview ---

  20-41   20 ELFORSK Spots

          21 - intensity
          22 - temperature


  101-149 12 ELFORSK Strips

          101 - varm white 1
          102 - cold white 1
          103 - varm white 2
          104 - cold white 2


  201 -   LED Plano Spots

          201 - red
          202 - green
          203 - blue
          204 - strobe


  251-    ETC Studio HD LED spots




  301-    8 OSRAM Kreios PAR 162W indoor LED spots

          PERSonality (channel mode) set to AR2.d

          301 - master dimmer
          302 - red
          303 - green
          304 - blue
          305 - green compensation
          306 - white


          Inner rig                Outer rig              |  Water
                                                          |  ~ ~   ~
          337-342   325-330        313-318   301-306      |   ~ ~ ~ ~
                                                          |  ~ ~ ~
          343-348   331-336        319-324   307-312      |  ~  ~ ~ ~
                                                          |   ~ ~

  401-431 4 Dimmers

          01-04 Dim A (Outer rig, outmost dimmer)
          05-08 Dim B (Outer rig, innermost dimmer)
          09-12 Dim C (Inner rig, outmost dimmer)
          13-16 Dim D (Inner rig, innermost dimmer)


  432-463 4 Relays / Switches

          Inner rig                Outer rig              |  Water
                                                          |  ~ ~   ~
          444-449   440-443        436-439   432-425      |   ~ ~ ~ ~
          D         C              B         A            |  ~ ~ ~
          Auto      Manual         Manual    Auto         |   ~ ~
                                                          |  ~ ~   ~

  464-496 8 TV2000 Studio Led Floodlights

          464- cold white
          465- varm white

          Inner rig                Outer rig              |  Water
                                                          |  ~ ~   ~
          476-477   472-473        468-469   464-465      |   ~ ~ ~ ~
                                                          |  ~ ~ ~
          478-479   474-475        470-471   466-476      |  ~  ~ ~ ~
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
  panelLightInnerColder > M1-12  P  M1-03 < panelLightOuterColder
  panelLightInnerWarmer > M1-11  U  M1-04 < panelLightOuterWarmer
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



//////////////////////////////////////////////////////////////////////

/* Perlin improved noise implementation for Arduino
   Fixed Point
   http://mrl.nyu.edu/~perlin/noise/INoise.java

   Changed the lookup tables with prerendered to store in Program memory.
   Otherwise Arduino has a memory shortage.

   copyleft kasperkamperman.com 15-12-2013
*/


#include <avr/pgmspace.h>

// permutation array
const PROGMEM char p[] = {
  151, 160, 137, 91, 90, 15,
  131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
  190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
  88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
  77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
  102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
  135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
  5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
  223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
  129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
  251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
  49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
  138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
};

// array below generated with this code line in the INoise code.
// static { for (int i=0; i < 256 ; i++) fade[i] = (int)((1<<12)*f(i/256.)); }
const PROGMEM uint16_t fadeArray[] = {
  0,    0,    0,    0,    0,    0,    0,    0,    1,    1,    2,    3,    3,    4,    6,    7,
  9,    10,   12,   14,   17,   19,   22,   25,   29,   32,   36,   40,   45,   49,   54,   60,
  65,   71,   77,   84,   91,   98,   105,  113,  121,  130,  139,  148,  158,  167,  178,  188,
  199,  211,  222,  234,  247,  259,  273,  286,  300,  314,  329,  344,  359,  374,  390,  407,
  424,  441,  458,  476,  494,  512,  531,  550,  570,  589,  609,  630,  651,  672,  693,  715,
  737,  759,  782,  805,  828,  851,  875,  899,  923,  948,  973,  998,  1023, 1049, 1074, 1100,
  1127, 1153, 1180, 1207, 1234, 1261, 1289, 1316, 1344, 1372, 1400, 1429, 1457, 1486, 1515, 1543,
  1572, 1602, 1631, 1660, 1690, 1719, 1749, 1778, 1808, 1838, 1868, 1898, 1928, 1958, 1988, 2018,
  2048, 2077, 2107, 2137, 2167, 2197, 2227, 2257, 2287, 2317, 2346, 2376, 2405, 2435, 2464, 2493,
  2523, 2552, 2580, 2609, 2638, 2666, 2695, 2723, 2751, 2779, 2806, 2834, 2861, 2888, 2915, 2942,
  2968, 2995, 3021, 3046, 3072, 3097, 3122, 3147, 3172, 3196, 3220, 3244, 3267, 3290, 3313, 3336,
  3358, 3380, 3402, 3423, 3444, 3465, 3486, 3506, 3525, 3545, 3564, 3583, 3601, 3619, 3637, 3654,
  3672, 3688, 3705, 3721, 3736, 3751, 3766, 3781, 3795, 3809, 3822, 3836, 3848, 3861, 3873, 3884,
  3896, 3907, 3917, 3928, 3937, 3947, 3956, 3965, 3974, 3982, 3990, 3997, 4004, 4011, 4018, 4024,
  4030, 4035, 4041, 4046, 4050, 4055, 4059, 4063, 4066, 4070, 4073, 4076, 4078, 4081, 4083, 4085,
  4086, 4088, 4089, 4091, 4092, 4092, 4093, 4094, 4094, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
};

unsigned long currentTime;     // for time measuring purposes
unsigned long passedTime;      // for time measuring purposes
unsigned long longestTime = 0; // for time measuring purposes

long perlinTimeInc = 200L;// 1000;
long perlinXInc    = 3471L;
long perlinYInc    = 1000L;

unsigned long perlinTimePosition = 0;

// returns a value between 0 - 255 for lights
int renderNoise(unsigned long x, unsigned long y, unsigned long z)
{ //return constrain(137 + inoise(x, y, z)>>9, 0, 255); // 127 + 10 zie Processing
  return 137 + (inoise(x, y, z) >> 9);
  //return 137 + (inoise(x, y, z)/512);
}

#define P(x) pgm_read_byte_near(p + ((x)&255))

long inoise(unsigned long x, unsigned long y, unsigned long z)
{
  long X = x >> 16 & 255, Y = y >> 16 & 255, Z = z >> 16 & 255, N = 1L << 16;
  x &= N - 1; y &= N - 1; z &= N - 1;

  long u = fade(x), v = fade(y), w = fade(z), A = P(X) + Y, AA = P(A) + Z, AB = P(A + 1) + Z, B = P(X + 1) + Y, BA = P(B) + Z, BB = P(B + 1) + Z;

  return lerp(w, lerp(v, lerp(u, grad(P(AA), x   , y   , z   ),
                              grad(P(BA), x - N , y   , z   )),
                      lerp(u, grad(P(AB), x   , y - N , z   ),
                           grad(P(BB), x - N , y - N , z   ))),
              lerp(v, lerp(u, grad(P(AA + 1), x   , y   , z - N ),
                           grad(P(BA + 1), x - N , y   , z - N )),
                   lerp(u, grad(P(AB + 1), x   , y - N , z - N ),
                        grad(P(BB + 1), x - N , y - N , z - N ))));
}

long lerp(long t, long a, long b) {
  return a + (t * (b - a) >> 12);
}

long grad(long hash, long x, long y, long z)
{ long h = hash & 15, u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

#define F(x) pgm_read_word_near(fadeArray + (x))

long fade(long t)
{
  long t0 = F(t >> 8), t1 = F(min(255, (t >> 8) + 1));
  return t0 + ( (t & 255) * (t1 - t0) >> 8 );
}


//////////////////////////////////////////////////////////////////////

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

// Channel vars

int manualRelayInnerStartChannel = 440;
int manualRelayOuterStartChannel = 436;
int tv2000StartChannel = 464;
int kreiosStartChannel = 301;
int autoRelayChannelOuter = 432;
int autoRelayChannelInner = 444;
int maxDmxChannels = tv2000StartChannel + 16;


// LOGIC vars

unsigned char op;

int startupDelayMillis = 3000;
float touchStepSize = 0.25;
float smoothSpeedStartup = 0.005;
float smoothSpeedNormal = 0.03;

boolean autoRelayChannelOuterOn = false;
boolean autoRelayChannelInnerOn = false;

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

int * panelLightInnerWarmer = &mux1array[11];
int * panelLightOuterWarmer = &mux1array[4];

int * panelLightInnerColder = &mux1array[12];
int * panelLightOuterColder = &mux1array[3];

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
  for (int i = 0; i < 16; i++)
  {
    mux0array[i] = LOW;
    mux1array[i] = LOW;
    mux2array[i] = HIGH;
  }

  for (int i = 0; i < 4; i++) {
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

  // while testing 200000 seems like a reasonable maximum
  // above that the precision gets less
  perlinTimePosition = 0; //200000;

  pinMode(13, OUTPUT);

}

void loop() {

  /*****************
    MUX SHIELD I/O
  ******************/

  //This for loop is used to scroll through and store the 16 inputs on the FIRST and SECOND multiplexer
  for (int i = 0; i < 16; i++)
  {
    muxSelect(i);
    //Read and store the input
    //Since internal pullup is on, the pin goes low on changing, so the value needs to be flipped from 0 to 1 (!)
    mux0array[i] = !digitalRead(14);
    mux1array[i] = !digitalRead(15);
  }

  //This for loop is used to scroll through the THIRD multiplexer
  for (int i = 0; i < 16; i++)
  {
    muxSelect(i);
    digitalWrite(16, mux2array[i]);
  }

  //The following lines are for printing out results of array0
  //Serial.print("mux0array: ");
  for (int i = 0; i < 16; i++)
  {
    //Serial.print(mux0array[i]);
    //Serial.print("-");
  }
  //Serial.println();  //line feed

  //The following lines are for printing out results of array1
  //Serial.print("mux1array: ");
  for (int i = 0; i < 16; i++)
  {
    //Serial.print(mux1array[i]);
    //Serial.print("-");
  }
  //Serial.println();

  /************************
    REACT TO INPUT PANELS
  ************************/

  // inner panel

  if (*panelRelayInnerOV1 == HIGH && panelRelayInnerOV1Former == LOW) {
    manualRelayStatesInner[0] = !manualRelayStatesInner[0];
  }
  panelRelayInnerOV1Former = *panelRelayInnerOV1;

  if (*panelRelayInnerOH2 == HIGH && panelRelayInnerOH2Former == LOW) {
    manualRelayStatesInner[1] = !manualRelayStatesInner[1];
  }
  panelRelayInnerOH2Former = *panelRelayInnerOH2;

  if (*panelRelayInnerNV3 == HIGH && panelRelayInnerNV3Former == LOW) {
    manualRelayStatesInner[2] = !manualRelayStatesInner[2];
  }
  panelRelayInnerNV3Former = *panelRelayInnerNV3;

  if (*panelRelayInnerNH4 == HIGH && panelRelayInnerNH4Former == LOW) {
    manualRelayStatesInner[3] = !manualRelayStatesInner[3];
  }
  panelRelayInnerNH4Former = *panelRelayInnerNH4;

  boolean innerState = *panelLightInnerToggle;
  if (innerState == HIGH && panelLightInnerToggleFormer == LOW) {
    if (rawBrightnessInner == 0) {
      if (!autoRelayChannelInnerOn) {
        toggleInnerStartupTime = millis() + startupDelayMillis;
      }
      brightnessInner = 0;
      temperatureInner = 0;
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

  if (*panelLightInnerUp == HIGH) {
    if (*panelLightInnerDown == HIGH) {
      rawBrightnessInner = rawBrightnessOuter;
    }
    else {
      rawBrightnessInner += touchStepSize;
      rawBrightnessInner = min(rawBrightnessInner, 255);
    }
  }
  else if (*panelLightInnerDown == HIGH) {
    rawBrightnessInner -= touchStepSize;
    rawBrightnessInner = max(rawBrightnessInner, 0);
  }

  if (*panelLightInnerWarmer == HIGH) {
    if (*panelLightInnerColder == HIGH) {
      rawTemperatureInner = rawTemperatureOuter;
    }
    else {
      rawTemperatureInner -= touchStepSize;
      rawTemperatureInner = max(rawTemperatureInner, 0);
    }
  }
  else if (*panelLightInnerColder == HIGH) {
    rawTemperatureInner += touchStepSize;
    rawTemperatureInner = min(rawTemperatureInner, 255);
  }

  // outer panel

  if (*panelRelayOuterOV1 == HIGH && panelRelayOuterOV1Former == LOW) {
    manualRelayStatesOuter[0] = !manualRelayStatesOuter[0];
  }
  panelRelayOuterOV1Former = *panelRelayOuterOV1;

  if (*panelRelayOuterOH2 == HIGH && panelRelayOuterOH2Former == LOW) {
    manualRelayStatesOuter[1] = !manualRelayStatesOuter[1];
  }
  panelRelayOuterOH2Former = *panelRelayOuterOH2;

  if (*panelRelayOuterNV3 == HIGH && panelRelayOuterNV3Former == LOW) {
    manualRelayStatesOuter[2] = !manualRelayStatesOuter[2];
  }
  panelRelayOuterNV3Former = *panelRelayOuterNV3;

  if (*panelRelayOuterNH4 == HIGH && panelRelayOuterNH4Former == LOW) {
    manualRelayStatesOuter[3] = !manualRelayStatesOuter[3];
  }
  panelRelayOuterNH4Former = *panelRelayOuterNH4;

  boolean outerState = *panelLightOuterToggle;

  if (outerState == HIGH && panelLightOuterToggleFormer == LOW) {
    if (rawBrightnessOuter == 0) {
      if (!autoRelayChannelOuterOn) {
        toggleOuterStartupTime = millis() + startupDelayMillis;
      }
      brightnessOuter = 0;
      temperatureOuter = 0;
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

  if (*panelLightOuterUp == HIGH) {
    if (*panelLightOuterDown == HIGH) {
      rawBrightnessOuter = rawBrightnessOuter;
    }
    else {
      rawBrightnessOuter += touchStepSize;
      rawBrightnessOuter = min(rawBrightnessOuter, 255);
    }
  }
  else if (*panelLightOuterDown == HIGH) {
    rawBrightnessOuter -= touchStepSize;
    rawBrightnessOuter = max(rawBrightnessOuter, 0);
  }

  if (*panelLightOuterWarmer == HIGH) {
    if (*panelLightOuterColder == HIGH) {
      rawTemperatureOuter = rawTemperatureOuter;
    }
    else {
      rawTemperatureOuter -= touchStepSize;
      rawTemperatureOuter = max(rawTemperatureOuter, 0);
    }
  }
  else if (*panelLightOuterColder == HIGH) {
    rawTemperatureOuter += touchStepSize;
    rawTemperatureOuter = min(rawTemperatureOuter, 255);
  }

  /*************************
    UPDATE SMOOTHED VALUES
  *************************/

  if (toggleInnerStartupTime < millis()) {
    float smoothSpeed = smoothSpeedNormal;
    brightnessInner *= 1.0 - smoothSpeed;
    brightnessInner += (smoothSpeed * rawBrightnessInner);
    brightnessInnerSmoothed *= 0.99;
    brightnessInnerSmoothed += brightnessInner * 0.01;
    temperatureInner *= 0.99;
    temperatureInner += 0.01 * rawTemperatureInner;
  }

  if (toggleOuterStartupTime < millis()) {
    float smoothSpeed = smoothSpeedNormal;
    brightnessOuter *= 1.0 - smoothSpeed;
    brightnessOuter += (smoothSpeed * rawBrightnessOuter);
    brightnessOuterSmoothed *= 0.99;
    brightnessOuterSmoothed += brightnessOuter * 0.01;
    temperatureOuter *= 0.99;
    temperatureOuter += 0.01 * rawTemperatureOuter;
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
    CALCULATE COLD/VARM CHANNEL VALUES
    from brightness and temperature
    prioritising light output.
  *                                    *
         ___
        /
       /     CW * brightness
      /
  *                                    *
             +
      ___
         \
          \  WW * brightness
           \
  *                                    *
  *************************************/

  int coldComponentOuter = round(constrain(map(temperatureOuter, 0.0, 127.0, 0.0, 255.0), 0.0, 255.0) * brightnessOuter / 255.);
  int warmComponentOuter = round(constrain(map(temperatureOuter, 127.0, 255.0, 255.0, 0.0), 0.0, 255.0) * brightnessOuter / 255.);
  int coldComponentInner = round(constrain(map(temperatureInner, 0.0, 127.0, 0.0, 255.0), 0.0, 255.0) * brightnessInner / 255.);
  int warmComponentInner = round(constrain(map(temperatureInner, 127.0, 255.0, 255.0, 0.0), 0.0, 255.0) * brightnessInner / 255.);

  /*******************
    AUTOMATED RELAYS
  *******************/

  // only turn on the master switches if there's some brightness

  if (brightnessOuterSmoothed > 0.001) {
    *panelLightOuterLed = HIGH;
    autoRelayChannelOuterOn = true;
  }
  else {
    if (toggleOuterStartupTime > millis() - startupDelayMillis) {
      *panelLightOuterLed = ((millis() % 1000 < 500) ? LOW : HIGH);
      autoRelayChannelOuterOn = true;
    }
    else {
      *panelLightOuterLed = HIGH;
      autoRelayChannelOuterOn = false;
    }
  }

  if (brightnessInnerSmoothed > 0.001) {
    *panelLightInnerLed = HIGH;
    autoRelayChannelInnerOn = true;
  }
  else {
    if (toggleInnerStartupTime > millis() - startupDelayMillis) {
      *panelLightInnerLed = ((millis() % 1000 < 500) ? LOW : HIGH);
      autoRelayChannelInnerOn = true;
    }
    else {
      *panelLightInnerLed = HIGH;
      autoRelayChannelInnerOn = false;
    }
  }

  /*************
    DMX UPDATE
  *************/

  // TV2000 Studio Led Floodlights

  DmxMaster.write(tv2000StartChannel, coldComponentOuter);
  DmxMaster.write(tv2000StartChannel + 1, warmComponentOuter);

  DmxMaster.write(tv2000StartChannel + 2, coldComponentOuter);
  DmxMaster.write(tv2000StartChannel + 3, warmComponentOuter);

  DmxMaster.write(tv2000StartChannel + 4, coldComponentOuter);
  DmxMaster.write(tv2000StartChannel + 5, warmComponentOuter);

  DmxMaster.write(tv2000StartChannel + 6, coldComponentOuter);
  DmxMaster.write(tv2000StartChannel + 7, warmComponentOuter);

  DmxMaster.write(tv2000StartChannel + 8, coldComponentInner);
  DmxMaster.write(tv2000StartChannel + 9, warmComponentInner);

  DmxMaster.write(tv2000StartChannel + 10, coldComponentInner);
  DmxMaster.write(tv2000StartChannel + 11, warmComponentInner);

  DmxMaster.write(tv2000StartChannel + 12, coldComponentInner);
  DmxMaster.write(tv2000StartChannel + 13, warmComponentInner);

  DmxMaster.write(tv2000StartChannel + 14, coldComponentInner);
  DmxMaster.write(tv2000StartChannel + 15, warmComponentInner);

  // Osram Kreios Par 162w indoor spots

  currentTime = micros(); // store current time

  // go a step further in time (input for y function in perlin noise)
  perlinTimePosition = perlinTimePosition + perlinTimeInc;

  // calculate the time the whole calculation took
  passedTime = micros() - currentTime;

  // because times will variate, remember the maximum time it took
  if (passedTime > longestTime) longestTime = passedTime;

  long startupTimer = millis() - startupDelayMillis;

  static long effectMaster = 0;
  if (toggleInnerStartupTime > startupTimer || toggleOuterStartupTime > startupTimer) {
      effectMaster++;
  } else {
      effectMaster--;
  }

  effectMaster = constrain(effectMaster, 0, 255);

  for (int i = 0; i < 8; i++) {
    int offset = 6 * i;
    DmxMaster.write(kreiosStartChannel + offset, effectMaster); // master
    DmxMaster.write(kreiosStartChannel + offset + 1, 0); // red
    DmxMaster.write(kreiosStartChannel + offset + 2, renderNoise(perlinXInc, i* perlinYInc, perlinTimePosition)/2); // green
    DmxMaster.write(kreiosStartChannel + offset + 3, renderNoise(33 * perlinXInc, i* perlinYInc, perlinTimePosition)); // blue
    DmxMaster.write(kreiosStartChannel + offset + 4, 0); // green comp
    DmxMaster.write(kreiosStartChannel + offset + 5, 0); // white
  }

  // AUTOMATED RELAYS

  // TV2000 Studio Led Floodlights
  DmxMaster.write(autoRelayChannelOuter, autoRelayChannelOuterOn ? 255 : 0);
  DmxMaster.write(autoRelayChannelInner, autoRelayChannelInnerOn ? 255 : 0);

  // MANUAL RELAYS

  for (int i = 0; i < 4; i++) {
    DmxMaster.write(manualRelayOuterStartChannel + i, manualRelayStatesOuter[i] ? 255 : 0);
    DmxMaster.write(manualRelayInnerStartChannel + i, manualRelayStatesInner[i] ? 255 : 0);
  }

  op ^= 1;
  digitalWrite(13, op & 1);  // toggle a pin so we can see loop rate


}

void muxSelect(int i) {
  //The following 4 commands set the correct logic for the control pins to select the desired input
  //See the Arduino Bitwise AND Reference: http://www.arduino.cc/en/Reference/BitwiseAnd
  //See the Aruino Bitshift Reference: http://www.arduino.cc/en/Reference/Bitshift
  digitalWrite(CONTROL0, (i & 15) >> 3);
  digitalWrite(CONTROL1, (i & 7) >> 2);
  digitalWrite(CONTROL2, (i & 3) >> 1);
  digitalWrite(CONTROL3, (i & 1));
}



