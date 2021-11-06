#include "GCodeSerial.h"

void GCodeSerial::begin(unsigned long baud)
{
  serial.begin(baud);
  lineNumber = 0;
  checksum = 0;
  emptyLine = true;
}

size_t GCodeSerial::write(uint8_t c)
{
  if (c == '\n')
  {
    if (!emptyLine)
    {
      serial.write('*');
      serial.print(checksum);
      emptyLine = true;
    }
  }
  else
  {
    if (emptyLine)
    {
      ++lineNumber;
      checksum = 0;
      emptyLine = false;        // do this first to avoid infinite recursion
      write('N');               // this recurses so that it updates the checksum
      print(lineNumber);        // this recurses so that it updates the checksum
    }
    checksum ^= c;
  }
  serial.write(c);
  return 1;
}

// End
