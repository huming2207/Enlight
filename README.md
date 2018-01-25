# Enlight

Yet another WiFi smart lamp, built with Espressif ESP32 micro-controller, SK6812 LED, Arduino/FreeRTOS (as backend), Semantic UI (for frontend) and love ♥️.

## To-do

- Key handling
    - Using interrupt to handle keys (need replacing SK6812 protocol libraries)
- Touch sensor
- Android/iOS app (Xamarin)

## Hardware setup

- Main board: ESP32 rev. 1 
    - NodeMCU-like "X32" board, with ESP32-WROOM module
    - WiFi: 802.11n 150Mbps, AP (Initial mode) + STA (Normal mode)
- Light: SK6812, 23 LEDs in total
- Aluminum shield with human body touch support 
    - Too sensitive, broken, might cancel this function later
- 1x key with built-in LED indicator
- Power supply: 5V @ 1.5A, USB (with UART on data bus)

Two devices (and two only) have been built, running exactly the same firmware

## Software note

- This project depends on these 3rd-party libraries:
    - Web framework: `ESPAsyncWebServer` by @me-no-dev
    - JSON library: ArduinoJson
    - LED control library: FastLED
    - Web UI library: Semantic UI
    - JavaScript (XMLHttpRequest etc.)

- Since there are some conflicts with Semantic UI stylesheet, the web page template placeholder symbol has been changed to "`~`" instead of "`%`". To avoid this issue, other developers may need to change the line `#define TEMPLATE_PLACEHOLDER '%'` to `#define TEMPLATE_PLACEHOLDER '~'` in the file `WebResponseImpl.h` in ESPAsyncWebServer library.

## Some photos

This lamp is the second device I've built, but the level-shifting module hasn't been installed at that time and it has been installed later.

SK6812 LEDs may works on a data line with 3.3v level voltage, but they are not very stable (may keeps blinking, resetting, etc.) 

According to the datasheet, minimum level voltage of SK6812 data cable should be 3.6v

![1.jpg](https://raw.githubusercontent.com/huming2207/Enlight/3b351d4527d3104cadc8cd1cb60aa3312adc01f9/readme_static_files/1.jpg)
![2.jpg](https://raw.githubusercontent.com/huming2207/Enlight/3b351d4527d3104cadc8cd1cb60aa3312adc01f9/readme_static_files/2.jpg)
![3.jpg](https://raw.githubusercontent.com/huming2207/Enlight/3b351d4527d3104cadc8cd1cb60aa3312adc01f9/readme_static_files/3.jpg)
![4.jpg](https://raw.githubusercontent.com/huming2207/Enlight/3b351d4527d3104cadc8cd1cb60aa3312adc01f9/readme_static_files/4.jpg)
![5.jpg](https://raw.githubusercontent.com/huming2207/Enlight/3b351d4527d3104cadc8cd1cb60aa3312adc01f9/readme_static_files/5.jpg)
![6.jpg](https://raw.githubusercontent.com/huming2207/Enlight/3b351d4527d3104cadc8cd1cb60aa3312adc01f9/readme_static_files/6.jpg)



