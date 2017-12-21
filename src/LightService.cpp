//
// Created by Ming Hu on 12/12/17.
//

#include "Common.h"
#include "LightService.h"
#include "Color.h"


void LightService::init()
{
  // Create service task, with higher (2) priority than original "loop()" method
  // UART will be fucked up if the task is created on the core where "setup()" method is not,
  // so use ARDUINO_RUNNING_CORE instead.
  log_i("TaskInit: Prepare to load service task...\n");
  xTaskCreatePinnedToCore(restfulServiceTask, "Service", 32000, nullptr, 2, nullptr, ARDUINO_RUNNING_CORE);

}

/**
 * Iterative all LEDs in the array and copy all colors to it
 * @param ledArray LED array
 * @param color Reference of color in CRGB LED object
 */
void LightService::copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color)
{

  for (uint8_t ledIndex = 0; ledIndex < ledArray.size(); ledIndex++) {
    ledArray[ledIndex] = color;
  }
}

/**
 * Get color value from NVRAM and return in CRGB LED object
 * @return Color in CRGB LED object
 */
CRGB LightService::getColorFromNvram()
{

  return CRGB((uint8_t) preferences.getUInt("red_led", 255),
              (uint8_t) preferences.getUInt("grn_led", 255),
              (uint8_t) preferences.getUInt("blu_led", 255));
}

/**
 * Set color value in CRGB LED object
 * @param color CRGB LED object
 * @return Size of saved data
 */
size_t LightService::setColorToNvram(CRGB color)
{

  return preferences.putUInt("red_led", color.r)
      + preferences.putUInt("grn_led", color.g)
      + preferences.putUInt("blu_led", color.b);
}

/**
 * Main service task
 * @param taskArg
 */
void LightService::restfulServiceTask(void *taskArg)
{

  // "Set-up" section begins
  // Load server
  log_i("Web: Loading web service...\n");
  webServer = WebServer();
  webServer.begin();
  log_i("Web: Service loaded at http://%s", WiFi.localIP().toString().c_str());

  // Load LEDs
  log_i("LED: Loading FastLED...\n");
  FastLED.addLeds<NEOPIXEL, ENLIGHT_LED_DATA_BUS_PIN>(enlightArray, ENLIGHT_LED_COUNT).setCorrection(TypicalSMD5050);

  // Load service settings
  log_i("Preference: loading LED preference with namespace \"enlight\"\n");
  preferences = Preferences();
  preferences.begin("enlight");

  // If not init, set to default
  if (!preferences.getBool("init_led", false)) {
    log_w("Preference: LED settings is not present, will be initialized!\n");
    CRGB initialRGB = Color::GetRgbFromColorTemp(ENLIGHT_DEFAULT_LED_COLOR_TEMP);
    copyColorToAllLed(enlightArray, initialRGB);
    setColorToNvram(initialRGB);

    // Set init flag to true
    log_i("Preference: setting LED init flag to true...\n");
    preferences.putBool("init_led", true);
  } else {
    log_w("Preference: LED settings found!\n");
    CRGB storedRGB = getColorFromNvram();
    copyColorToAllLed(enlightArray, storedRGB);
  }

  preferences.end();


  // "Loop" section
  while (true) {
    webServer.handleClient();
  }
}

void LightService::enlightHandlerInit()
{

  webServer.onNotFound([this]()
                       {
                         this->enlightNotFoundHandler();
                       });

  webServer.on("/switch", [this]()
  {
    this->enlightSwitchHandler();
  });

  webServer.on("/color", [this]()
  {
    this->enlightColorHandler();
  });

  webServer.on("/bright", [this]()
  {
    this->enlightBrightnessHandler();
  });

  webServer.on("/setting", [this]()
  {
    this->enlightSettingHandler();
  });

  webServer.on("/reset", [this]()
  {
    this->enlightResetHandler();
  });

  webServer.on("/info", [this]()
  {
    this->enlightInfoHandler();
  });
}

void LightService::enlightResetHandler()
{

  if (webServer.hasArg("factory") && webServer.arg("factory").equals("true")) {
    preferences.putBool("init_led", false);
    preferences.putBool("init_net", false);
  }

  ESP.restart();
}

void LightService::enlightSwitchHandler()
{

  if (webServer.hasArg("switch") && webServer.arg("switch").equals("on")) {
    FastLED.setBrightness((uint8_t) preferences.getUInt("led_bgt", 255));
    webServer.send(202, "text/plain", "OK");
  } else if (webServer.hasArg("switch") && webServer.arg("switch").equals("off")) {
    FastLED.setBrightness(0);
    webServer.send(202, "text/plain", "OK");
  } else {
    webServer.send(400, "text/plain", "Bad Request");
  }
}

void LightService::enlightColorHandler()
{

  if (webServer.hasArg("r") && webServer.hasArg("g") && webServer.hasArg("b")) {
    CRGB color = CRGB((uint8_t) webServer.arg("r").toInt(),
                      (uint8_t) webServer.arg("g").toInt(),
                      (uint8_t) webServer.arg("b").toInt());

    if (webServer.hasArg("save") && webServer.arg("save").equals("true")) {
      log_i("Preference: saving color settings to NVRAM");
      setColorToNvram(color);
    }

    copyColorToAllLed(enlightArray, color);
    FastLED.show();

    webServer.send(202, "text/plain", "OK");
  } else {
    webServer.send(400, "text/plain", "Bad Request");
  }
}

void LightService::enlightBrightnessHandler()
{

  if (webServer.hasArg("value")) {

    uint8_t value = (uint8_t) webServer.arg("value").toInt();
    FastLED.setBrightness(value);

    if (webServer.hasArg("save") && webServer.arg("save").equals("true")) {
      log_i("Preference: saving new brightness value to NVRAM...");
      preferences.putUInt("save", value);
    }

    webServer.send(202, "text/plain", "OK");

  } else {
    webServer.send(400, "text/plain", "Bad Request");
  }
}

void LightService::enlightSettingHandler()
{

}

void LightService::enlightInfoHandler()
{
  // Get device name
  String deviceName = String((unsigned long) ESP.getEfuseMac(), HEX);
  deviceName = deviceName.substring(deviceName.length() - 6);
}

void LightService::enlightNotFoundHandler()
{

  webServer.send(404, "text/plain", "Restful Service Not Found\n");
}


