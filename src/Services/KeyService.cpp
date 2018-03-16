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
  previous = HIGH;
  setBright = false;
}

void KeyService::handleInput()
{
  current = digitalRead(ENLIGHT_MAIN_KEY_INTR_PIN);

  // if the button state changes to pressed, remember the start time
  if (current == LOW && previous == HIGH && (millis() - firstTime) > 200) {
    firstTime = millis();
  }

  millis_held = (millis() - firstTime);
  secs_held = millis_held / 1000;

  // This if statement is a basic debouncing tool, the button must be pushed for at least
  // 100 milliseconds in a row for it to be considered as a push.
  if (millis_held > 100) {

    // check if the button was released since we last checked
    if (current == HIGH && previous == LOW) {
      // ===============================================================================
      // Button pressed for less than 1 second
      if(setBright) {
        log_i("Exiting brightness selection...");
        setBright = false;
      } else if (secs_held <= 1) {
        if(fastLED->getBrightness() == 0) {
          log_i("Now the LED should be ON, brightness %d.", fastLED->getBrightness());
          fastLED->setBrightness((uint8_t)preferences->getUInt(ENLIGHT_NVRAM_LED_BRIGHTNESS, 255));
          fastLED->show();
        } else {
          log_i("Now the LED should be OFF, brightness %d.", fastLED->getBrightness());
          fastLED->setBrightness(0);
          fastLED->show();
        }
      } else {
        setBright = true;
        log_i("Entering brightness selection...");
        while(digitalRead(ENLIGHT_MAIN_KEY_INTR_PIN) == HIGH && setBright) {
          if(fastLED->getBrightness() == 255) {
            fastLED->setBrightness(1);
          } else {
            fastLED->setBrightness((uint8_t)(fastLED->getBrightness() + 1));
          }

          delay(20);
          fastLED->show();
        }
      }
      // ===============================================================================
    }
  }

  previous = current;
  prev_secs_held = secs_held;

  delay(20);
}