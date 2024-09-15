# TinyBlink

Requires Arduino IDE and the following libs:

*<EEPROM.h>*

*<Adafruit_NeoPixel.h>*

This simple example shows how to respond to button clicks and how read the microphone. The button logic can identify a shot click and a longer press and hold:
- **click** switches the color
- **press and hold** adjusts the brightness
---
The PCB is wired to use the following I/O pins from the ATtiny85:
- LED_PIN 1
- MIC_PIN A1
- BUTTON_PIN A3
