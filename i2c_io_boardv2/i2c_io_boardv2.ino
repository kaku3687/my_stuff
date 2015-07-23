#include  <Wire.h>
#include <msp430g2553.h>

static uint16_t crc_table [256] = {

0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,
0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,
0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,
0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6,
0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,
0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,
0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2,
0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};




const unsigned int MODULE_ID = 1003;        //Module id, defines the type of module
const unsigned int MODULE_VERSION = 1;      //Defines the version of the module

//Command codes for I2C Message protocol, CC stands for command code
#define COMMAND_CODE_TEST                      0x01
#define COMMAND_CODE_MODULE_ID                 0x02
#define COMMAND_CODE_MODULE_VERSION            0x03
#define COMMAND_CODE_MODULE_RESET              0x04

#define COMMAND_CODE_DIGITAL_WRITE_PIN         0x19
#define COMMAND_CODE_ANALOG_WRITE_PIN          0x20
#define COMMAND_CODE_DIGITAL_READ              0x21
#define COMMAND_CODE_ANALOG_READ               0x22
#define COMMAND_CODE_DIGITAL_INTERRUPT         0x25
#define COMMAND_CODE_DIGITAL_INTERRUPT_CHECK   0x26
#define COMMAND_CODE_DIGITAL_INTERRUPT_CLEAR   0x27

#define COMMAND_CODE_SET_VREF                  0x28

unsigned char vref = 33;

//Stores command code for request data sending, see command code list above
unsigned int current_command_code = 0; 

//A buffer array for the write bytes to the I2C Master
unsigned char data_write_array[16] = { 
  0};
  
//A buffer array for the read in bytes from I2C Master
unsigned char data_read_array[16] = {
  0};

boolean pins_interrupted[20]={    
  false};

//Size of the data to be written to I2C Master
unsigned int data_write_size = 0;

//Address select variables;
unsigned int base=0x14;
unsigned int Addr0,Addr1,Addr2;
unsigned int offset;
byte newAddr;

//Interrupt Variables
unsigned int pin;// used in check_array()
unsigned int FLAG_pin=10;
unsigned int edgeType;
unsigned int resType;

//Interrupt array
typedef void (* pin_interrupt) (void);
pin_interrupt pin_interrupt_array[] = {NULL, NULL, digital_interrupt_pin2, digital_interrupt_pin3, digital_interrupt_pin4, 
                                       digital_interrupt_pin5, digital_interrupt_pin6, digital_interrupt_pin7, digital_interrupt_pin8, digital_interrupt_pin9,
                                       NULL, NULL, NULL, NULL, NULL,
                                       NULL, NULL, NULL, digital_interrupt_pin18, digital_interrupt_pin19};

void setup() { 
  analogReference(INTERNAL2V5);    //Set analog reference
  vref = 25;                   //Store the reference value to be sent during analogreads
  
  pinMode(FLAG_pin, INPUT);

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
  

}

void loop() {
  //Program loop  
  delay(100);  
} 

