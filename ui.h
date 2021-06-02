/*ui.h
 * Routines related to getting input from the user.
 */
 
#ifndef UI_H
#define UI_H

#include "extern.h"

int readNum(int &result)
{
  int index=0;
  int dat;
  while (Serial.available() > 0 && index < (S_BUFF_LEN - 1))
  {
    dat=Serial.read();

    s_buff[index] = (unsigned char)dat;
    //s_buff[index] = Serial.read();
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

void flushSerial()
{
  while(Serial.available())
    s_buff[0] = Serial.read();
  return;
}

void handleInputs()
{
  char dat;
  if (Serial.available() > 0) {
    // read the incoming byte:
    dat = Serial.read();

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
        return;
    }
    sprintf(s_buff,"Pat: %c %d %d",dat, pattern_id, new_pattern_id);
    Serial.println(s_buff);
  }  
}

#endif UI_H
