/*Render.h
 * The  algorithms that generate the  patterns to  display.
 */
#ifndef RENDER_H
#define RENDER_H

#include "extern.h"

void renderRGBS(COLOR color)
{
  int i;
  int space=color.c[3] + 1;
  for(i=0;i<NUM_LEDS;i+=space)
  {
    pixel[i].l = color.l;
  }
}

#endif
