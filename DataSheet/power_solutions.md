# Power Control Solutions

## Breakout Boards
| Source | Link | Voltage | Amperage | Logic| Cost |
| --- |--- |--- |--- |--- | --- |
| Amazon L298N |[Link](https://a.co/d/1e84QhE)|  5-35V | 2A | 5V | $3.50
| Amazon BTS7960 | [Link](https://a.co/d/3osQzfE) | 6V-27V | 43A | 3.3-5V|  $7
| Amazon DRV8871 | [Link](https://a.co/d/hwcKjuL) | 6.5-45V| 3.6A | 1.8-5V |  $4.50
| Pololu Simple Motor Driver| [Link](https://www.pololu.com/product/1365)| 6.5-50V| 12A| USB |$135
| Pololu 8874 | [Link](https://www.pololu.com/product/4035) | 37V | 2.1A | 1.8-5V | $12
| Pololu 8876 | [Link](https://www.pololu.com/product/4036) | 37V | 1.3A | 1.8-5V | $9
| Sparkfun L298| [Link](https://www.sparkfun.com/sparkfun-ardumoto-motor-driver-shield.html) | 46V | 2A |3.3-5V| $25
| Sparkfun TB6612| [Link](https://www.sparkfun.com/tb6612-1-2a-dc-stepper-motor-driver-breakout-board.html)|  15V | 1.2A | 5V | retired
| Adafruit DRV8833| [Link](https://www.adafruit.com/product/3297) | 10V | 2A | | $6
| Adafruit DRV8871| [Link](https://www.adafruit.com/product/3190)| 6.5-45V | 3.6A| 1.8-5V | $7.50

## Discrete Builds

### Gate driver IR2113 and FET IRF3205
IRF3205 is *outdate design*

- requires 12V gate driver
- IRF3205 55V, 110A, 8mOhm, $1.45
- requires careful timing
- external current Senses
- allows for high switching rates

Optimizations with replacement parts

Replace FET with:

| Device | Voltage | Current | RDS ON | Price
| --- | --- | --- | --- | --- |
| IRLB3034 | 40V | 195-300A | 1.7mOhm | $2-4, end of life
| IRLB8743 | 30V | 150A | 3.2mOhm | $ 0.7-1.5 
| BSC010NE2LS | 25V |  100A | 1.1mOhm | $3-5
| AOI4184 | 40V | 40-50A| 7-8 mOhm | $0.7
| AOI518 | 30V | 54A | 8mOhm | obsolete
| BSC340N08 | 80V | 23A | 34mOhm | $1.30
| IPB017N10N5 | 100V |  120-180A| 1.5mOhm | $5-7
| IPB017N06N5 | 60V |  100-180A|  1.7mOhm | $6.25
| IPP034N06L3 | 30V | 80A | 3.4mOhm | $1-2 restricted availability
| STN3NF06L | 60V | 4A | 100mOhm | $1.59
| FQP30N06L | 60V | 32A | 35mOhm | $1.8
| onsemi RFP50N06| 60V | 50A | 22mOhm | $0.83
| NTE2996   | 60V | 84A  | 12mOhm | $2.85
| **TI CSD18532Q5B**| 60V | 100A | 3.2mOhm | $2.99 US $1.58 CHN
| **TI CSD18540Q5B**| 60V | 100A | 2.2mOhm | $3.05 US $1.51 CHN

Gate Drivers:

- UCC27211A 12V gate, simple no current sense, half bridge, $2.60
- IRS2186 10-20V gate, simple not current sense, half bridge $1.95
- MP6528 12V gate, full bridge, protection circuits, for 5-60V FET $2.51 (no stock JLPCP, in stock US)

- **Smart Gate Driver**, full bridge [DRV8701](https://www.ti.com/product/DRV8701) for 47V FET. $2.85 US, $0.59 CHN

- **Smart Gate Driver**, 2 x full bridge [DRV8704](https://www.ti.com/product/DRV8704) for 47 FET.  $4.87 US, $5.54 CHN

Also check out example design from Texas Instruments for [single H bridge](https://www.ti.com/tool/DRV8701EVM) and for [Dual H bridge](https://www.ti.com/tool/DRV8704EVM) with eval kit for $60.


### "Smart" H Bridge: Infinion BTS7960 / BTN7970 / BTN8982
*breakout board available*

builtin protection, simpler

half bridge:
- BTS7960 45V, 43A, 25kHz, 16mOhm, 5V logic, current sense, no stock china, USA
- BTN7960 45V, 44A, 25kHz, 30mOhm, 5V logic, current sense, no stock
- BTN7970 45V, 44A, 25kHz, 30mOhm, 5V logic, current sense, $5 obsolete
- **BTN8982** 40V, 44A, 25kHz, 20mOhm, 5V logic, current sense (newer) $3.82 CHN $5.6 US

Requires 3V to 5V logic buffer with TI SN74HCS244 to operate with 3.3V micro controllers

Also check out example design from infinion on [hackster](https://www.hackster.io/infineon/products/dc-motor-control-shield-with-btn8982ta?ref=project-0f6579)

P = I x R x I = 160*0.02 = 3.2 W heat @ 40A


### Integrated H Bridge DRV8874 / DRV8876 / DRV887x
*breakout board available*

Easy to use
builtin current limit or current sense
simple interface

- DRV8871 6.5 - 45V, 3.6A, protection, current limiter
- DRV8876 4.5 - 37V, 3.5A, 700mOhm, 1.8-5V logic, current sense output
- DRV8874 4.5 - 37V, 6A, 200mOhm, 1.8-5V logic, current sense output, $3 

P = I x R x I 36*0.2 = 7.2 W heat @ 6A

This is lower cost and fully integrated solution but thermal management at 6A will require careful design as it has high RDSON

### Half H Bridge DRV81XX
- DRV8145 4.5 - 35V, 46A, 16mOhm, 125kHz, $6.87, half bridge only
- DRV8144 4.5 - 35V, 30A, 24mOhm $5.50, half bridge only


## Temperature
ADS1220
 
> Adapted from the MediBrick project:  
> https://github.com/MediBrick/MediBrick/blob/main/power_solutions.md