#ifndef EXTERN_H
#define EXTERN_H

typedef union COLOR_T
{
  unsigned long l;
  unsigned char c[4];
} COLOR;

extern byte ID_NUM;  //This listener's ID number.
extern byte NEW_EVENT;           //new event detected
extern byte EVENT_COMPLETE;      //Event has been handled.
extern const int SDI;           //Serial PIN Red wire (not the red 5v wire) 
extern const int CKI;           //Clock PIN Green wire
extern const int NUM_LEDS;
extern COLOR pixel[];
extern COLOR recv_col;

void RecvCallbackFunc(int Press);
void ReqCallbackFunc();
#endif
