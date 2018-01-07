//
// Created by Ming Hu on 12/12/17.
//

#ifndef ENLIGHT_COMMON_H
#define ENLIGHT_COMMON_H

// Convert the version info from git revision hex digit to string
// From: https://stackoverflow.com/questions/5459868/c-preprocessor-concatenate-int-to-string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Detect if it works on one core only
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// Version definition
// It will be redefined by GCC's "-D" flag later in the PIO build bot
#ifndef ENLIGHT_GIT_REV
#define ENLIGHT_GIT_REV "Unknown"
#endif

#define ENLIGHT_GIT_REV_STR STR(ENLIGHT_GIT_REV)

#define ENLIGHT_VERSION "1.0-"
#define ENLIGHT_VERSION_FULL ENLIGHT_VERSION ENLIGHT_GIT_REV_STR

// NVRAM Settings key maps
// LED color
#define ENLIGHT_NVRAM_LED_COLOR                       "led_color"

// LED Brightness
#define ENLIGHT_NVRAM_LED_BRIGHTNESS                  "led_bgt"

// Initialization flag
#define ENLIGHT_NVRAM_INIT_FLAG                       "enlight_init"

// WiFi SSID
#define ENLIGHT_NVRAM_WIFI_SSID                       "wifi_ssid"

// WiFi Password
// Note: if no password then fill "nu11"
#define ENLIGHT_NVRAM_WIFI_PASSWORD                   "wifi_passwd"

#endif //ENLIGHT_COMMON_H
