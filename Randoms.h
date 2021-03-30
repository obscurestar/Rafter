/*Randoms.h
 * A base and sub classes for sending  patterns to the lighting system.
 */
 
#ifndef RANDOMS_H
#define RANDOMS_H

#include "extern.h"
#include "pattern.h"

class Randoms : public Pattern
{
  public:
    void setup();
    void render();
    void teardown() {};
#ifdef RCVR
    void receive(int num_bytes){};
#endif
};

void Randoms::setup()
{
  HEADER h;
  h.id = P_RANDOMS;
  h.num=0;
#ifdef CMDR
  sendAll( sizeof(HEADER), (byte *)&h );
#endif
}

void Randoms::render()
{
  int i,j;

  for(i=0;i<NUM_LEDS;++i)
  {
    for(j=0; j<3; ++j)
    {
      pixel[i].c[j] = random(255);
    }
  }
}

#endif
