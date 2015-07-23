#include <Wire.h>  //Include the I2C wire library
#include "pin_defines.h"
#include <msp430g2553.h>

const unsigned int MODULE_ID = 0047;        //Module id, defines the type of module
const unsigned int MODULE_VERSION = 2;      //Defines the version of the module

#define BASE_I2C_ADDRESS 0x12  //Base i2c address for this device

void setup(){ 
  //Pins for enabling the speed of the motors
  pinMode(MOTOR0_A, OUTPUT);
  pinMode(MOTOR0_B, OUTPUT);
  pinMode(MOTOR1_A, OUTPUT);
  pinMode(MOTOR1_B, OUTPUT);
  
  //Set some default values for the pins
  analogWrite(MOTOR0_A, 0);
  analogWrite(MOTOR0_B, 0);
  analogWrite(MOTOR1_A, 0);
  analogWrite(MOTOR1_B, 0);
  
  //Setup flag pin
  pinMode(pin_flag, OUTPUT);
  digitalWrite(pin_flag, LOW);
  
  //Address selecting and flag pins
  pinMode(pin_addr_sel_0, INPUT_PULLDOWN);
  pinMode(pin_addr_sel_1, INPUT_PULLDOWN);
  pinMode(pin_addr_sel_2, INPUT_PULLDOWN);
  //********************************
  
  delay(1);
  
  //Setup i2c connection
  int slaveAddress = BASE_I2C_ADDRESS;
  if(digitalRead(pin_addr_sel_0) == HIGH)    //A0
    slaveAddress += 1;
  if(digitalRead(pin_addr_sel_1) == HIGH)    //A1
    slaveAddress += 2;
  if(digitalRead(pin_addr_sel_2) == HIGH)    //A2
    slaveAddress += 4;

  Wire.begin(slaveAddress);                // This device to join I2C bus as slave with address as defined by address pins
  Wire.onReceive(receiveEvent);           // register function call for when data is received from I2C Master
  Wire.onRequest(requestEvent);           // register function call for when data is requested to I2C Master
}

void loop(){    
  /*
  delay(10);
  
  set_dir(0, 0);
  set_dir(1, 0);
  
  for(int i = 0; i < 25; i ++ ){
    set_speed(0, (byte)(i*10));
    set_speed(1, (byte)(250 - i*10));
    delay(200);
  }
  
  set_dir(0, 1);
  set_dir(1, 1);
  
  for(int i = 0; i < 25; i ++ ){
    set_speed(0, (byte)(250 - i*10));
    set_speed(1, (byte)(i*10));
    delay(200);
  }
  */
}

