#ifndef EXTERN_H
#define EXTERN_H

typedef union COLOR_T
{
  unsigned long l;
  unsigned char c[4];
} COLOR;

extern const byte NUM_RECV;     //Number of receivers to address
extern const int SDI;           //Serial PIN Red wire (not the red 5v wire) 
extern const int CKI;           //Clock PIN Green wire
extern const int NUM_LEDS;
extern COLOR pixel[];
extern byte Completed[];
#endif
