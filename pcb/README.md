# Rantoge Replacement PCB

![preview](pcb.png)

- [Schematic](schematic.pdf)
- [Gerber](Gerber_Rantoge.zip)

## Parts
- 2 &times; DRV8825 "stepstick" module
- 2 &times; 6mmx6mmx11mm push button switch
- 5 &times; JST B5B-ZR (LF)(SN) 5-pin ZH series connectors
- MT3608 step up power supply module
- 100 uF capacitor
- Molex 1050170001 micro USB port
- B5819W SOD-123 Schottky diode

## Notes

- I expect that the original motherboard is 1mm thick. I ordered a 1.6mm PCB, and needed to trim down the nylon columns slightly to align the board with the openings in the case and to allow the L8 screws to reach.
- You'll need to set the current limit on the DRV8825s, and the adjustment screw is very senstive since it's capable of much more current than we need. I found the best way was to turn the screw all the way clockwise before connecting power, then start calibration of that motor and very slowly turn the screw anticlockwise until the motor starts to move.
- The diode is just to provide some protection if you connect power to both the board and the microcontroller. If you're careful not to do that, it's optional and you can just connect a wire between the pads.

## Pinouts

### HOUR, MINUTE
To stepper motors

### POWER
To MT3608 power supply module

|Pin|Connection|
|---|----------|
|1|9V (VOUT+)|
|2|GND (VOUT-)|
|3|not connected|
|4|GND (VIN-)|
|5|5V (VIN+)|

### P1
To microcontroller

|Pin|Connection|
|---|----------|
|1|Left button|
|2|Minute SLEEP|
|3|Minute STEP|
|4|Hour SLEEP|
|5|Hour STEP|


### P2
To microcontroller

|Pin|Connection|
|---|----------|
|1|Right button|
|2|5V (see below)|
|3|GND|
|4|GND|
|5|3.3V  in|

The board can be powered by providing 5V to pin 3 (but beware that the 5V pin on the microcontroller probably can't provide enough current - you'd need a separate power supply).
Alternatively, it can be powered from the USB port and it will provide 5V on pin 3 to power the microcontroller.