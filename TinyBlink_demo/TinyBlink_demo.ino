/*************************************************************************


    [2024] - WAVELIGHTS - Markus E. Loeffler
    All Rights Reserved.

*/
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

#define LED_COUNT 12
#define LED_COUNT_HALF (LED_COUNT / 2)
#define LED_PIN 1
#define MIC_PIN A1
#define BUTTON_PIN A3

Adafruit_NeoPixel ledStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint8_t gColorEEPROM;
uint8_t gBrightEEPROM;
uint8_t gBright2;
uint8_t gManuColor;
float gPegel_smooth;
uint16_t gButtonState = 0;  // Current debounce status
bool gWasLongPress = false;

uint32_t getColor(uint8_t val)
{
  const uint8_t niceHue[7] = { 20, 80, 128, 170, 190, 240, 0 };
  uint32_t color;
  if (gManuColor == 0) {
    uint16_t hue = 64000 + val * 40;
    color = ledStrip.ColorHSV(hue, 255, val);
  } else {
    uint8_t index = (gColorEEPROM - 1) % 9;
    uint8_t minLuma = min(255, val + 45);
    if (index == 7) {
      color = ledStrip.ColorHSV(0, 0, minLuma);
    } else {
      color = ledStrip.ColorHSV(niceHue[index] * 257, 255, minLuma);
    }
  }
  return color;
}

void pegeleffect()
{
  uint32_t color = getColor(127);
  uint32_t colorOff = getColor(0);
  uint8_t pegel = min(gPegel_smooth , LED_COUNT) ;

  for (uint8_t n = 0; n < LED_COUNT; ++n) {
    uint32_t col = n < pegel ? color : colorOff;
    ledStrip.setPixelColor(n, col);
  }
}


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
  //------------------------- read audio
  uint16_t minA = 1023;
  uint16_t maxA = 0;
  for (uint8_t i = 0; i < 80; i++) {
    uint16_t aIN = analogRead(MIC_PIN);
    maxA = max(aIN, maxA);
    minA = min(aIN, minA);
  }
  float dynamic = maxA - minA;
  gPegel_smooth = 0.9f * gPegel_smooth + 0.1f * dynamic;

  //------------------------- set global LED brightness
  uint16_t inVal = gBrightEEPROM;
  inVal *= inVal;
  inVal = inVal / 67;
  ledStrip.setBrightness(inVal + 14);

  //------------------------- paint LEDs
  gManuColor = gColorEEPROM % 9;

  pegeleffect();

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

  delay(10);
  ledStrip.show();
}
