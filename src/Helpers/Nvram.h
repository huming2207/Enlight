//
// Created by Ming Hu on 7/1/18.
//

#ifndef ENLIGHT_NVRAM_HELPER_H
#define ENLIGHT_NVRAM_HELPER_H

#include <FastLED.h>
#include <Preferences.h>

class Nvram
{
 public:
  static CRGB getColorFromNvram(Preferences *preferences);
  static size_t setColorToNvram(CRGB &color, Preferences *preferences);
};

#endif //ENLIGHT_NVRAM_HELPER_H
