/*Pattern.h
 * A base and sub classes for sending  patterns to the lighting system.
 */
 
#ifndef RGBW_H
#define RGBW_H

#include "extern.h"
#include "pattern.h"

class RGBS : public Pattern
{
  public:
    RGBS()
    {
      mColor.l  = 0;  //Set to black
      mOn=1;          //Num pix on followed by
      mOff=0;         //Num pix off.
    }
    virtual void setup() 
    {
      //TODO pick a color. This will depend on our UI. 
      mColor.c[0]=255;  //Set to red.
    }
    virtual void render()
    {
      int i=0;
      int j;
      while(i<NUM_LEDS)
      {
        for(j=0;j<mOn && i<NUM_LEDS;++j,++i)
        {
          pixel[i].l = mColor.l;
        }
        for(j=0;j<mOff && i<NUM_LEDS;++j,++i)
        {
          pixel[i].l = 0;
        }
      }
    }
    void teardown() {}
#ifdef  RCVR
    void receive() {}
#endif
  public:
    COLOR mColor; 
    byte mOn;
    byte mOff;  
};

#endif
