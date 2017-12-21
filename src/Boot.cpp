//
// Created by Ming Hu on 17/12/17.
//

#include <http_parser.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include "Boot.h"
#include "Hardware.h"


/**
 * Initialization function
 * @return True if online, False if not.
 */
bool Boot::init()
{
  // Load configuration and see if there's any existing stuff
  preferences.begin("enlight");

  // Get unique device name from EfuseMac
  String deviceName = String((unsigned long) ESP.getEfuseMac(), HEX);
  deviceName = "enlight-" + deviceName.substring(deviceName.length() - 6);

  // Register mDNS service
  MDNS.begin(deviceName.c_str());
  MDNS.addService("http", "tcp", 80);

  // See if it has been configured or not
  if (!preferences.getBool("init_net", false)) {
    log_w("Network: network is not yet configured, will load in AP mode anyway...");

    if (!WiFi.softAP(ENLIGHT_DEFAULT_WIFI_SSID)) {
      log_e("Network: cannot set WiFi driver to AP mode!");
      return false;
    }

    // Do init
    IPAddress selfIP = WiFi.softAPIP();
    log_i("Network: setup successful, go to http://%s", selfIP.toString().c_str());

    // Set init flag to true
    preferences.putBool("init_net", true);

    // Start web task
    xTaskCreatePinnedToCore(bootInitWebTask, "BootWeb", 32000, nullptr, 2, nullptr, ARDUINO_RUNNING_CORE);
  } else {
    // Continue to connect WiFi
    WiFi.begin(preferences.getString("wifi_ssid", ENLIGHT_DEFAULT_WIFI_SSID).c_str(),
               preferences.getString("wifi_passwd", "").c_str());

    // Wait some time until it connects
    uint8_t retryCount = 0;
    while (WiFi.status() != WL_CONNECTED || retryCount < ENLIGHT_DEFAULT_RETRY_COUNT) {
      log_i("Network: awaiting WiFi AP response, tried %d times...", retryCount);
      delay(1000);
    }

    // Goes to offline mode if
    if (WiFi.status() != WL_CONNECTED) {
      log_e("Network: no WiFi connection, will work in offline mode!");
      return false;
    }

    return true;

  }
}

void Boot::bootInitWebTask(void *taskParam)
{

  webServer.on("/info", [this]()
  {
    this->handleInfoPage();
  });

  webServer.on("/finish", [this]()
  {
    this->handleFinishPage();
  });
}

void Boot::handleInfoPage()
{
  // Create a JSON buffer
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

  // Send to client
  webServer.send(200, "application/json", jsonString);

}

void Boot::handleFinishPage()
{

  bool failedAttempt = false;

  // If no WiFi SSID, then it must be corrupted.
  if (!webServer.hasArg("plain")) {
    webServer.send(400, "text/plain", "Bad Request");
    return;
  } else {
    // Deal with the POSTed JSON text
    JsonObject &settingObject = jsonBuffer.parseObject(webServer.arg("plain"));

    // Save to NVRAM
    if (preferences.putString("wifi_ssid", settingObject["wifi_ssid"].as<String>())) failedAttempt = true;

    if (settingObject["wifi_passwd"].as<String>().length() < 8) {
      // Add a magic place holder if the wifi password string is less than 8 chars
      if (preferences.putString("wifi_passwd", "nu11") < 1) failedAttempt = true;
    } else {
      if (preferences.putString("wifi_passwd", settingObject["wifi_passwd"].as<String>()) < 1) failedAttempt = true;
    }

    // If failed, stop here
    if (failedAttempt) {
      webServer.send(500, "text/plain", "Internal Server Error");
    } else {
      // Clear up the JSON buffer and make something new
      jsonBuffer.clear();
      JsonObject &returnObject = jsonBuffer.createObject();
      returnObject["sys_id"] = String((unsigned long) ESP.getEfuseMac(), HEX);
      returnObject["status"] = "OK";

      // Print to string
      String jsonString;
      returnObject.printTo(jsonString);

      // Send it to client
      webServer.send(200, "application/json", jsonString);
    }
  }

  // Reboot the device after all things are set.
  ESP.restart();

}
