/*ui.h
 * Routines related to getting input from the user.
 * PLEASE NOTE:  This file is only included for the CMDR, not receivers.
 */
 
#ifndef UI_H
#define UI_H

#include "extern.h"

bool UIisAvailable(byte channel)
{
  if (channel==COM_NONE) return false;
  if (channel==COM_BLUE)
  {
    return  BlueSerial.available();
  }
  return Serial.available();
}

byte UIRead()
{
  if (COM_CHANNEL == COM_NONE) return 0;
  if (COM_CHANNEL == COM_SERIAL) return Serial.read();
  return BlueSerial.read();
}
int readNum(int &result)
{
  int index=0;
  int dat;
  while (UIisAvailable(COM_CHANNEL) > 0 && index < (S_BUFF_LEN - 1))
  {
    //dat=Serial.read();

    //s_buff[index] = (unsigned char)dat;
    s_buff[index] = UIRead();
    if ( isdigit(s_buff[index]) )
    {
      index++;
    }
    else
    {
      if (index > 0) break;
    }
  }
  s_buff[index] = '\0';
  result = atoi(s_buff);
  return index;
}

void flushUIBuffer()
{
  while(UIisAvailable(COM_CHANNEL))
    s_buff[0] = UIRead();
  COM_CHANNEL=COM_NONE;         //Input stream is voided, go back to neutral state.
  return;
}

bool handleInputsByType(byte channel)
{
  char dat;
  if (UIisAvailable(channel) > 0) {
    // read the incoming byte:
    COM_CHANNEL = channel;
    dat = UIRead();

    switch(dat){
      case 'R':   //Rain
        new_pattern_id = P_RAIN;
        break;
      case 'c':   //Solid color
        new_pattern_id = P_SOLID;
        loop_status |= PATTERN_CHANGE; //Force a change
        break;
      case 'r':   //Random
        new_pattern_id = P_RANDOMS;
        break;
      case 'P':   //Pride
      case 'p':
      case 'q':   //Moving rainbow
        new_pattern_id = P_PRIDE;
        break;
      case 'C':   //chase
        break;
      default:    //Bail out.
        return false;
    }
    sprintf(s_buff,"Pat: %c %d %d",dat, pattern_id, new_pattern_id);
    Serial.println(s_buff);
    return true;
  }
  return false;
}

void handleInputs()
{
  if (!handleInputsByType(COM_SERIAL))
  {
    handleInputsByType(COM_BLUE);
  }
}
#endif UI_H
