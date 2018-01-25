//
// Created by Ming Hu on 12/12/17.
//

#ifndef ENLIGHT_COLOR_H
#define ENLIGHT_COLOR_H

#include "Defaults/Firmware.h"

#include <FastLED.h>


class Color
{
  public:
    static CRGB GetRgbFromColorTemp(int colorTemp);
    static uint32_t rgbToColorCode(uint8_t red, uint8_t green, uint8_t blue);
    static uint32_t rgbToColorCode(CRGB &color);
    static void copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color, CFastLED *fastLED);

  private:
    static uint8_t clipColorValue(uint8_t number, uint8_t min, uint8_t max);

};

#endif //ENLIGHT_COLOR_H
