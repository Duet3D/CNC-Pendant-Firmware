# CNC-Pendant-Firmware

This is firmware to run on an Arduino Pro Micro 5v/16MHz (https://www.sparkfun.com/products/12640) to connect a popular rotary encoder to the PanelDue port of Duet electronics. Build it using Arduino. 

60mm CNC Rotary Encoder handwheel to Arduino Pro Micro wiring:

```
ProMicro   Schematic
VCC        +5V encoder wheel
           +5V duet wifi
           +5V panel due

GND        0v encoder wheel
           GND duet wifi
           GND panel due       
           LED-
           STOP_BUTTON_0
           PAUSE_BUTTON_0
           ENABLE_SWITCH_0
           X1-100_SWITCH_COMMON
           XYZU_SWITCH_COMMON
           RESISTOR_DIVIDER_10k_GND

D2       A output of encoder wheel         
D3       B output of encoder wheel
D4       X output of XYZU_SWITCH 
D5       Y output of XYZU_SWITCH
D6       Z output of XYZU_SWITCH
D7       U output of XYZU_SWITCH
D10      LED+ (via 200 ohm resistor)
14       ENABLE_SWITCH_1
15       PAUSE_BUTTON_1
A0       STOP_BUTTON_1
A1       X1        grey
A2       X10       grey/black
A3       X100      orange

Not Connected       /A output of encoder wheel
                    /B output of encoder wheel
```

Arduino to Duet PanelDue connector wiring (3- or 4-core cable):
```
ProMicro  Duet
VCC       +5V
GND       GND
TX0    Through 6K8 resistor to URXD, also connect 10K resistor between URXD and GND
```

To connect a PanelDue as well (the pendant passes the PanelDue commands through):
```
PanelDue +5V to +5V/VCC
PanelDue GND to GND
PanelDue DIN to Duet UTXD
PanelDue DOUT to Arduino RX1
```

ENABLE switch disconnects all the signals between the pendant and the Duet Wifi (except the STOP button!)
All the buttons are NO (normally open), in order to use NC type of buttons you might want to replace all LOW->HIGH in the code dealing with these buttons
Button 'Pause' currently doesn't do anything useful, but will be programmed later.
5th position of the X-Y-Z-U axis switch currently duplicates the functionality of the ENABLE switch


Please, refer to the hardware/cnc_schematic.jpeg for wiring instructions and dimensions of some parts
hardware/cnc_pendant.step contains the 3d model for the case (https://www.thingiverse.com/thing:4559188)

Current firmware is designed to be compiled in PlatformIO (https://platformio.org/), install it and everything should work. If not - contact me.

It can also be compiled in the Arduino framework (https://www.arduino.cc/), in order to do that:
 1) Please rename src/CNC-pendant.cpp to src/CNC-pendant.ino
 2) Setup Arduino to work with Arduino Pro Micro properly, please refer to documentation here: https://www.sparkfun.com/products/12640 (especially, 'Documents' section!)
 3) Don't forget to select the proper board in Arduino UI before flashing it! 5v/16MHz!
 3) Sometimes you need to reset the board in order to flash it, you can use tweezers to connect RST to GND twice (this magic is from the 'Firmware Note' document, see above). 
 
Maybe it makes sense to replace the Arduino Pro Micro with ESP8266/ESP32 just because it is cheaper, what do you think?