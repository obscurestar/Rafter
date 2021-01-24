/*I2C_Cmd.h
 * Command and poll the subprocessors
 */
#ifndef I2C_CMD_H
#define I2C_CMD_H

#include <I2C_Anything.h>
#include "extern.h"

void CommandAll() 
{
  byte i;
  signed char j=0;
  COLOR col;
  col.l = randomColor();

  /*Because the timing is easiest, display in reverse order.
   * Subprocessor 3 2 1 and commander last.
   */
  for(j=NUM_RECV-1;j>=0;j--)
  {
    //Serial.println(j);
    if (j==0)
    {
       setEvens(col);
       raster_post();
       //delay(500);
    }
    else
    {
      Wire.beginTransmission(j);
      i = I2C_writeAnything(col);
      //Serial.println(i);
      Wire.endTransmission();
      //delay(50);
    }
  }
}

void ReqAck() 
{
  byte i;
  signed char j=0;

  byte all_done=false;
  //char buff[100];
  for (j=1;j<NUM_RECV;++j)
  {
    Completed[j] = false;
  }
  while(!all_done)
  {
    all_done=true;
    //for(j=1;j<NUM_RECV;++j)
    for(j=NUM_RECV-1;j>0;j--)
    {
      if(!Completed[j])
      {
        //sprintf(buff, "Requesting from: %d", j);
        //Serial.println(buff);
        Wire.requestFrom(j, 1);
        while( Wire.available() )
        {
          I2C_readAnything(Completed[j]);
          //Completed[j] = Wire.read();
        }
        if ( !Completed[j] )
        {
          all_done=false;
        }
      }
    }
  }
}

#endif
