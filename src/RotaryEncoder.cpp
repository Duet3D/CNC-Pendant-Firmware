#include "RotaryEncoder.h"
#include "arduino.h"

void RotaryEncoder::init()
{
  pinMode(pin0, INPUT_PULLUP);
  pinMode(pin1, INPUT_PULLUP);
  change = 0;
  delay(10);                  // ensure we read the initial state correctly
  state = readState();
}

void RotaryEncoder::poll()
{
  // State transition table. Each entry has the following meaning:
  // 0 - the encoder hasn't moved
  // 1 - the encoder has moved 1 unit clockwise
  // -1 = the encoder has moved 1 unit anticlockwise
  static const int tbl[16] = 
  {  0, +1, -1, 0,    // position 3 = 00 to 11, can't really do anything, so 0
    -1,  0, -2, +1,   // position 2 = 01 to 10, assume it was a bounce and should be 01 -> 00 -> 10  
    +1, +2,  0, -1,   // position 1 = 10 to 01, assume it was a bounce and should be 10 -> 00 -> 01
     0, -1, +1, 0     // position 0 = 11 to 10, can't really do anything
  };
  const unsigned int t = readState();
  const int movement = tbl[(state << 2) | t];
  change += movement;
  state = t; 
}

int RotaryEncoder::getChange()
{
  int r;
  noInterrupts();
  if (change >= ppc)
  {
    r = change/ppc;
  }
  else if (change <= -ppc)
  {
    r = -((-change)/ppc);
  }
  else
  {
    r = 0;
  }
  change -= (r * ppc);
  interrupts();
  return r;
}

// End
