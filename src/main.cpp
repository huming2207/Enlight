#include "main.h"
#include "Services/WebService.h"
#include "Services/KeyService.h"

#include <Arduino.h>

WebService webService; // NOLINT
KeyService keyService; // NOLINT
CFastLED fastLED;      // NOLINT
Preferences preferences; // NOLINT

void setup()
{
  // Main setup begins
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  log_i("System boot started...");

  // Initialise web services
  webService.init(&fastLED, &preferences);

  // Key Interrupt
  keyService.init(&fastLED, &preferences);
}

void loop()
{
  keyService.handleInput();

  // When user presses Ctrl-C, it should reboots.
  if(Serial.available() > 0) {
    if(Serial.read() == 3) {
      log_w("Will now restart!");
      ESP.restart();
    }
  }
}

