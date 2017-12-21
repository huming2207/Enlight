//
// Created by Ming Hu on 17/12/17.
//

#ifndef ENLIGHT_NETWORKSERVICE_H
#define ENLIGHT_NETWORKSERVICE_H

#include <WebServer.h>
#include <Preferences.h>
#include "Common.h"


class Boot
{
  public:
    bool init();

  private:
    Preferences preferences;

    TaskHandle_t bootWebTaskHandle;

    WebServer webServer;

    DynamicJsonBuffer jsonBuffer;

    void bootInitWebTask(void *taskParam);

    void handleInfoPage();

    void handleFinishPage();

};

#endif //ENLIGHT_NETWORKSERVICE_H
