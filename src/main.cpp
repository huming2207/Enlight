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
}