void receiveEvent(int bytes){
  if(bytes >0){

    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is

    switch(current_command_code){

    //   0x19 Digitaly write to pins  
    case COMMAND_CODE_DIGITAL_WRITE_PIN: 
      readI2C(2); 
      //putCRC(data_read_array, 2);
      putCRC_read_only(current_command_code, data_read_array, 2);
      cc_digital_write(data_read_array[0], data_read_array[1]);
      
      break;
    
    //  0x20 Write to pins 4,9,or 19
    case COMMAND_CODE_ANALOG_WRITE_PIN:  
      readI2C(2);  
      //putCRC(data_read_array, 2);
      putCRC_read_only(current_command_code, data_read_array, 2);
      cc_analog_write(data_read_array[0],data_read_array[1]);
      
      break;
    
    //  0x21 Read either 0 or 1 from
    case COMMAND_CODE_DIGITAL_READ:   
      data_write_size=2;// writes out 2 bits to master.
      // 1st bit for pin #. 
      //2nd bit  sets either INPUT (0x00) or INPUT_PULLUP(0x02) or INPUT_PULLDOWN(0x04). 
      readI2C(2); 
      cc_digital_read(data_read_array[0], data_read_array[1]);
      break;
      
    case COMMAND_CODE_SET_VREF:
      readI2C(1);
      putCRC_read_only(current_command_code, data_read_array, 1); 
      cc_set_vref(data_read_array[0]);
      break;
      
    case COMMAND_CODE_ANALOG_READ: //  0x22   *************************************************************************************************************************    
      // 1st bit for pin #.
     // 2nd bit  sets either INPUT (0x00) or INPUT_PULLUP(0x02) or INPUT_PULLDOWN(0x04). 
      readI2C(2);   
      data_write_size = 4;// writes out 4 bits to master.
      cc_analog_read(data_read_array[0], data_read_array[1]);
      break;
      
    case COMMAND_CODE_DIGITAL_INTERRUPT: //   0x25  *************************************************************************************************
      //FIRST bit is pin #. 
      //SECOND bit is Resistor either INPUT (0x00) . INPUT_PULLUP(0x02). INPUT_PULLDOWN(0x04). 
      //THIRD bit is Trigger interrupt on RISING (0x03) edge or FALLING edge (0x02)
      readI2C(3); 
      //putCRC(data_read_array, 3);
      putCRC_read_only(current_command_code, data_read_array, 3);
      // writes out 1 bits to master.
      // choosing Resistor Type
      cc_digital_interrupt(data_read_array[0], data_read_array[1], data_read_array[2]); 
      break;

    //   0x26  Checkts status of interupted pins array writes to master what pin has flagged the interrupt.
    case COMMAND_CODE_DIGITAL_INTERRUPT_CHECK:  
      data_write_size = 1; // writes out 1 bits to master.
      data_write_array[0] = check_array();
    break;

     //   0x27 dettaches interrupt on selected pin.
    case COMMAND_CODE_DIGITAL_INTERRUPT_CLEAR:   
      //pin # to be detached from interrupt and cleared from pins interrupted array  
      readI2C(1); 
      //putCRC(data_read_array, 1);
      putCRC_read_only(current_command_code, data_read_array, 3);
      cc_digital_interrupt_clear(data_read_array[0]);   
      break;
    
     case COMMAND_CODE_TEST:              //Case when command code is COMMAND_CODE_TEST      
      data_write_array[0] = 0x69;        //Write 1 for testing i2c connection
      data_write_size = 1;
      break;

    case COMMAND_CODE_MODULE_ID:
      data_write_array[0] = (MODULE_ID >> 8) & 0xFF;
      data_write_array[1] = MODULE_ID & 0xFF;
      data_write_size = 2;
      break;
      
    case COMMAND_CODE_MODULE_VERSION:
      data_write_array[0] = (MODULE_VERSION >> 8) & 0xFF;
      data_write_array[1] = MODULE_VERSION & 0xFF;
      data_write_size = 2;
      break;

    case COMMAND_CODE_MODULE_RESET:
      WDTCTL = 0;  //Forces a software reset
      break;  
   
    default:
     break;   
    } 
    
    /*
    while(Wire.available() > 0){
      Wire.read();
    }
    */
    
  } 
}

//Request data event
//This function is called when the I2C Master requests data from the I2C slave device(this device)
//The data to write and size must be set before the I2C Master requests data, else the requested data will be junk
void requestEvent()
{
  Wire.write(data_write_array, data_write_size);    //Write data to the I2C Master. data_write_array contains data bytes to write, data_write_size contains the number of bytes to wr
  data_write_size = 0;                              //After writing data, set the data_write_size to 0
}

//Function call that will read data from the I2C Master to the I2C slave device(this device)
//One parameter is need, the number of bytes to read
void readI2C(unsigned int count){
  for(unsigned int i = 0; i < count; i ++){      //Loop that reads in bytes based on the count value
    data_read_array[i] = Wire.read();  //Read the available byte into the data_read_array
  } 
}

unsigned int getCRC_read_array(unsigned char command_code, unsigned char *data, unsigned int length){
  unsigned int count;
  unsigned int crc = 0xffff;
  unsigned int temp;

  temp = (command_code ^ (crc >> 8)) & 0xff;
  crc = crc_table[temp] ^ (crc << 8);

  for (count = 0; count < length; ++count)
   {
     temp = (*data++ ^ (crc >> 8)) & 0xff;
     crc = crc_table[temp] ^ (crc << 8);
   }
   
  crc = (crc ^ 0x0000);
   
  return crc;
}

unsigned int getCRC_write_array(unsigned int packet_crc, unsigned char *data, unsigned int length){
  unsigned int count;
  unsigned int crc = 0xffff;
  unsigned int temp;

  temp = (((crc >> 8) & 0xFF) ^ (crc >> 8)) & 0xff;
  crc = crc_table[temp] ^ (crc << 8);
  
  temp = ((crc & 0xFF) ^ (crc >> 8)) & 0xff;
  crc = crc_table[temp] ^ (crc << 8);

  for (count = 0; count < length; ++count)
   {
     temp = (*data++ ^ (crc >> 8)) & 0xff;
     crc = crc_table[temp] ^ (crc << 8);
   }
   
  crc = (crc ^ 0x0000);
   
  return crc;
}

