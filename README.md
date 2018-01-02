# Enlight

Yet another WiFi smart lamp, built with ESP32, SK6812, Arduino/FreeRTOS (as backend), TypeScript+Semantic UI (as frontend) and love ♥️.

## More details tbd...

## Note

- This project depends on these 3rd-party libraries:
    - Web framework: `ESPAsyncWebServer` by @me-no-dev
    - JSON library: ArduinoJson
    - LED control library: FastLED
    - Web UI library: Semantic UI
    - TypeScript

- Since there are some conflicts with Semantic UI stylesheet, the web page template placeholder symbol has been changed to "`~`" instead of "`%`". To avoid this issue, other developers may need to change the line `#define TEMPLATE_PLACEHOLDER '%'` to `#define TEMPLATE_PLACEHOLDER '~'` in the file `WebResponseImpl.h` in ESPAsyncWebServer library.

