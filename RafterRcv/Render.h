/*Render.h
 * The  algorithms that generate the  patterns to  display.
 */
#ifndef RENDER_H
#define RENDER_H

#include "extern.h"

void setEvens(COLOR color)
{
  int i;
  for(i=0;i<NUM_LEDS;i+=2)
  {
    pixel[i].l =color.l;
  }
}

#endif
