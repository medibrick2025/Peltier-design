# GUI Module

This folder contains the GUI of the Peltier system.

The GUI has been developed and can be used for basic control and display of the system.

## DWIN Display Setup

The GUI is designed for a DWIN 7.0-inch display module (TTL interface).

To run the GUI on the display:

1. Copy the `DWIN_SET` folder to the root directory of a microSD card.
2. Insert the microSD card into the DWIN display module.
3. Power on the display.

The display will automatically detect the `DWIN_SET` folder and load the GUI project.

After initialization, disconnect the display from the power supply, remove the SD card then connect the display to the PCB and ESP the GUI will run independently on the display and communicate with the ESP32 via TTL serial interface. 

## Contents

- Arduino control code
- DGUS display files
- GUI-related resources