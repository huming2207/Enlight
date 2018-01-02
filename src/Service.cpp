//
// Created by Ming Hu on 31/12/17.
//

#include "Service.h"
#include "main.h"
#include "Color.h"

#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <AsyncJson.h>

void Service::init()
{
  bool forceOffline = false;

  // Load service settings
  log_i("Preference: loading LED preference with namespace \"enlight\"\n");
  preferences.begin("enlight");

  // If LED is not init, set to default
  if (preferences.getBool("enlight_init", true)) {

    // Init LEDs
    log_w("Preference: Settings are not present or corrupted, will be initialized!\n");
    preferences.clear();
    CRGB initialRGB = Color::GetRgbFromColorTemp(ENLIGHT_DEFAULT_LED_COLOR_TEMP);
    copyColorToAllLed(enlightArray, initialRGB);
    setColorToNvram(initialRGB);

    // Init network
    log_w("Network: network is not yet configured, will load in AP mode anyway...");

    if (!WiFi.softAP(ENLIGHT_DEFAULT_WIFI_SSID)) {
      log_e("Network: cannot set WiFi driver to AP mode! System will turns to offline mode");
      forceOffline = true;
    }

    // Print out IP address
    IPAddress selfIP = WiFi.softAPIP();
    log_i("Network: WiFi AP setup successful, go to http://%s", selfIP.toString().c_str());

  } else {

    log_w("Preference: LED settings found!\n");
    CRGB storedRGB = getColorFromNvram();
    copyColorToAllLed(enlightArray, storedRGB);

    // Continue to connect WiFi
    WiFi.begin(preferences.getString("wifi_ssid", ENLIGHT_DEFAULT_WIFI_SSID).c_str(),
               preferences.getString("wifi_passwd", "").c_str());

    // Wait some time until it connects
    uint8_t retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < ENLIGHT_DEFAULT_RETRY_COUNT) {
      log_i("Network: awaiting WiFi AP response, tried %d times...", retryCount);
      retryCount += 1;
      delay(1000);
    }

    if (WiFi.status() != WL_CONNECTED) {
      log_e("Network: no WiFi connection, will work in offline mode!");
      forceOffline = true;
    }

  }

  // Goes to offline mode if WiFi is not connected, otherwise set up mDNS and web service.
  if(!forceOffline) {

    // Get unique device name from EfuseMac
    String deviceName = String((unsigned long) ESP.getEfuseMac(), HEX);
    deviceName = "enlight-" + deviceName;

    // Register mDNS service
    log_i("MDNS: Setting up mDNS service with device name %s...\n", deviceName.c_str());

    if(WiFi.getMode() == WIFI_MODE_APSTA || WiFi.getMode() == WIFI_MODE_AP) {
      MDNS.begin(deviceName.c_str(), TCPIP_ADAPTER_IF_AP);
    } else {
      MDNS.begin(deviceName.c_str());
    }

    MDNS.addService("_http", "_tcp", 80);


    // Do web initialization
    webInit();
  }

}

void Service::webInit()
{
  log_d("Service: init filesystem...");
  SPIFFS.begin();

  log_d("Service: init web service, registering handlers");
  webServer.on("/power",
               HTTP_GET,
               std::bind(&Service::enlightSwitchHandler, this, std::placeholders::_1));

  webServer.on("/reset",
               HTTP_GET,
               std::bind(&Service::enlightResetHandler, this, std::placeholders::_1));

  webServer.on("/color",
               HTTP_GET,
               std::bind(&Service::enlightColorHandler, this, std::placeholders::_1));

  webServer.on("/bright",
               HTTP_GET,
               std::bind(&Service::enlightBrightnessHandler, this, std::placeholders::_1));

  webServer.on("/setting",
               HTTP_POST,
               std::bind(&Service::enlightSettingHandler, this, std::placeholders::_1));

  webServer.on("/info",
               HTTP_GET,
               std::bind(&Service::enlightInfoHandler, this, std::placeholders::_1));

  webServer.serveStatic("/", SPIFFS, "/")
      .setTemplateProcessor(std::bind(&Service::enlightTemplateRenderer, this, std::placeholders::_1))
      .setDefaultFile("index.html");

  // Separate resource out from root, as the template engine will fuck up the CSS
  webServer.serveStatic("/resources", SPIFFS, "/res")
      .setTemplateProcessor(std::bind(&Service::enlightTemplateRenderer, this, std::placeholders::_1));

  webServer.begin();

  log_i("Web: Service loaded at http://%s", WiFi.localIP().toString().c_str());
}

/**
 * Get color value from NVRAM and return in CRGB LED object
 * @return Color in CRGB LED object
 */
CRGB Service::getColorFromNvram()
{

  return CRGB((__uint8_t) preferences.getUInt("red_led", 255),
              (__uint8_t) preferences.getUInt("grn_led", 255),
              (__uint8_t) preferences.getUInt("blu_led", 255));
}

/**
 * Set color value in CRGB LED object
 * @param color CRGB LED object
 * @return Size of saved data
 */
size_t Service::setColorToNvram(CRGB color)
{

  return preferences.putUInt("red_led", color.r)
      + preferences.putUInt("grn_led", color.g)
      + preferences.putUInt("blu_led", color.b);
}

