# CNC-Pendant-Firmware

This is firmware to run on an Arduino Nano to interface a populate style of wired CNC pendant to the PanelDue port of Duet electronics. Build it using Arduino. 

Pendant to Arduino Nano connections:

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
TX1/D0  Through 5K6 resistor to URXD, also connect 10K resistor between URXD and GND
