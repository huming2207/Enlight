//
// Created by Ming Hu on 31/12/17.
//

#ifndef ENLIGHT_SERVICE_H
#define ENLIGHT_SERVICE_H

#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <FastLED.h>
#include "Hardware.h"

class Service
{

 private:
  Preferences preferences;
  void webInit();
  void copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color);
  void enlightResetHandler(AsyncWebServerRequest *request);
  void enlightSwitchHandler(AsyncWebServerRequest *request);
  void enlightColorHandler(AsyncWebServerRequest *request);
  void enlightBrightnessHandler(AsyncWebServerRequest *request);
  void enlightSettingHandler(AsyncWebServerRequest *request);
  void enlightInfoHandler(AsyncWebServerRequest *request);
  CRGBArray<ENLIGHT_LED_COUNT> enlightArray;

 public:
  void init();
  CRGB getColorFromNvram();
  size_t setColorToNvram(CRGB color);
};

#endif //ENLIGHT_SERVICE_H
