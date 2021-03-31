#ifndef EXTERN_H
#define EXTERN_H

#include "pattern.h"


typedef union COLOR_T
{
  unsigned long l;
  unsigned char c[4];
} COLOR;

typedef struct HEADER_S
{
  byte id;    //Pattern ID
  byte num;   //Length of message.
}  __attribute__((packed)) HEADER;

enum {
    FIRST_RUN      = 1 << 0, //This is the first run.
    PATTERN_CHANGE = 1 << 1, //The pattern has changed.
    SETUP_COMPLETE = 1 << 2, //Setup
    TEARDOWN_COMPLETE = 1 << 3, //Teardown completed.
    SIGNAL1 = 1 << 4,         //Signal defined by Pattern subclass.
  };

enum {
    //DANGER!  This enum MUST be the same order as the Pattern array
    P_SOLID,    //Solid color
    P_RANDOMS,   //Random colors
    P_RAIN
};

extern const byte NUM_RECV;     //Number of receivers to address
extern const int SDI;           //Serial PIN Red wire (not the red 5v wire) 
extern const int CKI;           //Clock PIN Green wire
extern const int NUM_LEDS;
extern COLOR pixel[];
extern byte Completed[];
extern Pattern* pattern[];
extern byte pattern_id;
extern byte new_pattern_id;
extern byte loop_status;
extern const int S_BUFF_LEN;
extern char s_buff[];
extern int loop_delay;

extern int readNum(int &result);

#ifdef RCVR
void RecvCallbackFunc(int Press);
void ReqCallbackFunc();
int receiveBytes(int num, byte* dest);
#endif

#endif
