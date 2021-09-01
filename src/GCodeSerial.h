#ifndef GCODESERIAL_INCLUDED
#define GCODESERIAL_INCLUDED

#include "Arduino.h"

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
  uint8_t checksum;
  bool emptyLine;
};

#endif
