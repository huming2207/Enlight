//
// Created by Ming Hu on 14/12/17.
//

#ifndef ENLIGHT_HARDWARE_H
#define ENLIGHT_HARDWARE_H

/**
 *
 * NVRAM map:
 *
 * init_net     Main Network initialisation init flag        bool
 * init_led     Main LED initialisation init flag,           bool
 * led_red      Red LED's color,                             uint_8
 * led_grn      Green LED's color,                           uint_8
 * led_blu      Blue LED's color,                            uint_8
 * led_bgt      LED Brightness,                              uint_8
 * wifi_ssid    WiFi SSID                                    string
 * wifi_passwd  WiFi Password                                string         Note: if no password then fill "nu11"
 *
 */

// SK6812/WS2812 LED count on the bus
#define ENLIGHT_LED_COUNT                           6

// SK6812/WS2812 LED data bus pin
#define ENLIGHT_LED_DATA_BUS_PIN                    21

// Default (estimated) LED color temperature
#define ENLIGHT_DEFAULT_LED_COLOR_TEMP              5500

// Default SSID
#define ENLIGHT_DEFAULT_WIFI_SSID                   "enlight"

// Default retry time (in second)
#define ENLIGHT_DEFAULT_RETRY_COUNT                10


#endif //ENLIGHT_HARDWARE_H
