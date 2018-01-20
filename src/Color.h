//
// Created by Ming Hu on 12/12/17.
//

#ifndef ENLIGHT_COLOR_H
#define ENLIGHT_COLOR_H

#include "Firmware.h"

#include <FastLED.h>


class Color
{
  public:
    static CRGB GetRgbFromColorTemp(int colorTemp);

  private:
    static uint8_t clipColorValue(uint8_t number, uint8_t min, uint8_t max);

};

#endif //ENLIGHT_COLOR_H
