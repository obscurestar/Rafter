/*Pattern.h
 * A base and sub classes for sending  patterns to the lighting system.
 */
 
#ifndef PATTERN_H
#define PATTERN_H
#include "extern.h"

class Pattern
{
  public:
    Pattern(char* tokens) : mTokens(tokens) {};
    virtual void setup() = 0;
    virtual void render() = 0;
    virtual void teardown() = 0;
#ifdef CMDR
    bool hasToken(char c) { return strchr(mTokens,c); }
#else
    virtual void receive(int num_bytes) = 0;
#endif

#ifdef CMDR
  public:
    char* mTokens;  //Tokens that match for UI.
#endif
};


#endif
