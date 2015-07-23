
#include <Servo.h>  //Include the servo library
#include <Wire.h>  //Include the I2C wire library
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

const unsigned int MODULE_ID = 1001;        //Module id, defines the type of module
const unsigned int MODULE_VERSION = 8;      //Defines the version of the module

//Command codes for I2C Message protocol, CC stands for command code
#define COMMAND_CODE_TEST  0x01
#define COMMAND_CODE_MODULE_ID  0x02
#define COMMAND_CODE_MODULE_VERSION  0x03
#define COMMAND_CODE_MODULE_RESET  0x04
#define COMMAND_CODE_WHAT_FLAG  0x05
#define COMMAND_CODE_CLEAR_FLAG 0x06

#define COMMAND_CODE_SET_SERVO_PULSE_WIDTH  0x10         //Set servo to requested pulse width between min/max values as attached, Packet format(4 bytes): CC  |  Servo#(3Bits) + PWM_Upper(5Bits)  |  PWM(Lower)

#define COMMAND_CODE_REQUEST_SERVO_PULSE_WIDTH  0x11     //Request a servo PWM, Packet format(2 bytes): CC  |  Servo#
                                                         //Return data(2 bytes):  PWM_Upper  |  PWM_Lower

#define BASE_I2C_ADDRESS 0x11

#define SERVO_COUNT  8

#define FLAG_TYPE_ERROR_NONE  0x01
#define FLAG_TYPE_ERROR_GENERAL  0x02
#define FLAG_TYPE_ERROR_PACKET 0x10
#define FLAG_TYPE_ERROR_SERVO 0x11

byte current_flag_type = 0x01;

#define MIN_PULSE_WIDTH  544            //Min pulse width value
#define MAX_PULSE_WIDTH  2400        //Max pulse width value
#define DEFAULT_PULSE_WIDTH  1500    //Default pulse width value when attached

//Stores the servo pin numbers
const unsigned int SERVO_PIN_ARRAY[] = {
  2,3,4,5,6,7,8,9};

//Stores the 8 servo objects
Servo servos[8];  

//Stores command code for request data sending, see command code list above
unsigned int current_command_code = 0; 

//A buffer array for the read in bytes from I2C Master
unsigned char data_read_array[16] = {
  0};

//Size of the data to be written to I2C Master
unsigned int data_write_size = 0;

//A buffer array for the write bytes to the I2C Master
unsigned char data_write_array[16] = {
  0};
    
    
unsigned int count;
unsigned int i;

unsigned int servo_num;
unsigned int pwm;

void setup() {
  
  pinMode(10, INPUT);      //Put alert pin to input, so there it doesnt cause problems with alert line

  pinMode(11, INPUT_PULLDOWN);   //A0
  pinMode(12, INPUT_PULLDOWN);   //A1
  pinMode(13, INPUT_PULLDOWN);   //A2

  delay(1);

  int slaveAddress = BASE_I2C_ADDRESS;
  if(digitalRead(11) == HIGH)    //A0
    slaveAddress += 1;
  if(digitalRead(12) == HIGH)    //A1
    slaveAddress += 2;
  if(digitalRead(13) == HIGH)    //A2
    slaveAddress += 4;

  Wire.begin(slaveAddress);                // This device to join I2C bus as slave with address as defined by address pins
  Wire.onReceive(receiveEvent);           // register function call for when data is received from I2C Master
  Wire.onRequest(requestEvent);           // register function call for when data is requested to I2C Master
}

void loop() 
{  
  delay(2000);  //Program loop  
} 


//Receiver data event
void receiveEvent(int bytes)
{ 
  if(bytes > 0){                          //Check is the packet is not empty
    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is

    switch(current_command_code){        //Switch statement to parse code based on the command code

    case COMMAND_CODE_SET_SERVO_PULSE_WIDTH:
      count = 2;
      readI2C(&count);
      putCRC(data_read_array, 2);
            
      servo_num =  0x07 & (data_read_array[0] >> 5);          //Extract the 3 bits used to identify the servo_num
      pwm = ((data_read_array[0] & 0x1F) << 8) | data_read_array[1];  //Combine the pwm value lower 5bits from first byte and all second byte combined
      
      set_servo_pulse_width(&servo_num, &pwm);    
     
      break;

    case COMMAND_CODE_REQUEST_SERVO_PULSE_WIDTH:

      count = 1;
      readI2C(&count);
      
      servo_num = data_read_array[0];
      
      if((0 <= servo_num) & (servo_num < SERVO_COUNT)){    //Check if servo number falls within the range
        pwm = servos[servo_num].readMicroseconds();      
        data_write_array[0] = (pwm >> 8) & 0xFF;
        data_write_array[1] = pwm & 0xFF;
        data_write_size = 2;    
      }
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
      
    case COMMAND_CODE_WHAT_FLAG:
      data_write_array[0] = current_flag_type;
      data_write_size = 1;
      break;

    case COMMAND_CODE_CLEAR_FLAG:
      //TODO: Remove flag queue incase there is multiple flag
      digitalWrite(10, LOW);                //Turn pin low
      pinMode(10, INPUT);                  //Change pin mode to input
      current_flag_type = FLAG_TYPE_ERROR_NONE;
      break;
      
      
    default:                                //Case when the command code doesnt match any of the ones test, 
      //Set alert pin to high
      pinMode(10, OUTPUT);                  //Turn pin to high
      digitalWrite(10, HIGH);               //Set alert pin high
      current_flag_type = FLAG_TYPE_ERROR_PACKET;
      break;                                
    }
    
    //Clear out remaining bytes, prevents I2C from locking up
    while(Wire.available() > 0){
      Wire.read();
    }  
  }
}



//Request data event
//This function is called when the I2C Master requests data from the I2C slave device(this device)
//The data to write and size must be set before the I2C Master requests data, else the requested data will be junk
void requestEvent()
{
  Wire.write(data_write_array, data_write_size);    //Write data to the I2C Master. data_write_array contains data bytes to write
                                                    //data_write_size contains the number of bytes to write
  data_write_size = 0;                              //After writing data, set the data_write_size to 0
}


//Function call that will read data from the I2C Master to the I2C slave device(this device)
//One parameter is needed, the number of bytes to read
void readI2C(unsigned int* count){
  for(unsigned int i = 0; i < *count; i ++){      //Loop that reads in bytes based on the count value
    data_read_array[i] = Wire.read();  //Read the available byte into the data_read_array
  } 
}

void putCRC(unsigned char *data, unsigned int length)
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
} 

boolean set_servo_pulse_width(unsigned int* servo_num, unsigned int* pwm){ 
  //Check if servo value falls within range
  if(!((0 <= *servo_num) & (*servo_num < SERVO_COUNT))){ 
    return false;
  }
  
  //If pwm is 0, then detach the servo from pulsing
  if( *pwm == 0 ){
     if(servos[*servo_num].attached() == true){              
      servos[*servo_num].detach();    //Attach the servo
     }
     return true;
  }
  
  //Check if pwm values fall within range, if not, return false
  if((MIN_PULSE_WIDTH > *pwm) | (*pwm > MAX_PULSE_WIDTH)){
    return false;
  }
  
  //Check if the servo is attached before changing the pwm, if not then attach servo
  if(servos[*servo_num].attached() == false){    
    servos[*servo_num].attach(SERVO_PIN_ARRAY[*servo_num], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);    //Attach the servo
  }
  
  //Set the servo to the requested pwm
  servos[*servo_num].writeMicroseconds(*pwm);
  return true; 
}































