# MOSFET RGB Strip Wiring (ESP32-S2-Saola-1)

## GPIO → Physical Pins

| Color | GPIO | Board Location        |
|-------|------|-----------------------|
| R     | 16   | J2 pin 18             |
| G     | 17   | J2 pin 19             |
| B     | 18   | J3 pin 21              |

## Working circuit (per channel)

- **MOSFET:** IRFZ44N (N-channel), one per colour.
- **Gate series resistor:** 100 Ω between GPIO and gate (limits current, helps with ringing).
- **Gate pulldown:** 10 kΩ from gate to GND (keeps MOSFET off when GPIO is floating).

Repeat for R, G, B (3× MOSFET, 3× 100 Ω, 3× 10 kΩ).

## Checklist

1. **Common ground** – ESP32 GND must be connected to strip power supply GND.
2. **Power** – RGB strip has its own supply (5–12 V depending on strip). Do not power the strip from the ESP32.
3. **Wrong pins?** – In `main.cpp` change `PIN_R`, `PIN_G`, `PIN_B` to match your wiring.
4. **Inverted logic?** – If LEDs stay on when they should be off, set `INVERT_PWM = true`.