void putCRC(unsigned crc, unsigned char start_index){
   data_write_array[start_index] = (crc >> 8) & 0xFF;
   data_write_array[start_index + 1] = crc & 0xFF;
   data_write_size = data_write_size + 2;
}

void putCRC_read_only(unsigned char command_code, unsigned char *data, unsigned int length){
  putCRC(getCRC_read_array(command_code, data, length), 0);
}

void putCRC_read_write_only(unsigned char command_code, unsigned char *data, unsigned int length){
  putCRC(getCRC_read_array(command_code, data, length), length);
}

/*
unsigned int putCRC_write_only(unsigned char *data, unsigned int length)
{ 
   unsigned int count;
   unsigned int crc = 0xffff;
   unsigned int temp;

   temp = (current_command_code ^ (crc >> 8)) & 0xff;
   crc = crc_table[temp] ^ (crc << 8);

   for (count = 0; count < length; ++count)
   {
     temp = (*data++ ^ (crc >> 8)) & 0xff;
     crc = crc_table[temp] ^ (crc << 8);
   }
   
   crc = (crc ^ 0x0000);
   
   data_write_array[0] = (crc >> 8) & 0xFF;
   data_write_array[1] = crc & 0xFF;
   data_write_size = 2;
   
   return crc;
} 

unsigned int putCRC_write_read(unsigned char *data, unsigned int length){
   unsigned int count;
   unsigned int crc = 0xffff;
   unsigned int temp;

   for (count = 0; count < length; ++count)
   {
     temp = (*data++ ^ (crc >> 8)) & 0xff;
     crc = crc_table[temp] ^ (crc << 8);
   }
   
   crc = (crc ^ 0x0000);
   
   data_write_array[length-1] = (crc >> 8) & 0xFF;
   data_write_array[length] = crc & 0xFF;
   //data_write_size = data_write_size + 2;
   
   return crc;
   
}
*/


void analog_write( unsigned int pinNum, unsigned int value){
  pinMode(pinNum,OUTPUT);
  analogWrite(pinNum,value);
}

// interrupt routines for each pin pass the pin number to digital_interrupt()
void digital_interrupt(int pin){  
  noInterrupts();
  detachInterrupt(pin);
  pins_interrupted[pin]=true; 
  pinMode(FLAG_pin, OUTPUT);
  digitalWrite(FLAG_pin, HIGH);  
  interrupts();  
}

void digital_interrupt_pin2(){digital_interrupt(2);}
void digital_interrupt_pin3(){digital_interrupt(3);}
void digital_interrupt_pin4(){digital_interrupt(4);}
void digital_interrupt_pin5(){digital_interrupt(5);}
void digital_interrupt_pin6(){digital_interrupt(6);}
void digital_interrupt_pin7(){digital_interrupt(7);}
void digital_interrupt_pin8(){digital_interrupt(8);}
void digital_interrupt_pin9(){digital_interrupt(9);}
void digital_interrupt_pin18(){digital_interrupt(18);}
void digital_interrupt_pin19(){digital_interrupt(19);}


//checks if FLAG_pin is on. go throughs pins_interrupted array[] to check if pin is flagged. 
//if no alerts are on turns off alert pin.
byte check_array(){ 
  for(byte i = 2;i <= 19;i++){
    if(pins_interrupted[i]==true){
      pinMode(FLAG_pin, OUTPUT);
      digitalWrite(FLAG_pin, HIGH);
      return i;
    }        
  }
  
  //pinMode(FLAG_pin, OUTPUT);
  //pinMode(FLAG_pin, LOW);
  //pinMode(FLAG_pin, INPUT);
  return 0xff;
}


//

void cc_digital_write(unsigned char & pinNum, unsigned char & state){
  if( ((pinNum >= 2) & (pinNum <= 9)) | (pinNum == 18) | (pinNum == 19)){
    pinMode(pinNum,OUTPUT);
    if(state== 0x01){
      digitalWrite(pinNum,HIGH);
    }
    else{
      digitalWrite(pinNum,LOW);
    }  
  }
}

void cc_analog_write(unsigned char & pinNum, unsigned char & value){
   if( (pinNum == 4) | (pinNum == 9) | (pinNum == 19) ){
    pinMode(pinNum,OUTPUT);
    analogWrite(pinNum, value);
   }
}

