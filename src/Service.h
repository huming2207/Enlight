//
// Created by Ming Hu on 31/12/17.
//

#ifndef ENLIGHT_SERVICE_H
#define ENLIGHT_SERVICE_H

#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <FastLED.h>
#include <Update.h>
#include "HardwareMacro.h"

class Service
{

 private:
  Preferences *preferences;
  AsyncWebServer webServer = AsyncWebServer(80);
  void webInit();
  void copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color);
  void enlightResetHandler(AsyncWebServerRequest *request);
  void enlightSwitchHandler(AsyncWebServerRequest *request);
  void enlightColorHandler(AsyncWebServerRequest *request);
  void enlightBrightnessHandler(AsyncWebServerRequest *request);
  void enlightSettingHandler(AsyncWebServerRequest *request);
  void enlightInfoHandler(AsyncWebServerRequest *request);
  void enlightColorTempHandler(AsyncWebServerRequest * request);
  void enlightOtaHandler(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
  void enlightSaveHandler(AsyncWebServerRequest *request);
  void enlightOtaStatusHandler(AsyncWebServerRequest *request);
  CRGBArray<ENLIGHT_LED_COUNT> enlightArray;
  String enlightTemplateRenderer(const String& var);

 public:
  void init(CFastLED *led, Preferences *pref);
  CFastLED *fastLED;
  UpdateClass updater;
};

#endif //ENLIGHT_SERVICE_H
