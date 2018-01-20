//
// Created by Ming Hu on 7/1/18.
//

#include "Helpers.h"
#include "Firmware.h"

CRGB Helpers::getColorFromNvram(Preferences *preferences)
{
  return CRGB(preferences->getUInt(ENLIGHT_NVRAM_LED_COLOR, 0));
}

size_t Helpers::setColorToNvram(CRGB &color, Preferences *preferences)
{
  return preferences->putUInt(
      ENLIGHT_NVRAM_LED_COLOR,
      (uint32_t)(((color.r & 0xff) << 16) + ((color.g & 0xff) << 8) + (color.b & 0xff)));
}

uint32_t Helpers::rgbToColorCode(uint8_t red, uint8_t green, uint8_t blue)
{
  CRGB color = CRGB(red, green, blue);
  return rgbToColorCode(color);
}

uint32_t Helpers::rgbToColorCode(CRGB &color)
{
  return (uint32_t)(((color.r & 0xff) << 16) + ((color.g & 0xff) << 8) + (color.b & 0xff));
}
