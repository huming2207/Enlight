//
// Created by Ming Hu on 17/12/17.
//

#include "Boot.h"

/**
 * Initialization function
 * @return True if online, False if not.
 */
bool Boot::init()
{
    // Load configuration and see if there's any existing stuff
    Preferences preferences;
    preferences.begin("enlight_net");

    // See if it has been configured or not
    if(!preferences.getBool("configured", false))
    {
        log_w("Network: network is not yet configured, will load in AP mode anyway...");
        if(!WiFi.softAP(ENLIGHT_DEFAULT_WIFI_SSID))
        {
            log_e("Network: cannot set WiFi driver to AP mode!");
            return false;
        }

        // Do init
        IPAddress selfIP = WiFi.softAPIP();
        log_i("Network: setup successful, go to http://%s", selfIP.toString().c_str());

        // Start web task
        xTaskCreatePinnedToCore(bootInitWebTask, "BootWeb", 32000, nullptr, 2, nullptr, ARDUINO_RUNNING_CORE);
    }
    else
    {
        // Continue to connect WiFi
        WiFi.begin(preferences.getString("wifi_ssid", ENLIGHT_DEFAULT_WIFI_SSID).c_str(),
                   preferences.getString("wifi_passwd", "").c_str());

        // Wait some time until it connects
        uint8_t retryCount = 0;
        while(WiFi.status() != WL_CONNECTED || retryCount < ENLIGHT_DEFAULT_RETRY_COUNT)
        {
            log_i("Network: awaiting WiFi AP response, tried %d times...", retryCount);
            delay(1000);
        }

        // Goes to offline mode if
        if(WiFi.status() != WL_CONNECTED)
        {
            log_e("Network: no WiFi connection, will work in offline mode!");
            return false;
        }

        return true;

    }
}

void Boot::bootInitWebTask(void *taskParam)
{
    webServer.on("/", [this]()
    {
        this->handleInitPage();
    });

    webServer.on("/finish", [this]()
    {
        if(webServer.method() != HTTP_POST)
        {
            webServer.send(403, "text/html", ENLIGHT_STATIC_PAGE_403);
        }
        else
        {
            this->handleFinishPage();
        }
    });
}

void Boot::handleInitPage()
{
    webServer.send(200, "text/html", )
}

void Boot::handleFinishPage()
{
    vTaskDelete(&bootWebTaskHandle);
}
