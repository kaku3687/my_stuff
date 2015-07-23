#include <Energia.h>
#include <Wire.h>
#include "pin_definitions.h"
#include "command_code_definitions.h"

#define BASE_I2C_ADDRESS  0x24



void setup()
{
  //Sets up the address select lines
  pinMode(PIN_ADDR_SEL_0, INPUT_PULLDOWN); //A0
  pinMode(PIN_ADDR_SEL_1, INPUT_PULLDOWN); //A1
  
  delay(1);

  int slaveAddress = BASE_I2C_ADDRESS;
  if(digitalRead(PIN_ADDR_SEL_0) == HIGH)    //A0
    slaveAddress += 1;
  if(digitalRead(PIN_ADDR_SEL_1) == HIGH)    //A1
    slaveAddress += 2;

  Wire.begin(slaveAddress);                // This device to join I2C bus as slave with address as defined by address pins
  Wire.onReceive(receiveEvent);           // register function call for when data is received from I2C Master
  Wire.onRequest(requestEvent);           // register function call for when data is requested to I2C Master
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(100);
}
