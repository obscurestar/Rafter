/*Display.h
 * These are the utilities for sending the pixel values to the physical array.
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include "extern.h"

void write_LED(int ID, long col)
{
  for(byte color_bit = 23 ; color_bit != 255 ; color_bit--)
  {
    //Feed color bit 23 first (red data MSB)
    
    digitalWrite(CKI, LOW); //Only change data when clock is low
    
    long mask = 1L << color_bit;
    //The 1'L' forces the 1 to start as a 32 bit number, otherwise it defaults to 16-bit.
    
    if(col & mask) 
      digitalWrite(SDI, HIGH);
    else
      digitalWrite(SDI, LOW);

    digitalWrite(CKI, HIGH); //Data is latched when clock goes high  
  }
}

//Takes the current strip color array and pushes it out.
//The second half of the array is reversed.
void raster_post (void) {
  //Each LED requires 24 bits of data
  //MSB: R7, R6, R5..., G7, G6..., B7, B6... B0 
  //Once the 24 bits have been delivered, the IC immediately relays these bits to its neighbor
  //Pulling the clock low for 500us or more causes the IC to post the data.

  int set_size = NUM_LEDS / 2;  //NOTE: Presumes even num leds.  
  int i;

  //Write the forward pixels
  for(i = 0; i < set_size ; ++i) 
  {
    write_LED(i, pixel[i].l); //24 bits of color data
  }
  
  //Write the backwards pixels
  for(i = 1; i <= set_size ; ++i) 
  {
    write_LED(i, pixel[NUM_LEDS - i].l); //24 bits of color data
  }
  
  //Pull clock low to put strip into reset/post mode
  digitalWrite(CKI, LOW);
  delayMicroseconds(750); //Wait for 500us to go into reset
}

#endif
