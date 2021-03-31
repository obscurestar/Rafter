/*Rafter Lighting system  1.0*/
#include <Wire.h> //Arduino library that enables I2C functionality

/********************CONFIGURATION BLOCK***************/
/* CMDR and RCVR_ID need to be set accordingly depending on which
 *  member of the system we're uploading code to.
 *  TODO: Could store the RCVR_ID in progmem and not require the 
 *  RCVR_ID config.
 */
#define CMDR /*Comment out this line for recievers*/

#ifndef CMDR
#define RCVR
/*If this unit is a receiver, set the RCVR_ID to a sequential number
 * starting with 1 as the first receiver.  0 appears to be reserved.
 */
byte RCVR_ID = 3;
#endif
/*****************END CONFIGURATION BLOCK**************/
const int SDI = 2;          //Serial PIN Red wire (not the red 5v wire) 
const int CKI = 3;          //Clock PIN Green wire

#ifdef CMDR
#include "i2c_cmd.h"
#else
#include "i2c_rcv.h"
#endif 

#include "display.h"  //Utilities to write to LED arrays.
#include "pattern.h"
#include "Rain.h"
#include "Solid.h"
#include "Randoms.h"
#include "render.h"   //The  algorithms that  set  the LED colors.

//Globals
const byte NUM_RECV=3;      //Number of receivers to address
const int NUM_LEDS=160;     //Num LEDS on strip 2x the raster len.
const int S_BUFF_LEN=80;    //Serial buffer length

byte pattern_id;
byte new_pattern_id;
byte loop_status = FIRST_RUN | PATTERN_CHANGE;
char s_buff[S_BUFF_LEN];    //Serial buffer
int loop_delay=0;           //How long to display a frame.

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

  Serial.begin(19200);

#ifdef CMDR
  Wire.begin(); // join I2C bus (address here is optional for master)
  //Wire.setClock(400000);   #4x speed boost but write good code first.
  
  Serial.println("CMDR");
#else
  Wire.begin(RCVR_ID);                                // Join I2C bus as a listener
  Wire.onReceive(RecvCallbackFunc);                  // The commander commands us
  Wire.onRequest(ReqCallbackFunc);                   // The commander asks us to report.

  sprintf(s_buff,"RCVR %d",RCVR_ID);
  Serial.println(s_buff);
#endif

  pattern_id = new_pattern_id = P_SOLID;  //Start simple. 
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
        loop_status |= PATTERN_CHANGE; //Force a change
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
#ifdef CMDR
  handleInputs();
#endif

  //New pattern_id may be changed by handleInputs() in cmdr or 
  if (new_pattern_id != pattern_id)
  {
    loop_status |= PATTERN_CHANGE;
  }

  //Don't do teardown on first run to avoid confusing receivers.
  if ( loop_status & PATTERN_CHANGE & ~FIRST_RUN )
  {
    pattern[ pattern_id ]->teardown();
  }

  if ( loop_status & PATTERN_CHANGE )
  {
    Serial.println("do setup");
    //Teardown complete we're now on the new pattern.
    //WARN potential bug here in recvrs if interrupt timing sucks.
    pattern_id = new_pattern_id;
 
    pattern[ pattern_id ]->setup();
  }

#ifdef CMDR
  flushSerial();   //Do this here.
#endif

  //Whatever this pattern is going to do to generate its display, do it.
  pattern [ pattern_id ]->render();

  //Send rendered frame to the LEDs
  raster_post();

  //Clear first run and pattern change status
  loop_status &= ~ (FIRST_RUN|PATTERN_CHANGE);
  delay(loop_delay);
}
