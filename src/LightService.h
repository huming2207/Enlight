//
// Created by Ming Hu on 12/12/17.
//

#ifndef ENLIGHT_LIGHT_H
#define ENLIGHT_LIGHT_H

#include "Common.h"
#include "Color.h"

class Light
{
    public:
        void init();

    private:
        aREST restfulService;
        Preferences preferences;
        CRGBArray<ENLIGHT_LED_COUNT> enlightArray;
        void copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color);
        CRGB getColorFromNvram();
        size_t setColorToNvram(CRGB color);
        void restfulServiceTask(void * taskArg);
};


#endif //ENLIGHT_LIGHT_H
