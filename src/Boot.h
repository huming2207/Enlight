//
// Created by Ming Hu on 17/12/17.
//

#ifndef ENLIGHT_NETWORKSERVICE_H
#define ENLIGHT_NETWORKSERVICE_H

#include "Common.h"

class Boot
{
    public:
        bool init();

    private:
        TaskHandle_t bootWebTaskHandle;
        ESP32WebServer webServer;
        void bootWebTask(void * taskParam);
        void Boot::handleFinishPage();

};


#endif //ENLIGHT_NETWORKSERVICE_H
