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
    Pride(char* tokens);
    void setup();
    void render();
    void teardown() {};
#ifdef CMDR
    long Fade(byte raw_index);
#else
    void receive(int num_bytes);
#endif
  public:
    COLOR mColors[6];
    byte  mNumCols;  //Number of colors in mColors (6)
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

Pride::Pride(char* tokens) : Pattern(tokens)
{
  mColors[0].l = makeLC(255,0,0);     //red
  mColors[1].l = makeLC(255,127,0);   //orange
  mColors[2].l = makeLC(255,255,0);   //yellow
  mColors[3].l = makeLC(0,255,0);     //green
  mColors[4].l = makeLC(0,0,255);     //blue
  mColors[5].l = makeLC(127,0,127);   //purple
  mNumCols = 6;
}

void Pride::setup()
{   
  int i;
  for(i=0;i<mNumCols;++i)

  loop_delay=20;   //Set master loop timing.
  loop_status &= ~SETUP_COMPLETE;
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
  //Serial.println(s_buff);

  //Each LED array contains two rows.
  c = NUM_LEDS/2;
  for (i=0;i<NUM_LEDS;++i)
  {
    if (i <= c)
    {
      pixel[i].l = a.l;
    }
    else
    {
      pixel[i].l = b.l;
    }
  }
}

#ifdef CMDR
//Generate crossfade a color to the next color
long Pride::Fade(byte raw_index)
{
  byte idx[2]; //Actual index generated via remainders.
  idx[0] = (raw_index) % mNumCols;
  idx[1] = (raw_index + 1) % mNumCols;
  
  float step_percent = mSteps/mStepNum;
  byte val;
  int c;
  COLOR output;
  
  for (c=0;c<3;++c) //Loop RGB
  {
    //Range is the absolute value between a and b.
    //Multiply by how far we are in completion of the steps
    //let our 'byte' type do the rounding for us (It's unsigned)
    val = step_percent * abs( mColors[idx[0]].c[c] - mColors[idx[1]].c[c] );

    if (mColors[idx[0]].c[c] < mColors[idx[1]].c[c])
    {
      output.c[c] = mColors[idx[0]].c[c] + val;
    }
    else
    {
      output.c[c] = mColors[idx[1]].c[c] + val;
    }       
  }
  return output.l;
}
#endif

void Pride::render()
{
  loop_delay = 1;
#ifdef CMDR
  int i;
  int c;
  
  c = mColIndex;

  /*To save memory, I'm using the messages color fields to get the fade 
   * values and pass them to the commander before I use msg to signal 
   * the receivers
   */
  struct MSG msg;
  
  msg.b.col[0].l = Fade(c++);
  msg.b.col[1].l = Fade(c++);
      
  setPixelColors(msg.b.col[0], msg.b.col[1]);

  for (i=1;i<=NUM_RECV;++i)
  {
    msg.h.id = pattern_id;
    msg.h.num = sizeof(struct BODY);
    
    msg.b.col[0].l = Fade(c++);
    msg.b.col[1].l = Fade(c++);

    sendTo(i, sizeof(struct MSG), (byte *)&msg );
  }

  raster_post(); //Look out!  Flipping LEDs here.
  do
  {
      delay(1);
  }  while (! askAllTrueMask( SETUP_COMPLETE ) );

  /*  Using % (remainder) here to create a rolling buffer. If we have 6 colors, 
   *   0%6 = 0, 1%6 = 1, .. ,6%6 = 0, 7%6 = 1, ...
   *   This saves us some ifs for range checking.
   */
  mStepNum = (mStepNum+1) % mSteps;
  if (!mStepNum)  //If step is 0, just moved colors.
  {
    mColIndex = (mColIndex+1) % 6; //Increment color index
  }
#endif
}

#ifdef RCVR
void Pride::receive(int num_bytes)
{
  loop_status &= ~SETUP_COMPLETE;
  struct BODY b;
  if (num_bytes != sizeof(b))
  {
    Serial.println("ERR");
  }
  receiveBytes(num_bytes, (byte *)&b);
  setPixelColors(b.col[0], b.col[1]);
  loop_status |= SETUP_COMPLETE;
}
#endif

#endif
