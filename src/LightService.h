//
// Created by Ming Hu on 12/12/17.
//

#ifndef ENLIGHT_LIGHT_H
#define ENLIGHT_LIGHT_H

#include "Hardware.h"

#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <FastLED.h>


class LightService
{
  public:
    void init();

  private:
    Preferences preferences;

    CRGBArray<ENLIGHT_LED_COUNT> enlightArray;

    void copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color);

    CRGB getColorFromNvram();

    size_t setColorToNvram(CRGB color);

 protected:
  AsyncWebServer webServer;

  void enlightHandlerInit();

  void enlightResetHandler(AsyncWebServerRequest *request);

  void enlightSwitchHandler(AsyncWebServerRequest *request);

  void enlightColorHandler(AsyncWebServerRequest *request);

  void enlightBrightnessHandler(AsyncWebServerRequest *request);

  void enlightSettingHandler(AsyncWebServerRequest *request);

  void enlightInfoHandler(AsyncWebServerRequest *request);

  void enlightNotFoundHandler(AsyncWebServerRequest *request);
};

#endif //ENLIGHT_LIGHT_H
