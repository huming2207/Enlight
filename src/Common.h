//
// Created by Ming Hu on 12/12/17.
//

#ifndef ENLIGHT_COMMON_H
#define ENLIGHT_COMMON_H

#include <cstdint>
#include <FastLED.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ESP32WebServer.h>
#include "Hardware.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#if !CONFIG_FREERTOS_UNICORE
#define HAL_APP_RUNNING_CORE 0
#else
#define HAL_APP_RUNNING_CORE 1
#endif

#endif //ENLIGHT_COMMON_H
