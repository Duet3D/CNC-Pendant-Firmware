// CNC pendant interface to Duet
// D Crocker, started 2020-05-04

/* Pendant to Arduino Nano connections:

Nano    Pendant
+5V     +5V
GND     0V, COM, C

A       D2
B       D3
X       D4
Y       D5
Z       D6
4       D6
5       D8
6       D9
X1      D10
X10     D11
X100    D12
STOP    D13

Arduino Nano to Duet PanelDue connector connections:

Nano    Duet
+5V     +5V
GND     GND
TX1/D0  Through 5K6 resistor to URXD, also connect 10L resistor between URXD and GND
*/

// Configuration constants
const int PinA = 2;
const int PinB = 3;
const int PinX = 4;
const int PinY = 5;
const int PinZ = 6;
const int PinAxis4 = 7;
const int PinAxis5 = 8;
const int PinAxis6 = 9;
const int PinTimes1 = 10;
const int PinTimes10 = 11;
const int PinTimes100 = 12;
const int PinStop = 13;

const unsigned long BaudRate = 57600;
const int PulsesPerClick = 2;
const unsigned long MinCommandInterval = 20;

// Table of commands we send, one entry for each axis
const char* const MoveCommands[] =
{
  "G91 G0 F6000 X",     // X axis
  "G91 G0 F6000 Y",     // Y axis
  "G91 G0 F600 Z",      // Z axis
  "G91 G0 F6000 U",     // axis 4
  "G91 G0 F6000 V",     // axis 5
  "G91 G0 F6000 W"      // axis 6
};

#include "RotaryEncoder.h"

RotaryEncoder encoder(PinA, PinB, PulsesPerClick);

int serialBufferSize;
int distanceMultiplier;
int axis;
uint32_t whenLastCommandSent = 0;

const int axisPins[] = { PinX, PinY, PinZ, PinAxis4, PinAxis5, PinAxis6 };
const int feedAmountPins[] = { PinTimes1, PinTimes10, PinTimes100 };

void setup()
{
  pinMode(PinA, INPUT_PULLUP);
  pinMode(PinB, INPUT_PULLUP);
  pinMode(PinX, INPUT_PULLUP);
  pinMode(PinY, INPUT_PULLUP);
  pinMode(PinZ, INPUT_PULLUP);
  pinMode(PinAxis4, INPUT_PULLUP);
  pinMode(PinAxis5, INPUT_PULLUP);
  pinMode(PinAxis6, INPUT_PULLUP);
  pinMode(PinTimes1, INPUT_PULLUP);
  pinMode(PinTimes10, INPUT_PULLUP);
  pinMode(PinTimes100, INPUT_PULLUP);
  pinMode(PinStop, INPUT_PULLUP);

  Serial.begin(BaudRate);

  serialBufferSize = Serial.availableForWrite();
}

void loop()
{
  // 1. Check for emergency stop
  if (digitalRead(PinStop) == LOW)
  {
    Serial.write("M112 ;" "\xF0" "\x0F" "\n");
  }

  // 2. Poll the feed amount switch
  distanceMultiplier = 0;
  int localDistanceMultiplier = 1;
  for (int pin : feedAmountPins)
  {
    if (digitalRead(pin) == LOW)
    {
      distanceMultiplier = localDistanceMultiplier;
      break;
    }
    localDistanceMultiplier *= 10;
  }

  // 3. Poll the axis selector switch
  axis = -1;
  int localAxis = 0;
  for (int pin : axisPins)
  {
    if (digitalRead(pin) == LOW)
    {
      axis = localAxis;
      break;
    }
    ++axis;    
  }
  
  // 4. Poll the encoder. Ideally we would do this in the tick ISR, but after all these years the Arduino core STILL doesn't let us hook it.
  // We could possibly use interrupts instead, but if the encoder suffers from contact bounce then that isn't a good idea.
  encoder.poll();

  // 5. If the serial output buffer is empty, send a G0 command for the accumulated encoder motion.
  if (Serial.availableForWrite() == serialBufferSize)
  {
    const uint32_t now = millis();
    if (now - whenLastCommandSent >= MinCommandInterval)
    {
      int distance = encoder.getChange() * distanceMultiplier;
      if (axis >= 0 && distance != 0)
      {
        whenLastCommandSent = now;
        Serial.write(MoveCommands[axis]);
        Serial.print(distance);
        Serial.write('\n');
      }
    }
  }
}
