#include  <Wire.h>
#include <msp430g2553.h>

const unsigned int MODULE_ID = 1003;        //Module id, defines the type of module
const unsigned int MODULE_VERSION = 1;      //Defines the version of the module

unsigned char vref = 33;

//Address select variables;
unsigned int base=0x10;
unsigned int Addr0,Addr1,Addr2;
unsigned int offset;
byte newAddr;

//Interrupt Variables
unsigned int pin;// used in check_array()
unsigned int FLAG_pin=8;
unsigned int edgeType;
unsigned int resType;

//Interrupt array
typedef void (* pin_interrupt) (void);
pin_interrupt pin_interrupt_array[] = {NULL, NULL, digital_interrupt_pin2, digital_interrupt_pin3, digital_interrupt_pin4, 
                                       digital_interrupt_pin5, digital_interrupt_pin6, digital_interrupt_pin7, NULL, digital_interrupt_pin9,
                                       digital_interrupt_pin10, NULL, NULL, digital_interrupt_pin13, NULL,
                                       NULL, NULL, NULL, NULL, digital_interrupt_pin19};

boolean pins_interrupted[20]={    
  false};

void setup()
{
  analogReference(INTERNAL2V5);    //Set analog reference
  vref = 25;                   //Store the reference value to be sent during analogreads
  
  pinMode(FLAG_pin, INPUT);

  pinMode(11,INPUT_PULLDOWN);//Address select 0
  pinMode(12,INPUT_PULLDOWN);//Address select 1
  pinMode(18,INPUT_PULLDOWN);//Address select 2
 
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
   if( digitalRead(18) == 1 ){
   Addr2 = 4;
   }
   else Addr2=000;

  offset= Addr0+Addr1+Addr2;
  newAddr= offset+base; // Address for slave based on how adress pins are soldered.
  
  Wire.begin(newAddr);                // This device to join I2C bus as slave with address #4
  Wire.onReceive(receiveEvent); // register function call for when data is received from I2C Master
  Wire.onRequest(requestEvent); // register function call for when data is requested to I2C Master

}

void loop()
{
   delay(100);
}
