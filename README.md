# CNC-Pendant-Firmware

This is firmware to run on an Arduino Pro Micro (preferred) or Arduino Nano to interface a popular style of wired CNC pendant to the PanelDue port of Duet electronics. Build it using Arduino. 

Pendant to Arduino Pro Micro wiring:

```
ProMicro Pendant   Wire colours
VCC      +5V       red
GND      0V,       black
         COM,      orange/black
         CN,       blue/black
         LED-      white/black

D2       A         green
D3       B         white
D4       X         yellow
D5       Y         yellow/black
D6       Z         brown
D7       4         brown/black
D8       5         powder (if present)
D9       6         powder/black (if present)
D10      LED+      green/black
A0       STOP      blue
A1       X1        grey
A2       X10       grey/black
A3       X100      orange

NC       /A,       violet
         /B        violet/black
```

Arduino to Duet PanelDue connector wiring (3- or 4-core cable):
```
ProMicro Duet
VCC      +5V (red wire)
GND      GND (yellow wire
TX1/D0   Through 6K8 resistor to URXD, also connect 10K resistor between URXD and GND (blue wire from resistor junction to Duet URXD0)
```

To connect a PanelDue as well (the pendant passes the PanelDue commands through):
```
PanelDue +5V to +5V/VCC (red wire)
PanelDue GND to GND (yellow wire)
PanelDue DIN to Duet UTXD or IO_0_OUT (green wire)
PanelDue DOUT to Arduino RX0 (blue wire of PanelDuet cable to green wire of pendant cable)
```

For wiring differences and hardware changes needed if using an Arduino Nano, see the comments at the start of the CNC-pendant.ino file.

