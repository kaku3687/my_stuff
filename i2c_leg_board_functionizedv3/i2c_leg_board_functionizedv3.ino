#include <TwoMsTimer.h>

#include <Servo.h>
#include <Wire.h>
//#include <math.h>
#include <msp430g2553.h>
#include "pin_defines.h"
#include "declarations.h"

#define BASE_I2C_ADDRESS  0x24
//#define BASE_I2C_ADDRESS  0x28
//#define BASE_I2C_ADDRESS 0x2C

const unsigned int MODULE_ID = 1038;        //Module id, defines the type of module
const unsigned int MODULE_VERSION = 0;      //Defines the version of the module

void setup()
{
  //Pins for enabling the speed of the motors
  pinMode(ANALOG1, OUTPUT);
  pinMode(ANALOG2, OUTPUT);
  pinMode(ANALOG3, OUTPUT);
  pinMode(ANALOG4, OUTPUT);
  
  digitalWrite(ANALOG1, LOW);
  digitalWrite(ANALOG2, LOW);
  digitalWrite(ANALOG3, LOW);
  digitalWrite(ANALOG4, LOW);
  
  //Set the analog read reference
  analogReference(INTERNAL2V5);    //Set analog reference
  vref = 25;                   //Store the reference value to be sent during analogreads
  
  //Sets up the flag pin
  pinMode(pin_flag, OUTPUT);
  digitalWrite(pin_flag, LOW);
  
  //Sets up the address select lines
  pinMode(pin_addr_sel_0, INPUT_PULLDOWN); //A0
  pinMode(pin_addr_sel_1, INPUT_PULLDOWN); //A1
  
  delay(1);

  int slaveAddress = BASE_I2C_ADDRESS;
  if(digitalRead(pin_addr_sel_0) == HIGH)    //A0
    slaveAddress += 1;
  if(digitalRead(pin_addr_sel_1) == HIGH)    //A1
    slaveAddress += 2;

  Wire.begin(slaveAddress);                // This device to join I2C bus as slave with address as defined by address pins
  Wire.onReceive(receiveEvent);           // register function call for when data is received from I2C Master
  Wire.onRequest(requestEvent);           // register function call for when data is requested to I2C Master
  
  //pinMode(2, OUTPUT);
  //digitalWrite(2, LOW);
  //set_servo_pulse_width(0, 1600);
  //set_servo_pulse_width(1, 1600);
  cc_leg_func_init(90, 90, 3, 40, 1, 1, 150, 150, 3, 2, 3, 30);
  //cc_leg_move(110, 110, 3);
  //cc_leg_step(110, 110, -1);
  //z_move(-10);
  
  //cc_set_threshold_enable((byte)1);
  //cc_set_threshold((unsigned int) 500);
  
  /*
  cc_leg_zero();                          //Zeros the leg, based on the limit switch. Sometimes you get different zeros... this could be mechanically the way the switch is being hit?
  while(status == 5){     //Wait until leg zero function has reach zero
   delay(100); 
  }
  */
  /*
  cc_leg_move(0, 0, 20);
  while(status == 3){     //Wait until leg zero function has reach zero
   delay(100); 
  }
  */
  //cc_leg_step(1.5, 1.5, 5);
  
}


void loop(){
  /*
  if(status == 1){            //Check if leg_func is ready
    cc_leg_step(0, 0, 15);                  //Request leg step
  }
  else if( status == 2 ){      //Check if leg_func finished successfully
    cc_walk_status_clear();                    //Clear leg_func status
  }
  else if(status == 4 ){
    cc_walk_status_clear(); 
    delay(1000);
  }
  else if(status == 5){
     delay(100); 
  }
  else{
    walk_timeout();                     //Check if there is a timeout
    delay(100);
  }
  */
  walk_timeout();      //Checks for timeout
  delay(500);
}












