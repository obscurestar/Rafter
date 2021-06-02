/*Pride.h
 * Rolling rainbow crossfade
 * For fun, this one does synched movement, waiting for all recievers to 
 * ack that they have completed color config before continuing. 
 * transaction time is not guaranteed to be regular. 
 */
 
#ifndef PRIDE_H
#define PRIDE_H

#include "extern.h"
#include "pattern.h"

class Pride : public Pattern
{
  public:
    Pride();
    void setup();
    void render();
    void teardown() {};
#ifdef RCVR
    void receive(int num_bytes);
#endif
  public:
    COLOR mColors[6];
    byte  mColIndex; //Index into mColors array
    signed int   mStepNum;  //Index into steps
    signed int   mSteps; //Number of steps to transistion between colors.
  private:
    void setPixelColors(COLOR a, COLOR b);
    struct BODY {
      COLOR col[2];
    } __attribute__((packed)); 
    
    struct MSG {
      HEADER h;
      struct BODY b;
    } __attribute__((packed));
 };

Pride::Pride()
{
  //Doing mColor[0].c[0] = red_val; mColor[0].c[1] = grn_val; etc 
  //is too tedious. Bitshift for great justice!
  mColors[0].l = 255 << 24;  //Red
  mColors[1].l = (255 << 24) + (127 << 16); //Orange
  mColors[2].l = (255 << 24) + (255 << 16); //Yellow
  mColors[3].l = (255 << 16); //Green
  mColors[4].l = (255 << 8); //Blue
  mColors[5].l = (127 << 24) + (127 << 8); //Purple  
}

void Pride::setup()
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
  loop_delay=0;   //Set master loop timing.
#ifdef CMDR
  mColIndex = 0;
  mStepNum = 0;
  readNum(mSteps);
  if (mSteps == 0) mSteps=1;
#endif
}

//Receivers effectively render only once on receive so the 
//actual render logic has been moved out of render for this pattern.
void Pride::setPixelColors(COLOR a, COLOR b)
{
  int i,c;

  sprintf(s_buff,"RNDR: (%d %d %d), (%d %d %d)", a.c[0], a.c[1], a.c[2], b.c[0], b.c[1], b.c[2]);
  Serial.println(s_buff);
  
  c = NUM_LEDS/2;
  for (i=0;i<NUM_LEDS;++i)
  {
    if (i < c)
    {
      pixel[i].l = a.l;
    }
    else
    {
      pixel[i].l = b.l;
    }
  }
}

void Pride::render()
{
  delay(1);
#ifdef CMDR
  int i;
  byte ci[2];
  int c;
  byte val;
  float step_percent = mStepNum/mSteps;
  COLOR col[6];
  
  for(i=0;i<6;++i)  //Loop mColors
  {
    ci[0] = (mColIndex + i) % 6;
    ci[1] = (mColIndex + i + 1) % 6;
    for (c=0;c<3;++c) //Loop RGB
    {
      val = step_percent * abs( mColors[ci[0]].c[c] - mColors[ci[1]].c[c] );
      if (mColors[ci[0]].c[c] < mColors[ci[1]].c[c])
      {
        col[i].c[c] = mColors[ci[0]].c[c] + val;
      }
      else
      {
        col[i].c[c] = mColors[ci[1]].c[c] + val;
      }       
    }
  }

  setPixelColors(col[0], col[1]);

  for (i=0;i<=NUM_RECV;++i)
  {
    ci[0] = (i * 2) % 6;    //Which color to send to this receiver.
    ci[1] = (i * 2 + 1) % 6;
    
    struct MSG msg;
    msg.h.id = P_PRIDE;
    msg.h.num = sizeof(struct BODY);
    msg.b.col[0] = col[ci[0]];
    msg.b.col[1] = col[ci[1]];

    sendTo(i, sizeof(struct MSG), (byte *)&msg );
  }

  do
  {
      delay(1);
  }  while (! askAllTrueMask( SETUP_COMPLETE ) );
  
  mStepNum = (mStepNum+1) % mSteps;
  if (!mStepNum)  //If step is 0, just moved colors.
  {
    mColIndex = (mColIndex+1) % 6; //Increment color index
  }
#endif
}

#ifdef RCVR
void Solid::receive(int num_bytes)
{
  struct BODY b;
  if (num_bytes != sizeof(b))
  {
    Serial.println("ERR");
  }
  receiveBytes(num_bytes, (char *)&b);
  setPixelColors(b.col[0], b.col[1]);
}
#endif

#endif
