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
    PATTERN_CHANGE    = 1 << 0, //The pattern has changed.
    RECVD_NEW_PATTERN = 1 << 1, //Received new pattern
    TEARDOWN_COMPLETE = 1 << 2, //Teardown completed.
    SETUP_COMPLETE    = 1 << 3,    //Setup we asked for is done.
  };

enum {
    //DANGER!  This enum MUST be the same order as the Pattern array
    P_SOLID,    //Solid color
    P_RANDOMS,   //Random colors
    P_RAIN,     //Rain algorithm
    P_PRIDE
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

#ifdef CMDR
enum {
  COM_NONE = 0,
  COM_SERIAL= 1,
  COM_BLUE
};
extern byte COM_CHANNEL;
#include <SoftwareSerial.h>
extern SoftwareSerial BlueSerial;
extern int readNum(int &result);
extern void flushSerial();
extern void handleInputs();
extern bool UIisAvailable(byte channel);
extern byte UIRead();
#else
void RecvCallbackFunc(int Press);
void ReqCallbackFunc();
int receiveBytes(int num, byte* dest);
#endif

long makeLC (byte r, byte g, byte b)
{
  COLOR tmp;
  tmp.c[0]=r;
  tmp.c[1]=g;
  tmp.c[2]=b;
  return tmp.l;
}
#endif
