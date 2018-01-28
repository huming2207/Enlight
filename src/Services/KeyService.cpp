/*
 * Sometimes interrupt does not work with FastLED (ref: https://github.com/FastLED/FastLED/wiki/Interrupt-problems)
 *
 * As a result, in this section we will use a separated task (in "loop()" function) and handle the key inputs.
 *
 */
#include <main.h>
#include "Defaults/Pinmaps.h"
#include "KeyService.h"

void KeyService::init(CFastLED *led, Preferences *pref)
{
  // Passes FastLED and Preferences objects in
  fastLED = led;
  preferences = pref;

  // Register main key
  // Setup main key function
  pinMode(ENLIGHT_MAIN_KEY_INTR_PIN, INPUT_PULLUP);

  // Setup main key's LED
  ledcSetup(ENLIGHT_MAIN_KEY_LEDC_CHANNEL, ENLIGHT_MAIN_KEY_LEDC_FREQ, ENLIGHT_MAIN_KEY_LEDC_BIT);
  ledcAttachPin(ENLIGHT_MAIN_KEY_LED_PIN, ENLIGHT_MAIN_KEY_LEDC_CHANNEL);
}

void KeyService::handleKeyLight()
{
  lastState = fastLED->getBrightness() > 0;

  // If the light is on, write the maximum duty value
  if(lastState) {
    ledcWrite(ENLIGHT_MAIN_KEY_LEDC_CHANNEL, 8192);

  } else { // Or, let it turns off slowly in 8 seconds
    for(uint32_t ledDuty = 8192; ledDuty < 1; ledDuty -= 1) {
      ledcWrite(ENLIGHT_MAIN_KEY_LEDC_CHANNEL, ledDuty);
      delay(1);
    }
  }
}

void KeyService::handleInput()
{
  uint32_t keyPressedDelay = 0;

  while(digitalRead(ENLIGHT_MAIN_KEY_INTR_PIN) == HIGH) {

    // Delay 50ms and take it to the delay counter
    delay(5);
    keyPressedDelay += 5;

    // If the key press event is longer than 1 second, then it must be long press event
    // i.e. adjust brightness
    if(keyPressedDelay > 1000) {
      if(fastLED->getBrightness() == 255) {
        fastLED->setBrightness(1);
      } else {
        ledcWrite(ENLIGHT_MAIN_KEY_LEDC_CHANNEL, 0);
        fastLED->setBrightness(fastLED->getBrightness() + (uint8_t)10);

        // Delay 50ms and take it to the delay counter
        delay(100);
        keyPressedDelay += 100;

        ledcWrite(ENLIGHT_MAIN_KEY_LEDC_CHANNEL, 8192);
      }
    }
  }

  // When key is pressed in less than about 1 second,
  // treat it as short pressing event, i.e. turn the light on/off
  if(keyPressedDelay <= 1000) {

    // Minimum brightness should be 1, if it's not, then it has turned off
    if (fastLED->getBrightness() < 1) {
      fastLED->setBrightness((uint8_t) preferences->getUInt(ENLIGHT_NVRAM_LED_BRIGHTNESS, 255));
    } else {
      fastLED->setBrightness(0);
    }
  }
}