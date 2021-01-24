/*I2C Receiver*/

#include <Wire.h> // Arduino library that enables I2C functionality
#include  "ic2_rcv.h"
#include "display.h"  //Utilities to write to LED arrays.

#include "Render.h"   //The  algorithms that  set  the LED colors.

//Global vars
byte ID_NUM=1;  //Set for each listener
byte NEW_EVENT=true;                              //new color
byte EVENT_COMPLETE=false;
const int SDI = 2; //Serial PIN Red wire (not the red 5v wire) 
const int CKI = 3; //Clock PIN Green wire
const int NUM_LEDS=160;

COLOR pixel[NUM_LEDS];
COLOR recv_col;

/*Setup loop************************************************************************************/
void setup() 
{
  pinMode(SDI, OUTPUT);
  pinMode(CKI, OUTPUT);
  
  //Make sure there is no light.
  memset(pixel,NUM_LEDS, sizeof(COLOR));
    
  Wire.begin(ID_NUM);                                // Join I2C bus as a listener
  Wire.onReceive(RecvCallbackFunc);                  // The commander commands us
  Wire.onRequest(ReqCallbackFunc);                   // The commander asks us to report.
  Serial.begin(19200);
}
/*Main loop**************************************************************************************/
void loop() 
{
  if (NEW_EVENT)
  {
    EVENT_COMPLETE = false;
    renderRGBS(recv_col);
    raster_post();
    NEW_EVENT = false;
    EVENT_COMPLETE = true;
  }
  delay(10);
}
