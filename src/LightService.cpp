//
// Created by Ming Hu on 12/12/17.
//

#include "LightService.h"
#include "StaticPages/Error.h"


void LightService::init()
{
    // Load server
    webServer = ESP32WebServer();
    webServer.begin();

    // Load LEDs
    FastLED.addLeds<NEOPIXEL, ENLIGHT_LED_DATA_BUS_PIN>(enlightArray, ENLIGHT_LED_COUNT).setCorrection(TypicalSMD5050);

    // Load service settings
    preferences.begin("enlight_led");
    bool isInited = preferences.getBool("init", false);

    // If not init, set to default
    if(!isInited)
    {
        CRGB initialRGB = Color::GetRgbFromColorTemp(ENLIGHT_DEFAULT_LED_COLOR_TEMP);
        copyColorToAllLed(enlightArray, initialRGB);
        setColorToNvram(initialRGB);
    }
    else
    {
        CRGB storedRGB = getColorFromNvram();
        copyColorToAllLed(enlightArray, storedRGB);
    }



}

/**
 * Iterative all LEDs in the array and copy all colors to it
 * @param ledArray LED array
 * @param color Reference of color in CRGB LED object
 */
void LightService::copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color)
{
    for(uint8_t ledIndex = 0; ledIndex < ledArray.size(); ledIndex++)
    {
        ledArray[ledIndex] = color;
    }
}

/**
 * Get color value from NVRAM and return in CRGB LED object
 * @return Color in CRGB LED object
 */
CRGB LightService::getColorFromNvram()
{
    return CRGB((uint8_t)preferences.getUInt("red-led", 255),
                (uint8_t)preferences.getUInt("grn-led", 255),
                (uint8_t)preferences.getUInt("blu-led", 255));
}

/**
 * Set color value in CRGB LED object
 * @param color CRGB LED object
 * @return Size of saved data
 */
size_t LightService::setColorToNvram(CRGB color)
{
    return preferences.putUInt("red-led", color.r)
           + preferences.putUInt("grn-led", color.g)
           + preferences.putUInt("blu-led", color.b);
}

void LightService::restfulServiceTask(void * taskArg)
{
    // "Set-up" section


    // "Loop" section
    while(true)
    {
        // TODO: put some other shit here
    }
}

void LightService::enlightHandlerInit()
{
    // Handle 404 not found error
    webServer.onNotFound(std::bind(&LightService::enlightNotFoundHandler, this));

}

void LightService::enlightIndexPageHandler()
{

}

void LightService::enlightSwitchHandler()
{

}

void LightService::enlightColorHandler()
{

}

void LightService::enlightBrightnessHandler()
{

}

void LightService::enlightSettingHandler()
{

}

void LightService::enlightInfoHandler()
{
    // Get
    String deviceName = String((unsigned long)ESP.getEfuseMac(), HEX);
    deviceName = deviceName.substring(deviceName.length() - 6);
}

void LightService::enlightNotFoundHandler()
{
    webServer.send(404, "text/html", ENLIGHT_STATIC_PAGE_404);
}


