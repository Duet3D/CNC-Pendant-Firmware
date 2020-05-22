# CNC-Pendant-Firmware

This is firmware to run on an Arduino Pro Micro (preferred) or Arduino Nano to interface a populate style of wired CNC pendant to the PanelDue port of Duet electronics. Build it using Arduino. 

Pendant to Arduino wiring:

```
Arduino Pendant   Wire colours
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
```

Arduino to Duet PanelDue connector wiring:
```
Nano    Duet
+5V     +5V
GND     GND
TX1/D0  Through 6K8 resistor to URXD, also connect 10K resistor between URXD and GND
```

To connect a PanelDue as well (the pendant passes the PanelDue commands through):
```
PanelDue +5V to +5V
PanelDue GND to GND
PanelDue DIN to Duet UTXD or IO_0_OUT
PanelDue DOUT to Arduino RXD, also 10K resistor from this junction to +5V
```

If using an Arduino Nano and using a PanelDue, it is necessary to replace the 1K resistor between the USB interface chip by a 10K resistor so that PanelDiue can override the USB chip. On Arduino Nano clones with CH340G chip, it is also necessary to remove the RxD LED or its series resistor. These changes are not necessary if using the Arduino Pro Micro.

