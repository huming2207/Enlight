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
  int current;
  int previous;
  long millis_held;    // How long the button was held (milliseconds)
  long secs_held;      // How long the button was held (seconds)
  long prev_secs_held; // How long the button was held in the previous check
  unsigned long firstTime; // how long since the button was first pressed
  bool setBright;

 public:
  void init(CFastLED *led, Preferences *pref);
  void handleInput();

};


#endif //ENLIGHT_KEYSERVICE_H
