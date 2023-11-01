#ifndef GCODESERIAL_INCLUDED
#define GCODESERIAL_INCLUDED

#include "Arduino.h"
#include "CRC16.h"

#define USE_CRC16   (1)     // 1 to append CRC16 to commands, 0 to append checksum

// Class to output to serial, adding line numbers and checksums
class GCodeSerial : public Print
{
public:
  GCodeSerial(HardwareSerial& device) : serial(device) { }

  void begin(unsigned long baud);
  size_t write(uint8_t) override;
  int availableForWrite() override { return serial.availableForWrite(); }
  using Print::write;

private:
  HardwareSerial& serial;
  uint16_t lineNumber;
#if USE_CRC16
  CRC16 crc;
#else
  uint8_t checksum;
#endif
  bool emptyLine;
};

#endif
