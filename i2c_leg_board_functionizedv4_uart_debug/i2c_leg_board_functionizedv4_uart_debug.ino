#include <SoftwareSerial.h>
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


#define rxPin pin_addr_sel_0
#define txPin pin_addr_sel_1

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
  
  /*
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
  */
  cc_leg_func_init(90, 90, 3, 40, 1, 1, 225, 225, 3, 2, 3, 30);

  SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
  pinMode(rxPin, INPUT_PULLUP);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);
  mySerial.setTimeout(50);
  mySerial.println("Hello World - SoftwareSerial");
  
  char someChar;
  while(1){
    //mySerial.println(analogRead(pin_analog_read_1));
    
    //listen for new serial coming in:
    if (mySerial.available()>0){
      someChar = mySerial.read();
      // print out the character:
      //mySerial.print(someChar);
      
      if(someChar == '0'){
        cc_leg_zero();
        mySerial.println("Zeroing");
      }
      
      else if(someChar == 'u'){
        int int_u = mySerial.parseInt();
        cc_leg_move_z(rot_count - int_u);
        mySerial.print("Up ");
        mySerial.println(int_u);
      }
      
      else if(someChar == 'd'){
        int int_d = mySerial.parseInt();
        cc_leg_move_z(rot_count + int_d);
        mySerial.print("Dn ");
        mySerial.println(int_d);
      }
    }
    
    delay(100);
  }
}


void loop(){

}












