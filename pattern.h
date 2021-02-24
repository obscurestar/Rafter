/*Pattern.h
 * A base and sub classes for sending  patterns to the lighting system.
 */
 
#ifndef PATTERN_H
#define PATTERN_H
#include "extern.h"

class Pattern
{
  public:
    virtual void setup() = 0;
    virtual void render() = 0;
    virtual void teardown() = 0;
#ifndef CMDR
    virtual void receive(int num) = 0;
#endif    
};


#endif
