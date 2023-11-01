#include "GCodeSerial.h"

void GCodeSerial::begin(unsigned long baud)
{
  serial.begin(baud);
  lineNumber = 0;
#if USE_CRC16
  crc.Reset(0);
#else
  checksum = 0;
#endif
  emptyLine = true;
}

size_t GCodeSerial::write(uint8_t c)
{
  if (c == '\n')
  {
    if (!emptyLine)
    {
      serial.write('*');
#if USE_CRC16
      // We must send the CRC as 5 digits so that it isn't mistaken for a checksum
      uint16_t crcVal = crc.Get();
      serial.write(crcVal/10000 + '0');
      crcVal %= 10000;
      serial.write(crcVal/1000 + '0');
      crcVal %= 1000;
      serial.write(crcVal/100 + '0');
      crcVal %= 100;
      serial.write(crcVal/10 + '0');
      serial.write(crcVal % 10 + '0');
#else
      serial.print(checksum);
#endif
      emptyLine = true;
    }
  }
  else
  {
    if (emptyLine)
    {
      ++lineNumber;
#if USE_CRC16
      crc.Reset(0);
#else
      checksum = 0;
#endif
      emptyLine = false;        // do this first to avoid infinite recursion
      write('N');               // this recurses so that it updates the checksum
      print(lineNumber);        // this recurses so that it updates the checksum
    }
#if USE_CRC16
    crc.Update(c);
#else
    checksum ^= c;
#endif
  }
  serial.write(c);
  return 1;
}

// End
