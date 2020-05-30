#include "PassThrough.h"

// Store character and add to checksum. If no room, we will discover that when we try to append the newline, so no need to record overflow here.
void PassThrough::StoreAndAddToChecksum(char c)
{
  actualChecksum ^= c;
  if (count < sizeof(buffer)/sizeof(buffer[0]))
  {
    buffer[count++] = c;
  }
}

// Get the command and reset the state
const char *PassThrough::GetCommand()
{
  state = State::waitingForStart;
  return buffer;
}

// Check for new data, return 0 if no command available, else length of the command
unsigned int PassThrough::Check(HardwareSerial& serial)
{
  while (state != State::haveCommand && serial.available() != 0)
  {
#if defined(__AVR_ATmega32U4__)     // Arduino Micro, Pro Micro or Leonardo
    RXLED0;                         // turn on receive LED
#endif
    const char c = serial.read();
#if defined(__AVR_ATmega32U4__)     // Arduino Micro, Pro Micro or Leonardo
    if (c == '\r' || c == '\n')
    {
      RXLED1;                       // turn off receive LED
    }
    else
    {
      RXLED0;                       // turn on receive LED
    }
#endif
    switch (state)
    {
      case State::waitingForStart:
        if (c == 'N')
        {
          state = State::receivingLineNumber;
          actualChecksum = c;
          count = 0;
        }
        break;

      case State::receivingLineNumber:
        if (c >= '0' && c <= '9')
        {
          StoreAndAddToChecksum(c);
          break;
        }
        state = State::receivingCommand;
        __attribute__ ((fallthrough));
      case State::receivingCommand:
        if (c == '*')
        {
          state = State::receivingChecksum;
          receivedChecksum = 0;
          break;
        }
        if (c == '\n' || c == '\r')
        {
          state = State::waitingForStart;
          break;
        }
        if (c == '"')
        {
          state = State::receivingQuotedString;
        }
        StoreAndAddToChecksum(c);
        break;

      case State::receivingQuotedString:
        if (c == '\n' || c == '\r')
        {
          state = State::waitingForStart;
          break;
        }
        if (c == '"')
        {
          state = State::receivingCommand;
        }
        StoreAndAddToChecksum(c);
        break;

      case State::receivingChecksum:
        if (c == '\n' || c == '\r')
        {
          if (count == sizeof(buffer)/sizeof(buffer[0]))    // if buffer too small for the command
          {
            state = State::waitingForStart;
            break;
          }
          buffer[count++] = '\n';
          state = (receivedChecksum == actualChecksum) ? State::haveCommand : State::waitingForStart;
        }
        else if (c >= '0' && c <= '9')
        {
          receivedChecksum = (10 * receivedChecksum) + (c - '0');
        }
        else
        {
          state = State::waitingForStart;
        }
        break;

      default:
        break;
    }
  }
  return (state == State::haveCommand) ? count : 0;
}

// End
