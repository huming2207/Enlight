//
// Created by Ming Hu on 12/12/17.
// Original JavaScript version written by Paul Kaplan: https://gist.github.com/paulkaplan/5184275
//

#include "Color.h"


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

  return CRGB(clipColorValue(redValue, 0, 255),    // Final red color value
              clipColorValue(greenValue, 0, 255),  // Final green color value
              clipColorValue(blueValue, 0, 255));  // Final blue color value
}

uint8_t Color::clipColorValue(int number, uint8_t min, uint8_t max)
{
  // for ESP32 platform, std::min and std::max are broken
  if (number < min) return min;
  if (number > max) return max;

  return number;
}
