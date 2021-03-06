/*I2C_rcv.h
 * Listen for orders and requests from the commander.
 */
#ifdef RCVR
#ifndef I2C_RCV_H
#define I2C_RCV_H

#include <Wire.h>
#include "extern.h"

int receiveBytes(int num, byte* dest)
{
  int i=0;
  byte c;
  while( Wire.available() )
  {
    if (i >= num) break;
    c = Wire.read();
    dest[i] = (byte)c;
    ++i;
  }
  return i;
}

/*Function/Event call****************************************************************************/
void RecvCallbackFunc(int Press)
{
  HEADER hdr;
  receiveBytes(sizeof(hdr), (byte *)&hdr);

  if (hdr.id != pattern_id) //The user requested a pattern change. 
  {
    //TODO clear some or all status here.
    loop_status |= RECVD_NEW_PATTERN;
    new_pattern_id  = hdr.id;
    sprintf(s_buff,"New pat: %d", new_pattern_id);
    Serial.println(s_buff);
  }
  pattern[ hdr.id ]->receive(hdr.num);
}


void ReqCallbackFunc()
{
  /*This function gets called as an interrupt whenever the commander 
   * wants status. 
   */
   /*Until the code is more sophisticated, this is a simple request 
    * for a the loop_status global variable. See enum for mask attributes.
    */
  Wire.write(loop_status);
  //TODO might want to clear some status here? 
}
#endif
#endif
