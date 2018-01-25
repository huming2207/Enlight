//
// Created by Ming Hu on 12/12/17.
// Original JavaScript version written by Paul Kaplan: https://gist.github.com/paulkaplan/5184275
//

#include <Defaults/Pinmaps.h>
#include "Nvram.h"
#include "Color.h"

/**
 * Convert color temperature to CRGB struct (with RGB value inside it)
 * @param colorTemp - color temperature
 * @return - CRGB struct
 */
CRGB Color::GetRgbFromColorTemp(int colorTemp)
{

  double temp = colorTemp / 100;
  int redValue, greenValue, blueValue;

  // If color is warmer than 6600k, then full red is needed.
  if (temp <= 66) {
    redValue = 255;

    greenValue = (int) temp;
    greenValue = (int) (99.4708025861 * log(greenValue) - 161.1195681661);

    // Calculate blue value, if temperature < 1900k, no more blue
    if (temp <= 19) {
      blueValue = 0;
    } else {
      blueValue = (int) (temp - 10);
      blueValue = (int) (138.5177312231 * log(blueValue) - 305.0447927307);
    }
  } else {
    redValue = (int) (temp - 60);
    redValue = (int) (329.698727446 * pow(redValue, -0.1332047592));

    greenValue = (int) (temp - 60);
    greenValue = (int) (288.1221695283 * pow(greenValue, -0.0755148492));

    blueValue = 255;
  }

  return CRGB(clipColorValue((uint8_t)redValue, 0, 255),    // Final red color value
              clipColorValue((uint8_t)greenValue, 0, 255),  // Final green color value
              clipColorValue((uint8_t)blueValue, 0, 255));  // Final blue color value
}

/**
 * ...just a clip method
 *
 * @param number - input value
 * @param min - minimum value
 * @param max - maximum value
 * @return return value between min < number < max
 */
uint8_t Color::clipColorValue(uint8_t number, uint8_t min, uint8_t max)
{
  // for ESP32 platform, std::min and std::max are broken
  if (number < min) return min;
  if (number > max) return max;

  return number;
}

/**
 * uint8_t struct to uint_32 value
 * @return unsigned integer, 32bit
 */
uint32_t Color::rgbToColorCode(uint8_t red, uint8_t green, uint8_t blue)
{
  CRGB color = CRGB(red, green, blue);
  return rgbToColorCode(color);
}

/**
 * FastLED color struct to uint_32 value
 * @param color
 * @return unsigned integer, 32bit
 */
uint32_t Color::rgbToColorCode(CRGB &color)
{
  return (uint32_t)(((color.r & 0xff) << 16) + ((color.g & 0xff) << 8) + (color.b & 0xff));
}

/**
 * Iterate all LEDs in the array and copy all colors to it
 * @param ledArray LED array
 * @param color Reference of color in CRGB LED object
 */
void Color::copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color, CFastLED *fastLED)
{
  log_i("Color: copying color, r=%d, g=%d b=%d", color.r, color.g, color.b);
  for (uint8_t ledIndex = 0; ledIndex < ledArray.size(); ledIndex++) {
    ledArray[ledIndex] = color;
  }

  fastLED->show();
}