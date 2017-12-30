//
// Created by Ming Hu on 12/12/17.
//

#include <ArduinoJson.h>

#include "Common.h"
#include "LightService.h"
#include "Color.h"

void LightService::init() {

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

  // "Set-up" section begins
  // Load server
  log_i("Web: Loading web service...\n");
  webServer = AsyncWebServer(80);

  log_i("Web: Registering handlers");
  enlightHandlerInit();

  log_i("Web: Service loaded at http://%s", WiFi.localIP().toString().c_str());

}

/**
 * Iterative all LEDs in the array and copy all colors to it
 * @param ledArray LED array
 * @param color Reference of color in CRGB LED object
 */
void LightService::copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color) {

  for (uint8_t ledIndex = 0; ledIndex < ledArray.size(); ledIndex++) {
    ledArray[ledIndex] = color;
  }
}

/**
 * Get color value from NVRAM and return in CRGB LED object
 * @return Color in CRGB LED object
 */
CRGB LightService::getColorFromNvram() {

  return CRGB((uint8_t) preferences.getUInt("red_led", 255),
              (uint8_t) preferences.getUInt("grn_led", 255),
              (uint8_t) preferences.getUInt("blu_led", 255));
}

/**
 * Set color value in CRGB LED object
 * @param color CRGB LED object
 * @return Size of saved data
 */
size_t LightService::setColorToNvram(CRGB color) {

  return preferences.putUInt("red_led", color.r)
      + preferences.putUInt("grn_led", color.g)
      + preferences.putUInt("blu_led", color.b);
}

void LightService::enlightHandlerInit() {
  webServer.on("/power",
               HTTP_GET,
               std::bind(&LightService::enlightSwitchHandler, this, std::placeholders::_1));

  webServer.on("/reset",
               HTTP_GET,
               std::bind(&LightService::enlightResetHandler, this, std::placeholders::_1));

  webServer.on("/color",
               HTTP_GET,
               std::bind(&LightService::enlightColorHandler, this, std::placeholders::_1));

  webServer.on("/bright",
               HTTP_GET,
               std::bind(&LightService::enlightBrightnessHandler, this, std::placeholders::_1));

  webServer.on("/setting",
               HTTP_POST,
               std::bind(&LightService::enlightSettingHandler, this, std::placeholders::_1));

  webServer.on("/info",
               HTTP_GET,
               std::bind(&LightService::enlightInfoHandler, this, std::placeholders::_1));

  webServer.begin();
}

void LightService::enlightResetHandler(AsyncWebServerRequest *request) {

  if (request->hasArg("factory") && request->arg("factory").equals("true")) {
    preferences.putBool("init_led", false);
    preferences.putBool("init_net", false);
  }

  ESP.restart();
}

void LightService::enlightSwitchHandler(AsyncWebServerRequest *request) {

  if (request->hasArg("switch") && request->arg("switch").equals("on")) {
    FastLED.setBrightness((uint8_t) preferences.getUInt("led_bgt", 255));
    request->send(202, "text/plain", "OK");
  } else if (request->hasArg("switch") && request->arg("switch").equals("off")) {
    FastLED.setBrightness(0);
    request->send(202, "text/plain", "OK");
  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void LightService::enlightColorHandler(AsyncWebServerRequest *request) {

  if (request->hasArg("r") && request->hasArg("g") && request->hasArg("b")) {
    CRGB color = CRGB((uint8_t) request->arg("r").toInt(),
                      (uint8_t) request->arg("g").toInt(),
                      (uint8_t) request->arg("b").toInt());

    if (request->hasArg("save") && request->arg("save").equals("true")) {
      log_i("Preference: saving color settings to NVRAM");
      setColorToNvram(color);
    }

    copyColorToAllLed(enlightArray, color);
    FastLED.show();

    request->send(202, "text/plain", "OK");
  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void LightService::enlightBrightnessHandler(AsyncWebServerRequest *request) {

  if (request->hasArg("value")) {

    uint8_t value = (uint8_t) request->arg("value").toInt();
    FastLED.setBrightness(value);

    if (request->hasArg("save") && request->arg("save").equals("true")) {
      log_i("Preference: saving new brightness value to NVRAM...");
      preferences.putUInt("save", value);
    }

    request->send(202, "text/plain", "OK");

  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void LightService::enlightSettingHandler(AsyncWebServerRequest *request) {
  if (!request->hasArg("plain")) {
    request->send(400, "text/plain", "Bad Request");
    return;
  }

  // Parse JSON
  DynamicJsonBuffer jsonBuffer;
  JsonObject &jsonObject = jsonBuffer.parseObject(request->arg("plain"));

  // Save to NVRAM
  // WiFi SSID
  if (jsonObject.containsKey("wifi_ssid")) {
    log_i("Preference: WiFi SSID has changed, new value: %s\n",
              jsonObject["wifi_ssid"].as<String>().c_str());
    preferences.putString("wifi_ssid", jsonObject["wifi_ssid"].as<String>());
  }

  // WiFi Password
  if (jsonObject.containsKey("wifi_passwd")) {
    log_i("Preference: WiFi password has changed, new value: %s\n",
              jsonObject["wifi_passwd"].as<String>().c_str());
    preferences.putString("wifi_passwd", jsonObject["wifi_passwd"].as<String>());
  }

  //
  if (jsonObject.containsKey("init_net")) {
    log_i("Preference: WiFi SSID has changed, new value: %s\n",
              jsonObject["wifi_passwd"].as<String>().c_str());
    preferences.putString("wifi_passwd", jsonObject["wifi_passwd"].as<String>());
  }
}

void LightService::enlightInfoHandler(AsyncWebServerRequest *request) {
  // Get device name
  String deviceName = String((unsigned long) ESP.getEfuseMac(), HEX);
  deviceName = deviceName.substring(deviceName.length() - 6);

  // Encode JSON and send it to user
  DynamicJsonBuffer jsonBuffer;

  JsonObject &infoObject = jsonBuffer.createObject();

  infoObject["led_count"] = ENLIGHT_LED_COUNT;
  infoObject["sys_free_ram"] = ESP.getFreeHeap();
  infoObject["sys_sdk_ver"] = ESP.getSdkVersion();
  infoObject["sys_id"] = String((unsigned long) ESP.getEfuseMac(), HEX).c_str();
  infoObject["sys_chip_rev"] = ESP.getChipRevision();
  infoObject["sys_dual_core"] = (ARDUINO_RUNNING_CORE == 1);
  infoObject["net_ssid"] = WiFi.SSID();
  infoObject["net_ip"] = WiFi.localIP().toString();

  // Print to string
  String jsonString;
  infoObject.printTo(jsonString);
  jsonBuffer.clear();

  // Send to user
  request->send(200, "application/json", jsonString);
}

void LightService::enlightNotFoundHandler(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Restful Service Not Found\n");
}


