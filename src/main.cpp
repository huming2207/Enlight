#include "main.h"
#include "WebService.h"
#include "Color.h"

#include <Arduino.h>

Service service; // NOLINT
CFastLED fastLED; // NOLINT
Preferences preferences; // NOLINT

volatile uint8_t fastLedBright = 0;
volatile bool keyPressed = false;
volatile bool shieldTouched = false;

void keyLightTask(void *param)
{
  log_i("KeyLightTask: task created.");

  bool lastState;

  // Endless loop starts
  while(true) {

    lastState = fastLED.getBrightness() > 0;

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
}

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
  xTaskCreatePinnedToCore(keyLightTask, "keyLightTask", 10000, nullptr, 1, nullptr, ARDUINO_RUNNING_CORE);
}



void loop()
{

  // Sometimes interrupt does not work with FastLED (ref: https://github.com/FastLED/FastLED/wiki/Interrupt-problems)
  // As a result, use a separate task instead.
  shieldTouched = (touchRead(ENLIGHT_TOUCH_INTR_PIN) <= ENLIGHT_TOUCH_INTR_SENSE);
  keyPressed = (digitalRead(ENLIGHT_MAIN_KEY_INTR_PIN) == HIGH);

  // If shield touched, then adjust the brightness
  if(shieldTouched) {

    // Get current LED brightness
    fastLedBright = fastLED.getBrightness();

    if(fastLedBright >= 255) {
      fastLedBright = 1;
    }

    fastLedBright += 1;

    fastLED.setBrightness(fastLedBright);
    fastLED.show();

  // If key pressed, then switch it off.
  } else if(keyPressed) {

    /*if(fastLED.getBrightness() != 0) {
      fastLED.setBrightness((uint8_t)preferences.getUInt(ENLIGHT_NVRAM_LED_BRIGHTNESS, 255));
    } else {
      fastLED.setBrightness(0);
    } */

  }

  // Don't stress the CPU (waste CPU time)
  delay(5);
}

