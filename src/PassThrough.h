#ifndef PASSTHROUGH_H_INCLUDED
#define PASSTHROUGH_H_INCLUDED

#include "Arduino.h"

// Class to accumulate commands received from PanelDue to be passed through
class PassThrough
{
public:
  unsigned int Check(HardwareSerial& serial);     // Check for new data, return 0 if no command available, else length of the command
  const char *GetCommand();                       // Get the command and reset the state
  
private:
  void StoreAndAddToChecksum(char c);             // Store character and add to checksum. If no room, set the overflow flag.
  // Pass through data
  enum class State : uint8_t
  {
    waitingForStart = 0,
    receivingLineNumber,
    receivingCommand,
    receivingQuotedString,
    receivingChecksum,
    haveCommand
  };

  uint8_t count = 0;
  uint8_t actualChecksum;
  uint8_t receivedChecksum;
  State state = State::waitingForStart;
  bool overflowed;
  char buffer[100];
};

#endif
