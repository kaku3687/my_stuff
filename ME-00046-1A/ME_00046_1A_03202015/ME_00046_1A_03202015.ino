#include  <Wire.h>
#include <msp430g2553.h>
#include <Stepper.h>
#include "pin_defines.h"
#include "declarations.h"

const unsigned int MODULE_ID = 1046;        //Module id, defines the type of module
const unsigned int MODULE_VERSION = 1;      //Defines the version of the module


//Address select variables;
unsigned int base=0x13;
unsigned int Addr0,Addr1,Addr2;
unsigned int offset;
byte newAddr;

//Interrupt Variables
unsigned int pin;// used in check_array()

Stepper thisStepper(NUM_STEPS, MOTOR1, MOTOR2, MOTOR3, MOTOR4);

void setup()
{    
 
  pinMode(FLAG_PIN, OUTPUT);
  digitalWrite(FLAG_PIN, LOW);

  pinMode(11,INPUT_PULLDOWN);//Address select 0
  pinMode(12,INPUT_PULLDOWN);//Address select 1
  pinMode(13,INPUT_PULLDOWN);//Address select 2
 
  delay(1);
 
  //Address select 0
  if( digitalRead(11) == 1 ){ 
   Addr0 = 1;
   }
   else Addr0=000;
   //Address select 1
   if( digitalRead(12) == 1){
   Addr1 = 2;
   }
   else Addr1=000;
   //Address select 2
   if( digitalRead(13) == 1 ){
   Addr2 = 4;
   }
   else Addr2=000;

  offset= Addr0+Addr1+Addr2;
  newAddr= offset+base; // Address for slave based on how adress pins are soldered.
  
  Wire.begin(newAddr);                // This device to join I2C bus as slave with address #4
  Wire.onReceive(receiveEvent); // register function call for when data is received from I2C Master
  Wire.onRequest(requestEvent); // register function call for when data is requested to I2C Master
  
  release();

  thisStepper.setSpeed(256);
  
  
}

void loop()
{  
   delay(100);
}
