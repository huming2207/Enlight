/*
 * Sometimes interrupt does not work with FastLED (ref: https://github.com/FastLED/FastLED/wiki/Interrupt-problems)
 *
 * As a result, in this section we will use a separated task (in "loop()" function) and handle the key inputs.
 *
 */
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

  delay(5);
}

void KeyService::handleInput()
{
  shieldTouched = (touchRead(ENLIGHT_TOUCH_INTR_PIN) <= ENLIGHT_TOUCH_INTR_SENSE);
  keyPressed = (digitalRead(ENLIGHT_MAIN_KEY_INTR_PIN) == HIGH);

  // If shield touched, then adjust the brightness
  if(shieldTouched) {

    // Get current LED brightness

    if(fastLED->getBrightness() >= 255) {
      fastLED->setBrightness(1);
    }

    fastLED->setBrightness(fastLED->getBrightness() + (uint8_t)1);

    fastLED->show();

    // If key pressed, then switch it off.
  } else if(keyPressed) {

    /*if(fastLED->getBrightness() != 0) {
      fastLED->setBrightness((uint8_t)preferences.getUInt(ENLIGHT_NVRAM_LED_BRIGHTNESS, 255));
    } else {
      fastLED->setBrightness(0);
    } */

  }

  // Don't stress the CPU (waste CPU time)
  delay(5);
}