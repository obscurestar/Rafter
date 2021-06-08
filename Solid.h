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
    Solid(char* tokens):Pattern(tokens){};
    void setup();
    void render();
    void teardown() {};
#ifdef RCVR
    void receive(int num_bytes);
#endif
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
  loop_delay=0;   //Set master loop timing.
#ifdef CMDR
  int i;
  int b;
  for(i=0; i<3; ++i)
  {
    readNum(b);
    mColor.c[i]=(byte)b;
  }
  
  struct MSG msg;
  msg.h.id = pattern_id;
  msg.h.num = sizeof(struct BODY);
  msg.b.col = mColor;
//  sprintf(s_buff,"RGB: %d %d %d", mColor.c[0], mColor.c[1], mColor.c[2]);
//  Serial.println(s_buff);
  sendAll( sizeof(struct MSG), (byte *)&msg );  //Notify all the receivers of the change.

#endif
}

void Solid::render()
{
  int i;
  for(i=0;i<NUM_LEDS;++i)
  {
    pixel[i].l = mColor.l;
  }
}

#ifdef RCVR
void Solid::receive(int num_bytes)
{
  struct BODY b;
//  if (num_bytes != sizeof(b))
//  {
//    Serial.println("ERR");
//  }
  receiveBytes(num_bytes, (byte *)&b);
  mColor.l = b.col.l;
//  sprintf(s_buff,"RCVD RGB: %d %d %d", mColor.c[0], mColor.c[1], mColor.c[2]);
//  Serial.println(s_buff);
}
#endif

#endif
