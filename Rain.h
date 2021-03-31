//rain.h
//Use random walks to do a palette shift.  

#ifndef RAIN_H  //These preprocessor directives let you include without worrying about dupes.
#define RAIN_H  //Once defined it doesn't need to be re-included.

#include "extern.h"
#include "i2c_cmd.h"
#include "pattern.h"

class Rain : public Pattern
{
  public:
    //The virtuals
    void setup();
    void render();
    void teardown() {};
#ifdef RCVR
    void receive(int num_bytes);
#endif

public:   //public variables.
  int mShiftOdds; //The 1 in n odds of picking a new hue mask.
public:   //public functions.
  Rain(); //Default constructor
private:
  byte extractByte(long lc, signed int which);
  byte pickHueMask();   //Get a new hue mask
  bool walkPixels();    //The math of the shimmer.
  
  struct BODY {
    byte hueMask;
    byte delayLen;
  } __attribute__((packed)); 
    
  struct MSG {
    HEADER h;
    struct BODY b;
  } __attribute__((packed));
#ifdef CMDR
  void sendHueChange();
#endif

private:  //class private variables
  byte mHueMask;  //Which bits are active on this iteration.
  bool mDirty;    //Set to true when any RGB not in current set is set for any pixel in chain.
  int mDelay;    //Just put this here to avoid namespace pollution.
};

Rain::Rain()
{
  mHueMask = 0; //Default to OFF.
  mShiftOdds=20;  //TODO  GET from ui.
  mDelay=0;
}

void Rain::setup()
{
  //TODO get shift odds from inputs
  //Setup message to  remotes.
  loop_status &= ~SIGNAL1;  //Set loop status to init.
  mDirty=false;
#ifdef CMDR
  mHueMask = pickHueMask();  //Calls sendAll()
  loop_delay=mDelay;
#endif
}

void Rain::render()  //Called from main loop().
{
  if (walkPixels()) //If false, still some 'old' colors.
  {
    loop_status |= SIGNAL1; //Prep to tell Cmdr we're ready to continue.
  }
  else
  {
    //The previous generation is dead.
    //Ask the receivers if their previous generation is also dead.
#ifdef CMDR
    if ( askAllCompleteMask( SIGNAL1 ) )
    {
      Serial.println("All Comp");
      if (!random(mShiftOdds))
      {
        Serial.println("Shifiting");
        mHueMask = pickHueMask();
      }
    }
#endif
  }
}

#ifdef RCVR
void Rain::receive(int num_bytes)
{
  struct BODY b;
  if (num_bytes != sizeof(b))
  {
    Serial.println("ERR");
  }
  receiveBytes(num_bytes, (char *)&b);
  mHueMask = b.hueMask;
  mDelay = b.delayLen;
  loop_delay = mDelay;
  loop_status &= ~SIGNAL1;  //Set loop status to init.
}
#endif

byte Rain::pickHueMask()
{
  //TODO update recievers at end of pick!
  //TODO pass in exclude_bit.
  byte exclude_bit = 2;
  
  byte cbits=random(6)+1;  //generate value from 1-6 this excludes 0(black) and 1(white)

  /*If the result is purely our exclude bit, let's make it a 1 in n chance that we decide to 
   * keep it.  Otherwise, just invert the byte selection.
   */
  if (cbits == exclude_bit && random(10) )
  {
    cbits = ~cbits;  // ~ is the complimentary operator 010 becomes 101 etc
  }

#ifdef CMDR
  sendHueChange();
#endif

  return cbits;
}

//Adafruit NeoPixel only gives us the long representing our color but we know the bytes in our
//color are RGBW so we can extract the individual bytes from the long like this and I'm leaving 
//this function as an example but truthfully, a union (see color.h) is more efficient. 
byte Rain::extractByte(long lc, signed int which)
{
  /*Bad error handling.  There are only 4 bytes.  signed int means we don't need to check the < 0 bound
   * but if someone requests byte 14 it's going to return 0.  Normally I'd throw an exception here.
   */
  
 if (which > 4) return 0;

 /* Alright, this return statement looks crazy.  Let's talk about it  First off, my blind use of constants.
  *  There are 8 bits in a byte (hence the 8) and 0xFF is just hex for 255 or 11111111 binary.  Which is 
  *  which byte of the long we want so:  We bitshift lc by the number of bits in a byte * number of bytes 
  *  to create an offset.  Then we mask it against 0xFF which eliminates values over 255.  Technically 
  *  we don't need to do the AND masking here because byte is an 8-bit word and it should auto-truncate
  *  due to return type but logical AND is a cheap operation and this makes me feel like my data is sanitized.
  */
 return ( (lc >> (8*which) & 0xFF) ); 
}

//Iterate through pixels and stagger around in the relative color space. 
bool Rain::walkPixels()
{
  mDirty=false;  //Set to true when any RGB not in current set is set for any pixel in chain.
  for (int p=0;p<NUM_LEDS; ++p)  //Loop through pixels.
  {
    for (int c=0;c<3;++c)     //Loop through RBG sub-pixels of each pixel.
    {
      COLOR pc;
      pc.l = pixel[p].l;
      
      if ( (mHueMask >> c) & 1 )
      {
          signed int val =pc.c[c] + (random(3) - 1); // rand result set [0,1,2] - 1 = [-1, 0, 1]
          if (val > 0 && val < 32)
          {
            pc.c[c] = val;  //Stagger around in the relative color space.
           pixel[p].l = pc.l;
          }
      }
      else
      {                       //Stagger towards 0, let iterator know this one doesn't count. 
        if (pc.c[c] > 0) //This RGB should not be set in this hue. Still draining previous color
        {
          mDirty=true;
          if (!random(6))
          {
            pc.c[c] --; //Wander slowly towards 0.
            pixel[p].l = pc.l;
          }
        }
      }
    }
  }
  return mDirty;
}

#ifdef CMDR
//Tell the receivers a new generation has begun.
void Rain::sendHueChange()
{
  //return;
  sprintf(s_buff,"Snd Hue: %d, dly: %d", mHueMask, mDelay);
  Serial.println(s_buff);
  struct MSG msg;
  msg.h.id = P_RAIN;
  msg.h.num = sizeof(struct BODY);
  msg.b.hueMask = mHueMask;
  msg.b.delayLen = mDelay;
  
  sendAll( sizeof(struct MSG), (byte *)&msg );  //Notify all the receivers of the change.
  delay(20); //SPATTERS DEBUG
}
#endif

#endif //RAIN_H
