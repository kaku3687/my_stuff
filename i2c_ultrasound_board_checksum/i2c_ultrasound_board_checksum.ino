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

#define COMMAND_CODE_TEST  0x01
#define COMMAND_CODE_MODULE_ID  0x02
#define COMMAND_CODE_MODULE_VERSION  0x03
#define COMMAND_CODE_MODULE_RESET  0x04
#define COMMAND_CODE_WHAT_FLAG  0x05
#define COMMAND_CODE_CLEAR_FLAG 0x06

const unsigned int MODULE_ID = 1010;        //Module id, defines the type of module
const unsigned int MODULE_VERSION = 1;      //Defines the version of the module

#define BASE_I2C_ADDRESS 0x1c

#define photoRes_Pin A0
#define flag_Pin P1_5
#define addr0_Pin P2_0
#define addr1_Pin P2_1
#define trig_Pin P2_2
#define servoX_Pin P2_6
#define servoY_Pin P2_7
#define flashLED_Pin P2_5
#define sonarLED_Pin P2_4
#define echo_Pin P2_3

#define p0_Pin A1
#define p1_Pin A2
#define p2_Pin A3
#define p3_Pin A4

#define MIN_PULSE_WIDTH  544            //Min pulse width value
#define MAX_PULSE_WIDTH  2400        //Max pulse width value
#define DEFAULT_PULSE_WIDTH  1500    //Default pulse width value when attached

#define COMMAND_CODE_SET_SERVO_PULSE_WIDTH  0x10
#define COMMAND_CODE_REQUEST_SERVO_PULSE_WIDTH  0x11
#define COMMAND_CODE_REQUEST_SONAR  0x12
#define COMMAND_CODE_SET_FLASHLED  0x13
#define COMMAND_CODE_ENABLE_SONAR 0x14
#define COMMAND_CODE_DISABLE_SONAR 0x15
#define COMMAND_CODE_REQUEST_FLASHLED 0x16
#define COMMAND_CODE_REQUEST_SONAR_STATE 0x17
#define COMMAND_CODE_SET_FLASHLED_PERCENT 0x18

#define SERVO_COUNT  2

Servo servos[2];
unsigned int servo_num, pwm;
const unsigned int SERVO_PIN_ARRAY[] = {
  servoX_Pin, servoY_Pin};

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance
unsigned char sonarReading = 0;

unsigned long sonarReading_startTime;
bool takingSonarReading = false;
unsigned long sonarReading_timeout_startTime;
unsigned long sonarReading_timeout_time = 500;

unsigned char flashLED_state = 0;
unsigned char flashLED_percent = 255;
unsigned char sonarEnabled = 0;

unsigned int count, i;

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

void setup() {

  pinMode(photoRes_Pin, INPUT);
  pinMode(flag_Pin, INPUT);

  pinMode(servoX_Pin, OUTPUT);
  pinMode(servoY_Pin, OUTPUT);

  pinMode(flashLED_Pin, OUTPUT);
  digitalWrite(flashLED_Pin, LOW);

  //pinMode(p0_Pin, INPUT);
  //pinMode(p1_Pin, INPUT);
  //pinMode(p2_Pin, INPUT);
  //pinMode(p3_Pin, INPUT);

  pinMode(trig_Pin, OUTPUT);
  digitalWrite(trig_Pin, LOW);  
  pinMode(echo_Pin, INPUT);  
  pinMode(sonarLED_Pin, OUTPUT);
  digitalWrite(sonarLED_Pin, LOW);

  pinMode(addr0_Pin, INPUT_PULLDOWN);  //Set A0 to input with pulldown res
  pinMode(addr1_Pin, INPUT_PULLDOWN);  //Set A1 to input with pulldown res

  delay(1);

  int slaveAddress = BASE_I2C_ADDRESS;
  if(digitalRead(addr0_Pin) == HIGH)    //A0
    slaveAddress += 1;
  if(digitalRead(addr1_Pin) == HIGH)    //A1
    slaveAddress += 2;

  Wire.begin(slaveAddress);               // This device to join I2C bus as slave with address as defined by address pins
  Wire.onReceive(receiveEvent);           // register function call for when data is received from I2C Master
  Wire.onRequest(requestEvent);           // register function call for when data is requested to I2C Master
}

void loop() {
  
  if(flashLED_state == 2){
    if(analogRead(photoRes_Pin) <= 512)
       analogWrite(flashLED_Pin, flashLED_percent);    
    else
       digitalWrite(flashLED_Pin, LOW);    
  }

  if(sonarEnabled == 1){
    takeSonarReading();
  }
  
  //Delay 50ms before next loop
  delay(50);
}


