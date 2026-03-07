# Peltier Brick Controller

## Description

The Peltier Brick is a modular controller designed to operate two independent Peltier heating and cooling modules. Each channel uses an IR2113 gate driver together with IRF3205 MOSFETs to form a switching stage that allows bidirectional current flow. By reversing the current through the Peltier device, the system can provide both heating and cooling.

A microcontroller generates PWM signals to regulate the power delivered to each Peltier element and control the temperature of each channel independently.

---

## Repository Structure

```
Peltier-design-clone
│
├── README.md
│
├── DataSheet
│   └── power_solutions.md
│
├── Hardware
│   └── Peltier_Controller_PCB
│        │
│        ├── SMD_PCB.kicad_pro
│        ├── SMD_PCB.kicad_sch
│        ├── SMD_PCB.kicad_pcb
│        │
│        ├── Gerber
│        ├── Drill
│        ├── BOM
│        │
│        └── SMD_PCB-all-pos.csv
│
├── PCB_Images
│   ├── Front.jpeg
│   └── Back.jpeg
│
└── Schematics
    └── Schematics.jpeg

---