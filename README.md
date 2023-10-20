# CNC-Pendant-Firmware

This is firmware to run on an Arduino Pro Micro (preferred) or Arduino Nano to interface a popular style of wired CNC pendant to the PanelDue port of Duet electronics. Build it using Arduino IDE. 

For a full guide to building the pendant, see [the Duet3D wiki here](https://docs.duet3d.com/en/User_manual/Connecting_hardware/IO_CNC_Pendant).

## Wiring

Pendant to Arduino Pro Micro wiring:

| Pro Micro | Pendant | Wire colours |
|:----------|:--------|:-------------|
| VCC       | +5V     | red          |
| GND       | 0V<br>COM<br>CN<br>LED- | black<br>orange/black<br>blue/black<br>white/black | 
| D2        | A       | green        | 
| D3        | B       | white | 
| D4        | X       | yellow | 
| D5        | Y       | yellow/black | 
| D6        | Z       | brown | 
| D7        | 4       | brown/black | 
| D8        | 5       | pink (if present) | 
| D9        | 6       | pink/black (if present) | 
| D10       | LED+    | green/black | 
| A0        | STOP    | blue | 
| A1        | X1      | grey | 
| A2        | X10     | grey/black | 
| A3        | X100    | orange | 
| NC        | /A<br>/B | violet<br>violet/black | 

Arduino Pro Micro to Duet 3 IO_0 connector or Duet 2 PanelDue connector wiring (3- or 4-core cable):

| Pro Micro | Duet |
|:----------|:-----|
| VCC       | +5V (red wire) |
| GND       | GND (yellow wire) |
| TXO<br>GND | Through 6K8 resistor to IO_0_IN (Duet 3) or URXD0 (Duet 2)<br>Also connect 10K resistor between GND and IO_0_IN (Duet 3) or URXD0 (Duet 2) (blue wire from resistor junction to Duet) |

To connect a PanelDue as well (the Arduino Pro Micro passes the PanelDue commands through to the Duet):

| PanelDue | Pro Micro / Duet |
|:---------|:-----------------|
| +5V      | +5V/VCC (red wire to Ardiuno or Duet) |
| GND      | GND (yellow wire to Ardiuno or Duet) |
| DIN      | **Duet** IO_0_OUT (Duet 3) or UTXD0 (Duet 2) (green wire) |
| DOUT     | **Pro Micro** RXI (blue wire of PanelDue cable to green wire of pendant cable) |

For wiring differences and hardware changes needed if using an Arduino Nano, see the comments at the start of the CNC-pendant.ino file.

## Support requests

Please use the [forum](https://forum.duet3d.com) for support requests.