void takeSonarReading() {
  if(!takingSonarReading){
    takingSonarReading = true;
    sonarReading_timeout_startTime = millis();
    digitalWrite(trig_Pin, LOW); 
    delayMicroseconds(2); 
  
    digitalWrite(trig_Pin, HIGH);
    delayMicroseconds(10); 
  
    digitalWrite(trig_Pin, LOW);
    attachInterrupt(echo_Pin, sonarInterrupt_start, RISING);
  }
  else{      //Check for timeout
    if((millis() - sonarReading_timeout_startTime) > sonarReading_timeout_time){
       detachInterrupt(echo_Pin); 
       takingSonarReading = false;
       sonarReading = 255;
       digitalWrite(sonarLED_Pin, HIGH); 
    }
  }
}

void sonarInterrupt_start(){
  sonarReading_startTime = micros();
  detachInterrupt(echo_Pin);
  attachInterrupt(echo_Pin, sonarInterrupt_end, FALLING);
}

void sonarInterrupt_end(){
  distance = (long)((micros() - sonarReading_startTime)/58.2);
  
  detachInterrupt(echo_Pin);
  takingSonarReading = false;
  
  if (distance >= maximumRange || distance <= minimumRange){
    sonarReading = 255;
    digitalWrite(sonarLED_Pin, HIGH); 
  }
  else {
    sonarReading = distance;
    digitalWrite(sonarLED_Pin, LOW); 
  }
}



//Receiver data event
void receiveEvent(int bytes)
{
  if(bytes > 0){                          //Check is the packet is not empty
    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is

    switch(current_command_code){        //Switch statement to parse code based on the command code
    
    case COMMAND_CODE_REQUEST_SONAR:
      data_write_array[0] = sonarReading;
      data_write_size = 1;
      putCRC_read_write_only(0x12, data_read_array, 0, 1);
      break;
    
    case COMMAND_CODE_SET_SERVO_PULSE_WIDTH:
      count = 2;
      readI2C(&count);
      putCRC_read_only(current_command_code, data_read_array, 2);

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
        putCRC_read_write_only(0x11, data_read_array, 1, 2);   
      }
      break;

    case COMMAND_CODE_SET_FLASHLED:
      count = 1;
      readI2C(&count);
      putCRC_read_only(current_command_code, data_read_array, 1);
      
      if((0 <= data_read_array[0]) & (data_read_array[0] <= 2)){
        flashLED_state = data_read_array[0];
        if(flashLED_state == 1){
          analogWrite(flashLED_Pin, flashLED_percent);
        }
        else{
          digitalWrite(flashLED_Pin, LOW);
        }
      }
      break;
      
     case COMMAND_CODE_SET_FLASHLED_PERCENT:
       count = 1;
       readI2C(&count);
       putCRC_read_only(current_command_code, data_read_array, 1);
        
       flashLED_percent = data_read_array[0]; 
       if(flashLED_state == 1)
         analogWrite(flashLED_Pin, flashLED_percent);
      break;
      
     case COMMAND_CODE_REQUEST_FLASHLED:
      data_write_array[0] = flashLED_state;
      data_write_size = 1;
      putCRC_read_write_only(0x16, data_read_array, 0, 1);
      break;
      
     case COMMAND_CODE_REQUEST_SONAR_STATE:
      data_write_array[0] = sonarEnabled;
      data_write_size = 1;
      putCRC_read_write_only(0x17, data_read_array, 0, 1);
      break;

    case COMMAND_CODE_ENABLE_SONAR:
        sonarEnabled = 1;
        putCRC_read_only(current_command_code, data_read_array, 0);
      break;
    
    case COMMAND_CODE_DISABLE_SONAR:
        putCRC_read_only(current_command_code, data_read_array, 0);
        sonarEnabled = 0;
        sonarReading = 0;
        digitalWrite(sonarLED_Pin, HIGH);
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

    //Clear out remaining bytes, prevents I2C from locking up
    while(Wire.available() > 0){
      Wire.read();
    }  
  }
}

void requestEvent()
{
  Wire.write(data_write_array, data_write_size);    //Write data to the I2C Master. data_write_array contains data bytes to write
  //data_write_size contains the number of bytes to write
  data_write_size = 0;                              //After writing data, set the data_write_size to 0
}

void readI2C(unsigned int* count){
  for(unsigned int i = 0; i < *count; i ++){      //Loop that reads in bytes based on the count value
    data_read_array[i] = Wire.read();  //Read the available byte into the data_read_array
  } 
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

