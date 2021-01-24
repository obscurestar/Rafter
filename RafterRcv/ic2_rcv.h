/*I2C_rcv.h
 * Listen for orders and requests from the commander.
 */
#ifndef I2C_RCV_H
#define I2C_RCV_H

#include <I2C_Anything.h>
#include "extern.h"

/*Function/Event call****************************************************************************/
void RecvCallbackFunc(int Press)
{
  int i;
  NEW_EVENT=true;
  I2C_readAnything(recv_col);
}

void ReqCallbackFunc()
{
  Wire.write(EVENT_COMPLETE);
}

#endif
