//
// Created by Ming Hu on 12/12/17.
//

#ifndef ENLIGHT_LIGHT_H
#define ENLIGHT_LIGHT_H

#include "Common.h"
#include "Color.h"

class LightService
{
    public:
        void init();

    private:
        Preferences preferences;
        ESP32WebServer webServer;
        CRGBArray<ENLIGHT_LED_COUNT> enlightArray;
        void copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color);
        CRGB getColorFromNvram();
        size_t setColorToNvram(CRGB color);
        void restfulServiceTask(void * taskArg);
        void enlightHandlerInit();
        void enlightIndexPageHandler();
        void enlightSwitchHandler();
        void enlightColorHandler();
        void enlightBrightnessHandler();
        void enlightSettingHandler();
        void enlightInfoHandler();
        void enlightNotFoundHandler();
};


#endif //ENLIGHT_LIGHT_H
