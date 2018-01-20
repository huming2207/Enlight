//
// Created by Ming Hu on 31/12/17.
//

#include "WebService.h"
#include "main.h"
#include "Helpers/Color.h"
#include "Helpers/Nvram.h"

#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <AsyncJson.h>

void WebService::init(CFastLED *led, Preferences *pref)
{

  bool forceOffline = false;

  // Load service settings
  log_i("Preference: loading LED preference with namespace \"enlight\"\n");
  preferences = pref;
  preferences->begin("enlight");

  // Load LEDs
  log_i("LED: init LED arrays...");
  fastLED = led;
  fastLED->addLeds<NEOPIXEL, ENLIGHT_LED_DATA_BUS_PIN>(enlightArray, ENLIGHT_LED_COUNT);

  // If the lamp is not init, set to default settings with WiFi in AP mode
  if (preferences->getBool(ENLIGHT_NVRAM_INIT_FLAG, true)) {

    // Init LEDs
    log_w("Preference: Settings are not present or corrupted, will be initialized!\n");
    preferences->clear();
    CRGB initialRGB = Color::GetRgbFromColorTemp(ENLIGHT_DEFAULT_LED_COLOR_TEMP);
    copyColorToAllLed(enlightArray, initialRGB);
    Helpers::setColorToNvram(initialRGB, preferences);

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
    CRGB storedRGB = Helpers::getColorFromNvram(preferences);
    copyColorToAllLed(enlightArray, storedRGB);

    // Continue to connect WiFi
    WiFi.begin(preferences->getString(ENLIGHT_NVRAM_WIFI_SSID, ENLIGHT_DEFAULT_WIFI_SSID).c_str(),
               preferences->getString(ENLIGHT_NVRAM_WIFI_PASSWORD, "").c_str());

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
  if (!forceOffline) {

    // Get unique device name from EfuseMac
    String deviceName = String((unsigned long) ESP.getEfuseMac(), HEX);
    deviceName = "enlight-" + deviceName;

    // Register mDNS service
    log_i("MDNS: Setting up mDNS service with device name %s...\n", deviceName.c_str());

    if (WiFi.getMode() == WIFI_MODE_APSTA || WiFi.getMode() == WIFI_MODE_AP) {
      MDNS.begin(deviceName.c_str(), TCPIP_ADAPTER_IF_AP);
    } else {
      MDNS.begin(deviceName.c_str());
    }

    // Register MDNS service
    MDNS.addService("_http", "_tcp", 80);


    // Do web initialization
    webInit();
  }

}

void WebService::webInit()
{

  // Mount filesystem
  log_d("WebService: init filesystem...");
  SPIFFS.begin();

  // Load OTA service
  updater = UpdateClass();

  // Register web service nodes
  log_d("WebService: init web service, registering handlers");
  webServer.on("/power",
               HTTP_GET,
               std::bind(&WebService::enlightSwitchHandler, this, std::placeholders::_1));

  webServer.on("/reset",
               HTTP_GET,
               std::bind(&WebService::enlightResetHandler, this, std::placeholders::_1));

  webServer.on("/color",
               HTTP_GET,
               std::bind(&WebService::enlightColorHandler, this, std::placeholders::_1));

  webServer.on("/temp",
                HTTP_GET,
                std::bind(&WebService::enlightColorTempHandler, this, std::placeholders::_1));

  webServer.on("/bright",
               HTTP_GET,
               std::bind(&WebService::enlightBrightnessHandler, this, std::placeholders::_1));

  webServer.on("/setting",
               HTTP_POST,
               std::bind(&WebService::enlightSettingHandler, this, std::placeholders::_1));

  webServer.on("/info",
               HTTP_GET,
               std::bind(&WebService::enlightInfoHandler, this, std::placeholders::_1));

  webServer.on("/save",
               HTTP_GET,
               std::bind(&WebService::enlightSaveHandler, this, std::placeholders::_1));

  webServer.on("/ota", HTTP_POST, [](AsyncWebServerRequest *request){
                 request->send(200); // Looks like it doesn't really works
               },
               std::bind(&WebService::enlightOtaHandler, this,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3,
                         std::placeholders::_4,
                         std::placeholders::_5,
                         std::placeholders::_6)); // 6 parameters here, so 6 placeholders are necessary
  
  webServer.serveStatic("/", SPIFFS, "/")
      .setTemplateProcessor(std::bind(&WebService::enlightTemplateRenderer, this, std::placeholders::_1))
      .setDefaultFile("index.html");

  // Separate resource out from root, as the template engine will fuck up the CSS
  webServer.serveStatic("/common", SPIFFS, "/ui").setCacheControl("max-age=31536000");

  webServer.begin();
}

/**
 * Iterative all LEDs in the array and copy all colors to it
 * @param ledArray LED array
 * @param color Reference of color in CRGB LED object
 */
void WebService::copyColorToAllLed(CRGBArray<ENLIGHT_LED_COUNT> ledArray, CRGB &color)
{
  log_i("Color: copying color, r=%d, g=%d b=%d", color.r, color.g, color.b);
  for (uint8_t ledIndex = 0; ledIndex < ledArray.size(); ledIndex++) {
    ledArray[ledIndex] = color;
  }

  fastLED->show();
}

void WebService::enlightResetHandler(AsyncWebServerRequest *request)
{

  if (request->hasArg("factory") && request->arg("factory").equals("true")) {
    preferences->putBool(ENLIGHT_NVRAM_INIT_FLAG, true);

    log_w("Reset: Got factory reset request, will reboot and continue!");
    ESP.restart();


  } else {
    request->send(400, "text/plain", "Bad Request");
  }


}

void WebService::enlightSwitchHandler(AsyncWebServerRequest *request)
{

  if (request->hasArg("switch") && request->arg("switch").equals("on")) {
    fastLED->setBrightness((uint8_t) preferences->getUInt(ENLIGHT_NVRAM_LED_BRIGHTNESS, 255));
    request->send(200, "text/plain", "OK");
  } else if (request->hasArg("switch") && request->arg("switch").equals("off")) {
    fastLED->setBrightness(0);
    request->send(200, "text/plain", "OK");
  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void WebService::enlightColorHandler(AsyncWebServerRequest *request)
{
  if (request->hasArg("value")
      && request->arg("value").charAt(0) == '#') {

    log_i("Color: got request raw value: %s", request->arg("value").c_str());

    // Create the string and remove the first '#' symbol
    String colorStr = request->arg("value");
    colorStr.remove(0, 1);

    log_i("Color: got string value after remove(0): %s", colorStr.c_str());

    uint32_t colorValue = (uint32_t) strtol(colorStr.c_str(), nullptr, 16);

    log_i("Color: converted to uint32_t: %u", colorValue);

    CRGB color = CRGB(colorValue);

    copyColorToAllLed(enlightArray, color);

    request->send(200, "text/plain", "OK");

  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void WebService::enlightColorTempHandler(AsyncWebServerRequest * request)
{
  if (request->hasArg("value") 
      && request->arg("value").toInt() <= 40000
      && request->arg("value").toInt() >= 1000) {

    CRGB color = Color::GetRgbFromColorTemp(request->arg("value").toInt());

    log_d("Color: got color %lu...", request->arg("value").toInt());

    copyColorToAllLed(enlightArray, color);
    fastLED->show();

    request->send(200, "text/plain", "OK");

  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void WebService::enlightBrightnessHandler(AsyncWebServerRequest *request)
{

  log_d("Brightness: got brightness request...\n");

  if (request->hasArg("value")) {

    uint8_t value = (uint8_t) request->arg("value").toInt();
    log_d("Brightness: setting to %d...", value);
    fastLED->show(value);
    request->send(200, "text/plain", "OK");


  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

void WebService::enlightSettingHandler(AsyncWebServerRequest *request)
{

  // Save to NVRAM
  // WiFi SSID
  if (request->hasArg(ENLIGHT_NVRAM_WIFI_SSID)) {
    log_i("Preference: WiFi SSID has changed, new value: %s\n", request->arg("wifi_ssid"));
    preferences->putString(ENLIGHT_NVRAM_WIFI_SSID, request->arg(ENLIGHT_NVRAM_WIFI_SSID));
  }

  // WiFi Password
  if (request->hasArg(ENLIGHT_NVRAM_WIFI_PASSWORD)) {
    log_i("Preference: WiFi password has changed, new value: %s\n", request->arg(ENLIGHT_NVRAM_WIFI_PASSWORD));
    preferences->putString(ENLIGHT_NVRAM_WIFI_PASSWORD, request->arg(ENLIGHT_NVRAM_WIFI_PASSWORD));
  }

  // Set init flag to true
  log_i("Preference: setting LED init flag to true...\n");
  preferences->putBool(ENLIGHT_NVRAM_INIT_FLAG, false);

  request->send(200, "text/plain", "OK");
}

void WebService::enlightInfoHandler(AsyncWebServerRequest *request)
{
  // Get device name
  String deviceName = String((unsigned long) ESP.getEfuseMac(), HEX);
  deviceName = deviceName.substring(deviceName.length() - 6);

  // Encode JSON and send it to user
  auto *response = new AsyncJsonResponse();
  JsonObject &infoObject = response->getRoot();

  // Add information to buffer
  infoObject["led_count"] = ENLIGHT_LED_COUNT;
  infoObject["sys_inited"] = preferences->getBool(ENLIGHT_NVRAM_INIT_FLAG, false);
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

void WebService::enlightOtaHandler(AsyncWebServerRequest *request, String filename, size_t index,
                                uint8_t *data, size_t len, bool final)
{
  // Originally implemented by JMishou, ref: https://gist.github.com/JMishou/60cb762047b735685e8a09cd2eb42a60
  // if index == 0 then this is the first frame of data
  if(!index){
    log_i("OTA: got image: %s\n", filename.c_str());

    if(!updater.begin()){
      log_e("OTA: congrats, you've fucked up with an error code: ", updater.getError());

      String errorStr = "<script>alert('Firmware upgrade failed, error code: ";
      errorStr += updater.getError();
      errorStr += "');</script>";
      request->send(200, "text/html", errorStr.c_str());
      updater.printError(Serial);

      delay(1000);
      ESP.restart();
    }
  }

  // Write chunked data to the free sketch space
  if(updater.write(data, len) != len){
    log_e("OTA: congrats, you've fucked up with an error code: ", updater.getError());
    String errorStr = "<script>alert('Firmware upgrade failed, error code: ";
    errorStr += updater.getError();
    errorStr += "');</script>";
    request->send(200, "text/html", errorStr.c_str());
    updater.printError(Serial);

    delay(1000);
    ESP.restart();
  }

  // If the final flag is set then this is the last frame of data
  if(final) {
    if(updater.end(true)) {
      request->send(200, "text/html",
                    "<script>"
                        "alert('Firmware updated, please wait until your device connects to your network again');"
                    "</script>");

      log_i("OTA: %u bypes written, rebooting...", index+len);

      // Add some delay to wait until the request send out
      delay(1000);
      ESP.restart();
    } else {
      updater.printError(Serial);
      request->send(500, "text/plain", String("Error, code #" + updater.getError()));
    }
  }
}

void WebService::enlightSaveHandler(AsyncWebServerRequest *request)
{
  CRGB color = *fastLED->leds();
  if(request->hasArg("cmd") && request->arg("cmd").equals("save")) {

    log_i("Save: saving current LED settings to NVRAM...");
    Helpers::setColorToNvram(color, preferences);
    preferences->putUInt(ENLIGHT_NVRAM_LED_BRIGHTNESS, fastLED->getBrightness());
    request->send(200, "text/plain", "OK");

  } else if(request->hasArg("cmd") && request->arg("cmd").equals("reset")) {
    
    log_i("Save: resetting default LED settings to NVRAM...");
    preferences->putUInt(ENLIGHT_NVRAM_LED_COLOR, 0xFFFFFF);
    preferences->putUInt(ENLIGHT_NVRAM_LED_BRIGHTNESS, 255);
    request->send(200, "text/plain", "OK");
    
  } else {
    request->send(400, "text/plain", "Bad Request");
  }
}

String WebService::enlightTemplateRenderer(const String &var)
{
  // Return WiFi SSID
  if (var == "WIFI_SSID") {
    return (String((WiFi.SSID().length() < 1) ? ENLIGHT_DEFAULT_WIFI_SSID : WiFi.SSID()));
  }

  // Return WiFi RSSI value in percentage
  if (var == "WIFI_RSSI") {
    return (String(2 * (WiFi.RSSI() + 100)) + "  %");
  }

  // Return current IP address
  if (var == "WIFI_IP") {
    return ((WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA) ?
            WiFi.softAPIP().toString() :
            WiFi.localIP().toString());
  }

  // Return firmware version
  if (var == "FIRM_VERSION") {
    return (ENLIGHT_VERSION_FULL);
  }

  // Return serial number (EfuseMac)
  if (var == "EFUSE_SN") {
    return (String((unsigned long) ESP.getEfuseMac(), HEX));
  }

  return String();
}


