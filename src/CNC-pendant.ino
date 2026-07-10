// CNC pendant interface to Duet
// D Crocker, started 2020-05-04

/* This Arduino sketch can be run on either Arduino Nano or Arduino Pro Micro. 
 * It should alo work on an Arduino Uno (using the same wiring scheme as for the Nano) or Arduino Leonardo (using the same wiring scheme as for the Pro Micro).
 * The recommended board is the Arduino Pro Micro because the passthrough works without any modificatoins to the Arduino. 

*** Pendant to Arduino Pro Micro connections ***

Pro Micro Pendant   Wire colours
VCC       +5V       red
GND       0V,       black
          COM,      orange/black
          CN,       blue/black
          LED-      white/black

D2        A         green
D3        B         white
D4        X         yellow
D5        Y         yellow/black
D6        Z         brown
D7        4         brown/black
D8        5         powder (if present)
D9        6         powder/black (if present)
D10       LED+      green/black
A0        STOP      blue
A1        X1        grey
A2        X10       grey/black
A3        X100      orange

NC        /A,       violet
          /B        violet/black

*** Arduino Pro Micro to Duet PanelDue connector connections ***

Pro Micro Duet
VCC       +5V
GND       GND
TX1/D0    Through 6K8 resistor to URXD, also connect 10K resistor between URXD and GND

To connect a PanelDue as well:

PanelDue +5V to +5V/VCC
PanelDue GND to GND
PanelDue DIN to Duet UTXD or IO_0_OUT
PanelDue DOUT to /Pro Micro RX1/D0.

*** Pendant to Arduino Nano connections ***

Nano    Pendant   Wire colours
+5V     +5V       red
GND     0V,       black
        COM,      orange/black
        CN,       blue/black
        LED-      white/black

D2      A         green
D3      B         white
D4      X         yellow
D5      Y         yellow/black
D6      Z         brown
D7      4         brown/black
D8      5         powder (if present)
D9      6         powder/black (if present)
D10     X1        grey
D11     X10       grey/black
D12     X100      orange
D13     LED+      green/black
A0      STOP      blue

NC      /A,       violet
        /B        violet/black

*** Arduino Nano to Duet PanelDue connector connections ***

Nano    Duet
+5V     +5V
GND     GND
TX1/D0  Through 6K8 resistor to URXD, also connect 10K resistor between URXD and GND

To connect a PanelDue as well:

PanelDue +5V to +5V
PanelDue GND to GND
PanelDue DIN to Duet UTXD or IO_0_OUT
PanelDue DOUT to Nano/Pro Micro RX1/D0.

On the Arduino Nano is necessary to replace the 1K resistor between the USB interface chip by a 10K resistor so that PanelDiue can override the USB chip.
On Arduino Nano clones with CH340G chip, it is also necessary to remove the RxD LED or its series resistor.

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
const int PinStop = A0;

#if defined(__AVR_ATmega32U4__)     // Arduino Micro, Pro Micro or Leonardo
const int PinTimes1 = A1;
const int PinTimes10 = A2;
const int PinTimes100 = A3;
const int PinLed = 10;
#endif

#if defined(__AVR_ATmega328P__)     // Arduino Nano or Uno
const int PinTimes1 = 10;
const int PinTimes10 = 11;
const int PinTimes100 = 12;
const int PinLed = 13;
#endif


const unsigned long BaudRate = 57600;
const int PulsesPerClick = 4;
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
#include "GCodeSerial.h"
#include "PassThrough.h"

RotaryEncoder encoder(PinA, PinB, PulsesPerClick);
PassThrough passThrough;

int serialBufferSize;
int distanceMultiplier;
int axis;
uint32_t whenLastCommandSent = 0;

const int axisPins[] = { PinX, PinY, PinZ, PinAxis4, PinAxis5, PinAxis6 };
const int feedAmountPins[] = { PinTimes1, PinTimes10, PinTimes100 };

#if defined(__AVR_ATmega32U4__)     // Arduino Leonardo or Pro Micro
# define UartSerial   Serial1
#elif defined(__AVR_ATmega328P__)   // Arduino Uno or Nano
# define UartSerial   Serial
#endif

GCodeSerial output(UartSerial);

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
  pinMode(PinLed, OUTPUT);

  output.begin(BaudRate);

  serialBufferSize = output.availableForWrite();

#if defined(__AVR_ATmega32U4__)     // Arduino Leonardo or Pro Micro
  TX_RX_LED_INIT;
#endif
}

// Check for received data from PanelDue, store it in the pass through buffer, and send it if we have a complete command
void checkPassThrough()
{
  unsigned int commandLength = passThrough.Check(UartSerial);
  if (commandLength != 0 && UartSerial.availableForWrite() == serialBufferSize)
  {
    output.write(passThrough.GetCommand(), commandLength);
  }
}

void loop()
{
  // 0. Poll the encoder. Ideally we would do this in the tick ISR, but after all these years the Arduino core STILL doesn't let us hook it.
  // We could possibly use interrupts instead, but if the encoder suffers from contact bounce then that isn't a good idea.
  // In practice this loop executes fast enough that polling it here works well enough
  encoder.poll();

  // 1. Check for emergency stop
  if (digitalRead(PinStop) == HIGH)
  {
    // Send emergency stop command every 2 seconds
    do
    {
      output.write("M112 ;" "\xF0" "\x0F" "\n");
      digitalWrite(PinLed, LOW);
      uint16_t now = (uint16_t)millis();
      while (digitalRead(PinStop) == HIGH && (uint16_t)millis() - now < 2000)
      {
        checkPassThrough();
      }
      encoder.getChange();      // ignore any movement
    } while (digitalRead(PinStop) == HIGH);

    output.write("M999\n");
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
    ++localAxis;    
  }

  static uint32_t ledTimer = 0;
  static bool ledState = false;
  static uint32_t morseEndTime = 0;
  static bool morseActive = false;
  const uint32_t ledOnTime = 10;    // ms LED ON when axis selected
  const uint32_t ledOffTime = 50;   // ms LED OFF when axis selected
  const uint32_t morseOnTime = 10;   // ms LED ON for Morse mode (twice as fast)
  const uint32_t morseOffTime = 25; // ms LED OFF for Morse mode (twice as fast)
  uint32_t now = millis();

  if (morseActive && now < morseEndTime) {
    // Morse-like flashing
    if (ledState) {
      if (now - ledTimer >= morseOnTime) {
        digitalWrite(PinLed, LOW);
        ledState = false;
        ledTimer = now;
      }
    } else {
      if (now - ledTimer >= morseOffTime) {
        digitalWrite(PinLed, HIGH);
        ledState = true;
        ledTimer = now;
      }
    }
  } else {
    morseActive = false;
    if (axis == -1) {
      // No axis selected, keep LED ON
      digitalWrite(PinLed, HIGH);
      ledState = true;
      ledTimer = now;
    } else {
      // Axis selected, flicker LED using software PWM
      if (ledState) {
        if (now - ledTimer >= ledOnTime) {
          digitalWrite(PinLed, LOW);
          ledState = false;
          ledTimer = now;
        }
      } else {
        if (now - ledTimer >= ledOffTime) {
          digitalWrite(PinLed, HIGH);
          ledState = true;
          ledTimer = now;
        }
      }
    }
  }
  
  // 5. If the serial output buffer is empty, send a G0 command for the accumulated encoder motion.
  if (output.availableForWrite() == serialBufferSize)
  {
#if defined(__AVR_ATmega32U4__)     // Arduino Micro, Pro Micro or Leonardo
    TXLED1;                         // turn off transmit LED
#endif
    const uint32_t now = millis();
    if (now - whenLastCommandSent >= MinCommandInterval)
    {
      int distance = encoder.getChange() * distanceMultiplier;
      if (axis >= 0 && distance != 0)
      {
#if defined(__AVR_ATmega32U4__)     // Arduino Micro, Pro Micro or Leonardo
        TXLED0;                     // turn on transmit LED
#endif
        whenLastCommandSent = now;
        output.write(MoveCommands[axis]);
        if (distance < 0)
        {
          output.write('-');
          distance = -distance;
        }
        output.print(distance/10);
        output.write('.');
        output.print(distance % 10);
        output.write('\n');
        // Morse-like LED flash for 100ms after move command (adjust duration as needed)
        morseActive = true;
        morseEndTime = millis() + 100;
      }
    }
  }

  checkPassThrough();
}

// End