/**
 * Iterative all LEDs in the array and copy all colors to it
 * @param ledArray LED array
 * @param color Reference of color in CRGB LED object
 */
void Service::copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color)
{

  for (uint8_t ledIndex = 0; ledIndex < ledArray.size(); ledIndex++) {
    ledArray[ledIndex] = color;
  }
}



void Service::enlightResetHandler(AsyncWebServerRequest *request)
{

  if (request->hasArg("factory") && request->arg("factory").equals("true")) {
    preferences.putBool("enlight_init", true);
  }

  ESP.restart();
}

void Service::enlightSwitchHandler(AsyncWebServerRequest *request)
{

  if (request->hasArg("switch") && request->arg("switch").equals("on")) {
    FastLED.setBrightness((uint8_t) preferences.getUInt("led_bgt", 255));
    request->send(200, "text/plain", "OK");
  } else if (request->hasArg("switch") && request->arg("switch").equals("off")) {
    FastLED.setBrightness(0);
    request->send(200, "text/plain", "OK");
  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void Service::enlightColorHandler(AsyncWebServerRequest *request)
{

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

    request->send(200, "text/plain", "OK");
  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void Service::enlightBrightnessHandler(AsyncWebServerRequest *request)
{

  if (request->hasArg("value")) {

    uint8_t value = (uint8_t) request->arg("value").toInt();
    FastLED.setBrightness(value);

    if (request->hasArg("save") && request->arg("save").equals("true")) {
      log_i("Preference: saving new brightness value to NVRAM...");
      preferences.putUInt("save", value);
    }

    request->send(200, "text/plain", "OK");

  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void Service::enlightSettingHandler(AsyncWebServerRequest *request)
{

  // Save to NVRAM
  // WiFi SSID
  if (request->hasArg("wifi_ssid")) {
    log_i("Preference: WiFi SSID has changed, new value: %s\n", request->arg("wifi_ssid"));
    preferences.putString("wifi_ssid", request->arg("wifi_ssid"));
  }

  // WiFi Password
  if (request->hasArg("wifi_passwd")) {
    log_i("Preference: WiFi password has changed, new value: %s\n", request->arg("wifi_passwd"));
    preferences.putString("wifi_passwd", request->arg("wifi_passwd"));
  }

  // Set init flag to true
  log_i("Preference: setting LED init flag to true...\n");
  preferences.putBool("enlight_init", false);
  
  request->send(200, "text/plain", "OK");
}

void Service::enlightInfoHandler(AsyncWebServerRequest *request)
{
  // Get device name
  String deviceName = String((unsigned long) ESP.getEfuseMac(), HEX);
  deviceName = deviceName.substring(deviceName.length() - 6);

  // Encode JSON and send it to user
  AsyncJsonResponse *response = new AsyncJsonResponse();
  JsonObject &infoObject = response->getRoot();

  // Add information to buffer
  infoObject["led_count"] = ENLIGHT_LED_COUNT;
  infoObject["sys_inited"] = preferences.getBool("enlight_init", false);
  infoObject["sys_free_ram"] = ESP.getFreeHeap();
  infoObject["sys_sdk_ver"] = ESP.getSdkVersion();
  infoObject["sys_id"] = String((unsigned long) ESP.getEfuseMac(), HEX);
  infoObject["sys_chip_rev"] = ESP.getChipRevision();
  infoObject["sys_dual_core"] = (ARDUINO_RUNNING_CORE == 1);
  infoObject["sys_ver"] = ENLIGHT_VERSION_FULL;

  // WiFi.localIP() will only return correct IP address when it works in client (STA only) mode.
  // To get correct IP address in AP mode or AP+STA mixed mode, we need to use WiFi.softAPIP() instead.
  infoObject["net_ip"] =
      (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA) ?
          WiFi.softAPIP().toString() :
          WiFi.localIP().toString();

  // ...same as SSID
  infoObject["net_ssid"] = (WiFi.SSID().length() < 1) ? ENLIGHT_DEFAULT_WIFI_SSID : WiFi.SSID();
  infoObject["net_sig"] = WiFi.RSSI();

  // Now prepare to send out the stream buffer
  response->setContentType("application/json");
  response->setCode(200);

  // Get the length
  response->setLength();

  // Send to user
  request->send(response);
}

String Service::enlightTemplateRenderer(const String& var)
{
  // Return WiFi SSID
  if(var == "WIFI_SSID") {
    return(String((WiFi.SSID().length() < 1) ? ENLIGHT_DEFAULT_WIFI_SSID : WiFi.SSID()));
  }

  // Return WiFi RSSI value in percentage
  if(var == "WIFI_RSSI") {
    return(String(2 * (WiFi.RSSI() + 100)) + " %25");
  }

  // Return current IP address
  if(var == "WIFI_IP") {
    return((WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA) ?
           WiFi.softAPIP().toString() :
           WiFi.localIP().toString());
  }

  // Return firmware version
  if(var == "FIRM_VERSION") {
    return(ENLIGHT_VERSION_FULL);
  }

  // Return serial number (EfuseMac)
  if(var == "EFUSE_SN") {
    return(String((unsigned long) ESP.getEfuseMac(), HEX));
  }

  return String();
}