//
// Created by Ming Hu on 7/1/18.
//

#ifndef ENLIGHT_HELPERS_H
#define ENLIGHT_HELPERS_H

#include <FastLED.h>
#include <Preferences.h>

class Helpers
{
 public:
  static CRGB getColorFromNvram(Preferences *preferences);
  static size_t setColorToNvram(CRGB &color, Preferences *preferences);
  static uint32_t rgbToColorCode(CRGB &color);
  static uint32_t rgbToColorCode(uint8_t red, uint8_t green, uint8_t blue);
};

#endif //ENLIGHT_HELPERS_H
