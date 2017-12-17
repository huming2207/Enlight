#include "main.h"

uint64_t chipid;

void setup()
{
    Serial.begin(9600);
    log_i("System boots, serial begins");
}

void loop()
{
  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  //Serial.printf("ESP32 Chip ID = %04X\n",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%05x\n",(uint16_t)chipid);//print Low 4bytes.

  delay(3000);

}