void cc_digital_read(unsigned char & pinNum, unsigned char & res){
  if( ((pinNum >= 2) & (pinNum <= 9)) | (pinNum == 18) | (pinNum == 19)){
    switch(res){
      case 0:
        pinMode(pinNum, INPUT);
        break;
      case 2:
        pinMode(pinNum, INPUT_PULLUP);
        break;
      case 4:
        pinMode(pinNum, INPUT_PULLDOWN);
        break;
      default:
        data_write_array[0]=0xff;
        data_write_array[1]=0xff;
        return;
    }
  }
    
  data_write_array[0] = pinNum;
    
  if (digitalRead(pinNum) == HIGH)
    data_write_array[1]=0x01;
  else
    data_write_array[1]=0x00;     
    
  return; 
}

void cc_set_vref(unsigned char & _vref){
  switch(_vref){
    case 33:
     analogReference(DEFAULT);    //Set analog reference
     vref = 33;
     break;
     
    case 15:
     analogReference(INTERNAL1V5);    //Set analog reference
     vref = 15;
     break; 
     
    case 25:
     analogReference(INTERNAL2V5);    //Set analog reference
     vref = 25;
     break; 
     
    case 255:
     analogReference(EXTERNAL);    //Set analog reference
     vref = 255;
     break;
     
    default:
      break;
  }
}

void cc_analog_read(unsigned char & pinNum, unsigned char & res){
  unsigned char analogPin;
  
  switch(pinNum){
    case 2:
      analogPin = A0;
      break; 
    case 3:
      analogPin = A1;
      break; 
    case 4:
      analogPin = A2;
      break; 
    case 5:
      analogPin = A3;
      break; 
    case 6:
      analogPin = A4;
      break; 
    case 7:
      analogPin = A5;
      break; 
    default:
      data_write_array[0]=0xff;
      data_write_array[1]=0xff;
      data_write_array[2]=0xff;
      data_write_array[3]=0xff;
      break;  
  }
  
   switch(res){
    case 0:
      pinMode(analogPin, INPUT);
      break;
    case 2:
      pinMode(analogPin, INPUT_PULLUP);
      break;
    case 4:
      pinMode(analogPin, INPUT_PULLDOWN);
      break;
    default:
      data_write_array[0]=0xff;
      data_write_array[1]=0xff;
      data_write_array[2]=0xff;
      data_write_array[3]=0xff;
      return;
   }
   
  unsigned int aRead = analogRead(analogPin);
  
  data_write_array[0] = pinNum;
  data_write_array[1] = vref;
  data_write_array[2]= (aRead >> 8) & 0x03; 
  data_write_array[3]= 0xFF & aRead;  

  return; 
}

void cc_digital_interrupt(unsigned char & pinNum, unsigned char & res, unsigned char & type){
  if( ((pinNum >= 2) & (pinNum <= 9)) | (pinNum == 18) | (pinNum == 19)){
    detachInterrupt(pinNum);
    int resType, edgeType;
    switch(res){
      case 0:
        resType = INPUT;
        break;
      case 2:
        resType = INPUT_PULLUP;
        break;
      case 4:
        resType = INPUT_PULLDOWN;
        break;
      default:
        data_write_array[0]=0xff;
        return;
    }
    
    switch(type){
      case 2:
        edgeType = FALLING;
        break;
      case 3:
        edgeType = RISING;
        break;
      default:
        data_write_array[0]=0xff;
        return;
    }
    
    pinMode(pinNum, resType);
    attachInterrupt(pinNum, pin_interrupt_array[pinNum], edgeType);   
  }
  
  return;
}

void cc_digital_interrupt_check(){
  
}

void cc_digital_interrupt_clear(unsigned char & pinNum){
  if( ((pinNum >= 2) & (pinNum <= 9)) | (pinNum == 18) | (pinNum == 19)){
    if( pins_interrupted[pinNum] == true){
      pins_interrupted[pinNum] = false;
      detachInterrupt(pinNum); 
      //pinMode(pinNum, OUTPUT);
      //digitalWrite(pinNum, LOW); 
      pinMode(pinNum, INPUT);  // UPDATED SETS PIN TO INPUT WITH NO PULLUP OR PULLDOWN
    }
  }
  
  if(check_array() == 255){
    pinMode(FLAG_pin, OUTPUT);
    digitalWrite(FLAG_pin, LOW); 
    pinMode(FLAG_pin, INPUT);  // UPDATED SETS PIN TO INPUT WITH NO PULLUP OR PULLDOWN
  }
}








