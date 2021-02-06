/*I2C Commander*/

#include <Wire.h> //Arduino library that enables I2C functionality
#include "display.h"  //Utilities to write to LED arrays.
#include "Pattern.h"
#include "Render.h"   //The  algorithms that  set  the LED colors.
#include "i2c_cmd.h"  //Command and poll the subprocessors.

//Globals
const byte NUM_RECV=4;      //Number of receivers to address
const int SDI = 2;          //Serial PIN Red wire (not the red 5v wire) 
const int CKI = 3;          //Clock PIN Green wire
const int NUM_LEDS=160;     //Num LEDS on strip 2x the raster len.

Pattern *pattern;

COLOR pixel[NUM_LEDS];      //The color state data for each pixel.
byte Completed[NUM_RECV];   //Status response buffer from subprocessors

/*Setup loop************************************************************************************/
void setup() 
{
  pinMode(SDI, OUTPUT);
  pinMode(CKI, OUTPUT);
  
  //Make sure there is no light.
  memset(pixel,NUM_LEDS, sizeof(COLOR));
    
  randomSeed(analogRead(0));  //Seed randomizer by grabbing whatever noise is on analog pin 0 at the moment.
  Wire.begin();                                                                         // join I2C bus (address here is optional for master)
  Serial.begin(19200);
  pattern = new RGBS();
}

/*Main loop**************************************************************************************/
void loop() 
{
  CommandAll();
  delay(50);
  ReqAck();
}
