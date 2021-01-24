/*Render.h
 * The  algorithms that generate the  patterns to  display.
 */
#ifndef RENDER_H
#define RENDER_H

#include "extern.h"

//Throws random colors down the strip array
long randomColor(void) {
  int x;
  COLOR colormap;
  int zc=0;
  
  for (x=0;x<3;++x)
  {
     colormap.c[random(3)] = random(55) + 200;
  }
  
  for (x=0;x<3;++x)
  {
    if (colormap.c[x] == 0)
    {
      zc ++;
    }
  }
  if (zc == 0)  //If white, kill it. 
  {
    colormap.c[random(3)] = 0;
  }
  
  return colormap.l;
}


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
