/*I2C Commander*/

#include <Wire.h> //Arduino library that enables I2C functionality
//#include <Vector>
#include "display.h"  //Utilities to write to LED arrays.
#include "pattern.h"
#include "Rain.h"
#include "Solid.h"
#include "Randoms.h"
#include "Render.h"   //The  algorithms that  set  the LED colors.

#ifdef RCVR
#include "ic2_rcv.h"
byte RCVR_ID = 0; //Change this ID for each reciever when uploading.
#endif 

//Globals
const byte NUM_RECV=3;      //Number of receivers to address
const int SDI = 2;          //Serial PIN Red wire (not the red 5v wire) 
const int CKI = 3;          //Clock PIN Green wire
const int NUM_LEDS=160;     //Num LEDS on strip 2x the raster len.
const int S_BUFF_LEN=80;    //Serial buffer length

byte pattern_id;
byte new_pattern_id;
byte loop_status = FIRST_RUN | PATTERN_CHANGE;
char s_buff[S_BUFF_LEN];    //Serial buffer

//std::vector<Pattern> pattern;  MUST be same order as Pattern Enum in externs!
Pattern* pattern[]  = { new Solid(), new Randoms, new Rain };
//pattern[P_RANDOMS] = new Randoms;
COLOR pixel[NUM_LEDS];      //The color state data for each pixel.

/*Setup loop************************************************************************************/
void setup() 
{
  pinMode(SDI, OUTPUT);
  pinMode(CKI, OUTPUT);
  
  //Make sure there is no light.
  memset(pixel,NUM_LEDS, sizeof(COLOR));
    
  randomSeed(analogRead(0));  //Seed randomizer by grabbing whatever noise is on analog pin 0 at the moment.
#ifdef CMDR
  Wire.begin(); // join I2C bus (address here is optional for master)
#else
  Wire.begin(RCVR_ID);                                // Join I2C bus as a listener
  Wire.onReceive(RecvCallbackFunc);                  // The commander commands us
  Wire.onRequest(ReqCallbackFunc);                   // The commander asks us to report.
#endif

  Serial.begin(19200);

#ifdef CMDR
  Serial.println("CMDR");
#else
  sprintf(s_buff,"RCVR %d",RCVR_ID);
  Serial.println(s_buff);
#endif
  pattern_id = new_pattern_id = P_RANDOMS;  //Start simple. 
}

int readNum(int &result)
{
  int index=0;
  int dat;
  while (Serial.available() > 0 && index < (S_BUFF_LEN - 1))
  {
    dat=Serial.read();

    s_buff[index] = (unsigned char)dat;
    //s_buff[index] = Serial.read();
    if ( isdigit(s_buff[index]) )
    {
      index++;
    }
    else
    {
      if (index > 0) break;
    }
  }
  s_buff[index] = '\0';
  result = atoi(s_buff);
  return index;
}

void flushSerial()
{
  while(Serial.available())
    s_buff[0] = Serial.read();
  return;
}

void handleInputs()
{
  char dat;
  if (Serial.available() > 0) {
    // read the incoming byte:
    dat = Serial.read();

    switch(dat){
      case 'R':   //Rain
        new_pattern_id = P_RAIN;
        break;
      case 'c':   //Solid color
        new_pattern_id = P_SOLID;
        break;
      case 'r':   //Random
        new_pattern_id = P_RANDOMS;
        break;
      case 'q':   //Moving rainbow
        break;
      case 'C':   //chase
        break;
      default:    //Bail out.
        return;

    }
    sprintf(s_buff,"Pat: %c %d %d",dat, pattern_id, new_pattern_id);
    Serial.println(s_buff);

  }  
}
/*Main loop**************************************************************************************/
void loop() 
{
  
  static byte loop_status = FIRST_RUN;

  handleInputs();
  
  if (new_pattern_id != pattern_id)
  {
    loop_status |= PATTERN_CHANGE;
    Serial.println("ch");
  }

  if ( loop_status & PATTERN_CHANGE & ~FIRST_RUN )
  {
    Serial.println("td");
    pattern[ pattern_id ]->teardown();
  }
  pattern_id = new_pattern_id;
 
  if ( loop_status & PATTERN_CHANGE )
  {
    Serial.println("su");
    pattern[ pattern_id ]->setup();  
  }
  
  flushSerial();   //Do this here.
  Serial.println("fl");
  pattern [ pattern_id ]->render();

  raster_post();
  
  loop_status &= ~ (FIRST_RUN|PATTERN_CHANGE);
}
