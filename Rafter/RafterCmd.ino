/*I2C Commander*/

#include <Wire.h> //Arduino library that enables I2C functionality
//#include <Vector>
#include "display.h"  //Utilities to write to LED arrays.
#include "pattern.h"
#include "Rain.h"
#include "RGBW.h"
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

byte pattern_id;
byte new_pattern_id;
byte loop_status = FIRST_RUN | PATTERN_CHANGE;

//std::vector<Pattern> pattern;
Pattern* pattern[] = { new Rain(), new Rain() };
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

  pattern_id = new_pattern_id = 1; //Set to rain. 
}

/*Main loop**************************************************************************************/
void loop() 
{
  
  static byte loop_status = FIRST_RUN | PATTERN_CHANGE;
  /*TODO 
  Patterns are children of class Pattern. 
  we must select a pattern and params here. 
  If the pattern is changed and this is not loop-step 0 pattern's teardown() is  run.
  The new pattern then gets to run its setup routine followed by next run
  Patterns control the message info they outside signalling info.
  */

  if ( loop_status & PATTERN_CHANGE & ~FIRST_RUN )
  {
    pattern[ pattern_id ]->teardown();
  }
  pattern_id = new_pattern_id;
 
  if ( loop_status & PATTERN_CHANGE )
  {
    pattern[ pattern_id ]->setup();  
  }
  
  pattern [ pattern_id ]->render();

  raster_post();
  
  loop_status &= ~ (FIRST_RUN|PATTERN_CHANGE);
}
