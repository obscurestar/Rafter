/*I2C_Cmd.h
 * Command and poll the subprocessors
 */
#ifdef CMDR
#ifndef I2C_CMD_H
#define I2C_CMD_H

#include "extern.h"

void sendAll(int msgLen, byte* msg)   //Sends the message to all receivers.
{
  //unsigned char *b = (unsigned char *)&a;
  byte rcv=0;
  for (rcv=1;rcv<=NUM_RECV;rcv++)
  {
    Wire.beginTransmission(rcv);
    Wire.beginTransmission(rcv);
    Wire.write(msg,msgLen);
    Wire.endTransmission();
  }
}

byte askAllCompleteMask( byte mask )
{
  //Until we need more sophisticated responses, let's assume a  bitmask is our ack
  //If all the flags in the mask are set on all the receivers return t else f.
  //This is a little inefficient. The first recvs get queried even  after they're done.
  byte rcv=0;

  for (rcv=1;rcv<=NUM_RECV;rcv++)
  {
    Wire.requestFrom((int)rcv, 1);
    while( Wire.available() )
    {
      if ( ! ( mask & Wire.read() ) ) 
        sprintf(s_buff,"Unit %d !comp", rcv);
        Serial.println(s_buff);
        return false;
    }
  }
  return true;
}

#endif
#endif
