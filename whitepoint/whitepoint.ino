/*************************************************************************


    [2024] - WAVELIGHTS - Markus E. Loeffler
    All Rights Reserved.

*/
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

#define LED_COUNT 30
#define LED_COUNT_HALF (LED_COUNT / 2)
#define LED_PIN 1
#define BUTTON_PIN A3

Adafruit_NeoPixel ledStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint8_t gColorEEPROM;
uint8_t gBrightEEPROM;
uint8_t gBright2;

uint16_t gButtonState = 0;  // Current debounce status
bool gWasLongPress = false;


// Service routine called by a timer interrupt
bool DebounceSwitch()
{
  gButtonState = (gButtonState << 1) | (digitalRead(BUTTON_PIN) == LOW ? 1 : 0) | 0xe000;
  if (gButtonState == 0xffff) {
    gBright2 = (gBright2 + 1) % 256;
    gBrightEEPROM = gBright2 > 127 ? 255 - gBright2 : gBright2;
    gWasLongPress = true;
    uint8_t pressVal = gBrightEEPROM * LED_COUNT / 127;
    for (uint8_t c = 0; c < LED_COUNT; ++c)
      ledStrip.setPixelColor(c, pressVal >= c ? 0x0000ff00 : 0);
  }
  if (gButtonState == 0xf000)
    return true;
  else
    return false;
}


void setup()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  ledStrip.begin();

  // color and brightsness are stored in EEPROM
  gColorEEPROM = eeprom_read_byte(0);
  gBrightEEPROM = eeprom_read_byte(1);
  gBright2 = gBrightEEPROM;
}


void loop() {

  //------------------------- set global LED brightness

  uint16_t inVal = gBrightEEPROM;
  inVal *= inVal;
  inVal = inVal / 67;
  ledStrip.setBrightness(inVal + 14);

  //------------------------- paint LEDs

  uint32_t color;
  switch (gColorEEPROM % 5)
  {
    case 0:
      color = ledStrip.ColorHSV( 0, 0, 255);
      break;
    case 1:
      color = ledStrip.ColorHSV( 5140, 63, 255);
      break;
    case 2:
      color = ledStrip.ColorHSV( 5140, 127, 255);
      break;
    case 3:
      color = ledStrip.ColorHSV( 5140, 191, 255);
      break;
    default:
      color = ledStrip.ColorHSV( 5140, 230, 255);
  }
  ledStrip.fill(color);

  //------------------------- read button

  bool btn = DebounceSwitch();
  if (btn) {
    if (gWasLongPress) {
      gWasLongPress = false;
      eeprom_write_byte((uint8_t*)1, gBrightEEPROM);
    } else {
      ++gColorEEPROM;
      eeprom_write_byte((uint8_t*)0, gColorEEPROM);
    }
  }

  //------------------------- show LEDs
  ledStrip.show();
  delay(20);
}
