//
// Created by hu on 1/20/18.
//

#ifndef ENLIGHT_KEYSERVICE_H
#define ENLIGHT_KEYSERVICE_H

#include <FastLED.h>
#include <Preferences.h>

class KeyService
{
 private:
  CFastLED *fastLED;
  Preferences *preferences;
  bool lastState = false;
  bool shieldTouched = false;
  bool keyPressed = false;

 public:
  void init(CFastLED *led, Preferences *pref);
  void handleKeyLight();
  void handleInput();

};


#endif //ENLIGHT_KEYSERVICE_H
