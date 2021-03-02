/*Solid.h
 * Set the lighting to a solid color.
 */
 
#ifndef SOLID_H
#define SOLID_H

#include "extern.h"
#include "pattern.h"

class Solid : public Pattern
{
  public:
    void setup();
    void render() {};
    void teardown() {};
    void receive(int num_bytes);
  public:
    COLOR mColor;
  private:
    struct BODY {
      COLOR col;
    } __attribute__((packed)); 
    
    struct MSG {
      HEADER h;
      struct BODY b;
    } __attribute__((packed));
 };

void Solid::setup()
{
  int i;
  int b;
  for(i=0; i<3; ++i)
  {
    readNum(b);
    mColor.c[i]=(byte)b;
    Serial.println(mColor.c[i]);
  }
  for(i=0;i<NUM_LEDS;++i)
  {
    pixel[i].l = mColor.l;
  }
  
#ifdef CMDR
  struct MSG msg;
  msg.h.id = P_SOLID;
  msg.h.num = sizeof(struct BODY);
  msg.b.col = mColor;
  
  sendAll( sizeof(struct MSG), (byte *)&msg );  //Notify all the receivers of the change.

#endif
}

void Solid::receive(int num_bytes)
{
#ifdef RCVR
  struct BODY b;
  if (num_bytes != sizeof(b))
  {
    Serial.println("ERR");
  }
  receiveBytes(num_bytes, (char *)&b);
  mColor.l = b.col.l;
#endif
}

#endif
