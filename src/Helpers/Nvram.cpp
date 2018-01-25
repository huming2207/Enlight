//
// Created by Ming Hu on 7/1/18.
//

#include "Nvram.h"
#include "Defaults/Firmware.h"

CRGB Nvram::getColorFromNvram(Preferences *preferences)
{
  return CRGB(preferences->getUInt(ENLIGHT_NVRAM_LED_COLOR, 0));
}

size_t Nvram::setColorToNvram(CRGB &color, Preferences *preferences)
{
  return preferences->putUInt(
      ENLIGHT_NVRAM_LED_COLOR,
      (uint32_t)(((color.r & 0xff) << 16) + ((color.g & 0xff) << 8) + (color.b & 0xff)));
}

