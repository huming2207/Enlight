//
// Created by Ming Hu on 17/12/17.
//

#ifndef ENLIGHT_NETWORKSERVICE_H
#define ENLIGHT_NETWORKSERVICE_H

#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include "Common.h"


class Boot
{
  public:
    bool init();

  private:
    Preferences preferences;

    AsyncWebServer webServer;

    void bootInitWebTask();

    void handleInfoPage(AsyncWebServerRequest *request);

    void handleFinishPage(AsyncWebServerRequest *request);

};

#endif //ENLIGHT_NETWORKSERVICE_H
