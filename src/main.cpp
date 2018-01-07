#include "main.h"
#include "Service.h"

#include <Arduino.h>

Service service; // NOLINT
CFastLED fastLED; // NOLINT
Preferences preferences; // NOLINT

volatile uint8_t fastLedBright = 0;
volatile uint16_t colorTemp = 1000;

void setup()
{
  // Main setup begins
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  log_i("System boot started...");

  service.init(&fastLED, &preferences);

  // Register main key
  // Setup main key function
  pinMode(ENLIGHT_MAIN_KEY_INTR_PIN, INPUT_PULLUP);

  // Setup main key's LED
  ledcSetup(ENLIGHT_MAIN_KEY_LEDC_CHANNEL, ENLIGHT_MAIN_KEY_LEDC_FREQ, ENLIGHT_MAIN_KEY_LEDC_BIT);
  ledcAttachPin(ENLIGHT_MAIN_KEY_INTR_PIN, ENLIGHT_MAIN_KEY_LEDC_CHANNEL);
}



void loop()
{

  // Sometimes interrupt does not work with FastLED (ref: https://github.com/FastLED/FastLED/wiki/Interrupt-problems)
  // As a result, use a separate task instead.
  bool shieldTouched = (touchRead(ENLIGHT_TOUCH_INTR_PIN) <= ENLIGHT_TOUCH_INTR_SENSE);
  bool keyPressed = (digitalRead(ENLIGHT_MAIN_KEY_INTR_PIN) == HIGH);

  if(shieldTouched && keyPressed) {



  } else if(shieldTouched) {

    fastLedBright = fastLED.getBrightness();

    if(fastLedBright >= 255) {
      fastLedBright = 0;
    }

    fastLedBright += 1;

    fastLED.setBrightness(fastLedBright);
    fastLED.show();


  } else if(keyPressed) {

    // Actually 7000k is enough for daily uses.
    if(colorTemp >= 7000) {
      colorTemp = 1000;
    } else {

    }



  }

  delay(5);
}