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
#include "ui.h"
#include "i2c_cmd.h"
#include <SoftwareSerial.h>  //Bluetooth
const int BT_TX = 4;        //Bluetooth transmit pin
const int BT_RX = 5;        //Bluetooth recieve pin
byte COM_CHANNEL = COM_NONE; //No communications active.

SoftwareSerial BlueSerial(BT_RX, BT_TX);
#else
#include "i2c_rcv.h"
#endif 

#include "display.h"  //Utilities to write to LED arrays.
#include "pattern.h"
#include "Rain.h"
#include "Solid.h"
#include "Randoms.h"
#include "Pride.h"
#include "render.h"   //The  algorithms that  set  the LED colors.

//Globals
const byte NUM_RECV=3;      //Number of receivers to address
const int NUM_LEDS=160;     //Num LEDS on strip 2x the raster len.
const int S_BUFF_LEN=80;    //Serial buffer length

byte pattern_id;
byte new_pattern_id;
byte loop_status = PATTERN_CHANGE;
char s_buff[S_BUFF_LEN];    //Serial buffer
int loop_delay=0;           //How long to display a frame.

//std::vector<Pattern> pattern;  MUST be same order as Pattern Enum in externs!
Pattern* pattern[]  = { new Solid(), new Randoms, new Rain, new Pride };
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
  //Wire.setClock(400000);   //4x speed boost but write good code first.
  BlueSerial.begin(9600);     //Only start bluetooth on commander.
  Serial.println("CMDR");
#else
  Wire.begin(RCVR_ID);                                // Join I2C bus as a listener
  Wire.onReceive(RecvCallbackFunc);                  // The commander commands us
  Wire.onRequest(ReqCallbackFunc);                   // The commander asks us to report.

  sprintf(s_buff,"RCVR %d",RCVR_ID);
  Serial.println(s_buff);
#endif

  pattern_id = new_pattern_id = P_SOLID;  //Start simple. 
  COM_CHANNEL=COM_NONE;
}


/*Main loop**************************************************************************************/
void loop() 
{
#ifdef CMDR
  handleInputs();  //Checks for user input.
#endif

  //New pattern_id may be changed by handleInputs() in cmdr or 
  if (new_pattern_id != pattern_id)
  {
    loop_status |= PATTERN_CHANGE;
  }

  //Don't do teardown on first run to avoid confusing receivers.
  if ( loop_status & PATTERN_CHANGE )
  {
    pattern[ pattern_id ]->teardown();
 
    pattern_id = new_pattern_id; //Switch to new pattern
//    Serial.println("do setup");
    pattern[ pattern_id ]->setup();
  }

#ifdef CMDR
  //Flush remaining input buffer for bluetooth and serial.
  flushUIBuffer();
#endif

  //Whatever this pattern is going to do to generate its display, do it.
  pattern [ pattern_id ]->render();

  //Send rendered frame to the LEDs
  raster_post();

  //Clear first run and pattern change status
  loop_status &= ~ PATTERN_CHANGE;
  COM_CHANNEL=COM_NONE;       //Clear com channel.
  delay(loop_delay);
}
