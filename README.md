# TinyBlinky

This is a simple demo on how to use the TinyBlink board - it requires Arduino IDE and the following libs to compile:

*<EEPROM.h>*

*<Adafruit_NeoPixel.h>*

The examples shows how to respond to button clicks and how read the microphone. The button logic can identify a shot click and a longer press and hold:
- **click**: switches the color
- **press and hold**: adjusts the brightness

What the example apps do:
- **TinyBlink_demo.ino**: The analog reading from the microphone will animate the LEDs
- **whitepoint.ino**: Button click will change the whitepoint of the LEDs


---
The PCB is wired to use the following I/O pins from the ATtiny85:
- LED_PIN 1
- MIC_PIN A1
- BUTTON_PIN A3

The other pins are not connected but have seperate solder pads so you can extend the I/O capabilities
