# Peltier Brick Controller

## Description

The Peltier Brick is a modular controller designed to operate two independent Peltier heating and cooling modules. Each channel uses an IR2113 gate driver together with IRF3205 MOSFETs to form a switching stage that allows bidirectional current flow. By reversing the current through the Peltier device, the system can provide both heating and cooling.

A microcontroller generates PWM signals to regulate the power delivered to each Peltier element and control the temperature of each channel independently.

# Repository Navigation

## Hardware Design

KiCad PCB design files and manufacturing outputs:

➡️ [Hardware](Hardware/)

---

## Datasheets and Design Notes

Power driver discussion and reference documentation:

➡️ [DataSheet](DataSheet/power_solutions.md)

---

## PCB Images

Rendered PCB images for quick viewing:

➡️ [PCB_Design](PCB_Design/)

---

## Circuit Schematics

Circuit schematic diagrams:

➡️ [Schematics](Schematics/)

---

## Source Attribution

Part of the design reference and documentation is derived from the MediBrick project.

Original repository:
https://github.com/MediBrick/MediBrick

Referenced document:
https://github.com/MediBrick/MediBrick/blob/main/power_solutions.md

This repository reorganizes the hardware design files and documentation for the **Peltier Heater–Cooler Brick controller**.

