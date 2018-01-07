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
#define ENLIGHT_LED_DATA_BUS_PIN                    0

// Main key interrupt pin
#define ENLIGHT_MAIN_KEY_INTR_PIN                   15

// Main key LED pin
#define ENLIGHT_MAIN_KEY_LED_PIN                    5

// Main key LED LEDC PWM channel
#define ENLIGHT_MAIN_KEY_LEDC_CHANNEL               0

// Main key LEDC frequency (5KHz from ESP-IDF v3.0 API doc and sample program)
#define ENLIGHT_MAIN_KEY_LEDC_FREQ                  5000

// Main key LEDC resolution bit
#define ENLIGHT_MAIN_KEY_LEDC_BIT                   13

// Shield touch sensor pin
#define ENLIGHT_TOUCH_INTR_PIN                      2

// Shield touch sensivity
#define ENLIGHT_TOUCH_INTR_SENSE                    15

// Default (estimated) LED color temperature
#define ENLIGHT_DEFAULT_LED_COLOR_TEMP              5500

// Default SSID
#define ENLIGHT_DEFAULT_WIFI_SSID                   "enlight"

// Default retry time (in second)
#define ENLIGHT_DEFAULT_RETRY_COUNT                10



#endif //ENLIGHT_HARDWARE_H
