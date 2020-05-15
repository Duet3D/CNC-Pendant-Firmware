// Emergency Stop
const int ESTOP_NO = 6;                       // Green Black
const int ESTOP_NC = 7;                       // Silver Black
bool controllerStopped = false;

// Axis selector inputs
const int PIN_X = 17;                         // Brown
const int PIN_Y = 16;                         // Orange
const int PIN_Z = 15;                         // Light Blue
const int PIN_4 = 14;                         // Blue

// Step selector inputs
// PIN_X1 is not needed
const int PIN_X10 = 2;                        // Gray
const int PIN_X100 = 3;                       // Blue Black

// LED output
const int PIN_LED = 4;                        // Light Purple

// Dial encoder
const int PIN_A_PLUS = 20;                    // Yellow - ISR must be attachable to this!
const int PIN_B_PLUS = 21;                    // White

const unsigned int INPUT_MAX_TIME = 100;      // Time to wait for an input to be completed in ms
const int INPUT_MAX_STEPS = 10;               // Maximum number of steps before a new move command is sent

// Default feedrate for move commands
const int FEEDRATE = 6000;                    // in mm/min


char getAxis()
{
  return !digitalRead(PIN_X) ? 'X' :
            !digitalRead(PIN_Y) ? 'Y' :
              !digitalRead(PIN_Z) ? 'Z' :
                !digitalRead(PIN_4) ? 'A' : 0;
}

int getStepSize()
{
  return !digitalRead(PIN_X10) ? 10 :
           !digitalRead(PIN_X100) ? 50 : 1;
}

volatile int pulsesGenerated = 0;
volatile unsigned long lastPulseTime = 0;

void pulseISR(void *param)
{
  if (digitalRead(PIN_B_PLUS))
  {
    pulsesGenerated--;
  }
  else
  {
    pulsesGenerated++;
  }
  lastPulseTime = millis();
}

void setup()
{
  // Set up emergency stop
  pinMode(ESTOP_NO, INPUT_PULLUP);
  pinMode(ESTOP_NC, INPUT_PULLUP);
  
  // Set up axis selection
  pinMode(PIN_X, INPUT_PULLUP);
  pinMode(PIN_Y, INPUT_PULLUP);
  pinMode(PIN_Z, INPUT_PULLUP);
  pinMode(PIN_4, INPUT_PULLUP);

  // Set up step selector inputs
  pinMode(PIN_X10, INPUT_PULLUP);
  pinMode(PIN_X100, INPUT_PULLUP);
  
  // Set up dial encoder
  pinMode(PIN_A_PLUS, INPUT);
  pinMode(PIN_B_PLUS, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_A_PLUS), pulseISR, RISING);

  // Set up LED output
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, false);

  // Set up UART channels
  Serial.begin(115200);
  Serial1.begin(57600);
  Serial.write("Diabase pendant input controller board v1.0\n");
}

unsigned int lineNumber = 1;
void sendGCode(const char *code)
{
  unsigned int cs = 0;

  // Checksums require the line number as well
  char line[16];
  snprintf(line, 16, "N%u ", lineNumber++);
  for(size_t i = 0; line[i] != 0; i++)
  {
     cs = cs ^ line[i];
  }
  for(size_t i = 0; code[i] != '*' && code[i] != 0; i++)
  {
    cs = cs ^ code[i];
  }
  cs &= 0xff;  // Defensive programming...

  Serial1.print(line);
  Serial1.print(code);
  Serial1.print('*');
  Serial1.print(cs);
  Serial1.print('\n');
}

void loop()
{
  if (!digitalRead(ESTOP_NC))
  {
    if (!controllerStopped)
    {
      sendGCode("M112");
      controllerStopped = true;
    }
  }
  else if (controllerStopped && !digitalRead(ESTOP_NO))
  {
    sendGCode("M999");
    controllerStopped = false;
  }
  else if (lastPulseTime != 0 && millis() - lastPulseTime > INPUT_MAX_TIME || abs(pulsesGenerated) > INPUT_MAX_STEPS)
  {
    // See how far we need to move
    noInterrupts();
    const int pulses = pulsesGenerated;
    pulsesGenerated = 0;
    lastPulseTime = 0;
    interrupts();

    // Get selector values
    const char axis = getAxis();
    const int stepSize = getStepSize();

#ifdef DEBUG
    // Display command in the regular console
    if (axis == 0)
    {
      Serial.write("Nothing selected, ");
    }
    else
    {
      Serial.write(axis);
      Serial.write(" selected, ");
    }

	  Serial.write("x");
	  Serial.print(stepSize);
	  Serial.write(", ");
    
	  Serial.print(pulses);
	  Serial.write(" steps generated (");
	  Serial.print(pulses * stepSize / 100.0);
	  Serial.write(" mm to go)\n");
#endif

    if (axis != 0)
    {
      // Relative moves
      sendGCode("G91");
    
      // Generate and send G1 command to the firmware
      char command[32], moveAmount[16];
      dtostrf(pulses * stepSize / 100.0, 4, 2, moveAmount);
      snprintf(command, 32, "G1 %c%s F%d", axis, moveAmount, FEEDRATE);
      sendGCode(command);

      // Absolute moves
      sendGCode("G90");
    }
  }
  
  digitalWrite(PIN_LED, getAxis() != 0 && digitalRead(ESTOP_NC));
  delay(50);
}
