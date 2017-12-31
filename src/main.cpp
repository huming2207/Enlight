#include "main.h"
#include "Service.h"

#include <Arduino.h>


uint64_t chipid;

Service service;

void setup() {

  Serial.begin(9600);
  log_i("System boots, serial begins");

  service.init();

}

void loop() {

  // Temporarily disable main loop until when
  delay(32767);

}