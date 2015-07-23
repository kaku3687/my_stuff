#include <Servo.h>
#include <Wire.h>
#include <msp430g2553.h>
//#include <msp430f5529.h>

#define BASE_I2C_ADDRESS  0x24
//#define BASE_I2C_ADDRESS  0x28
//#define BASE_I2C_ADDRESS 0x2C

const unsigned int MODULE_ID = 1037;        //Module id, defines the type of module
const unsigned int MODULE_VERSION = 1;      //Defines the version of the module

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

#define pin_limit_switch_0    P1_0
#define pin_limit_switch_1    P1_1
#define pin_analog_read_0     A3
#define pin_analog_read_1     A4
#define pin_analog_read_2     A5
#define ANALOG1               P2_1
#define ANALOG2               P2_2
#define ANALOG3               P2_4
#define ANALOG4               P2_5
#define pin_servo_0           P2_6
#define pin_servo_1           P2_7

#define pin_addr_sel_0        P1_2
#define pin_addr_sel_1        P2_0
#define pin_flag              P2_3


//COMMAND CODES
#define COMMAND_CODE_TEST                       0x01
#define COMMAND_CODE_MODULE_ID                  0x02
#define COMMAND_CODE_MODULE_VERSION             0x03
#define COMMAND_CODE_MODULE_RESET               0x04

#define COMMAND_CODE_SET_SERVO_PULSE_WIDTH      0x10     //Set servo to requested pulse width between min/max values as attached, Packet format(4 bytes): CC  |  Servo#(3Bits) + PWM_Upper(5Bits)  |  PWM(Lower)
#define COMMAND_CODE_REQUEST_SERVO_PULSE_WIDTH  0x11     //Request a servo PWM, Packet format(2 bytes): CC  |  Servo#
                                                         //Return data(2 bytes):  PWM_Upper  |  PWM_Lower
                                                         
#define COMMAND_CODE_SET_DIR                    0x12
#define COMMAND_CODE_SET_SPD                    0x13
#define COMMAND_CODE_GET_DIR                    0x14
#define COMMAND_CODE_GET_SPD                    0x15

#define COMMAND_CODE_DIGITAL_WRITE_PIN          0x19
#define COMMAND_CODE_ANALOG_WRITE_PIN           0x20
#define COMMAND_CODE_DIGITAL_READ               0x21
#define COMMAND_CODE_ANALOG_READ                0x22
#define COMMAND_CODE_DIGITAL_INTERRUPT          0x25
#define COMMAND_CODE_DIGITAL_INTERRUPT_CHECK    0x26
#define COMMAND_CODE_DIGITAL_INTERRUPT_CLEAR    0x27

#define COMMAND_CODE_SET_VREF                   0x28

//I2C RELATED VARIABLES
  //Stores command code for request data sending, see command code list above
unsigned int current_command_code = 0; 
  //A buffer array for the write bytes to the I2C Master
unsigned char data_write_array[16] = {0};
  //A buffer array for the read in bytes from I2C Master
unsigned char data_read_array[16] = {0};
  //Size of the data to be written to I2C Master
unsigned int data_write_size = 0;
//*************************

//MOTOR RELATED VARIABLES
#define NUM_BRIDGES        2
byte motor_speed[NUM_BRIDGES] = {0, 0};
boolean motor_direction[NUM_BRIDGES] = {false, false};
unsigned char motorNum, spd, dir;
//*************************

//SERVO RELATED VARIABLES
#define MIN_PULSE_WIDTH      544            //Min pulse width value
#define MAX_PULSE_WIDTH      2400        //Max pulse width value
#define DEFAULT_PULSE_WIDTH  1500    //Default pulse width value when attached
#define SERVO_COUNT 2
  //Stores the servo pin numbers
const unsigned int SERVO_PIN_ARRAY[] = {P2_6, P2_7};
  //Stores the 8 servo objects
Servo servos[SERVO_COUNT]; 
  //temporary storage for servonum and pwm
unsigned int servo_num, pwm;

//*************************

//IO RELATED VARIABLES
  //Keeps track of current voltage reference
unsigned char vref = 33;
  //Keeps track of which pins were interrupted
boolean pins_interrupted[8]={false};
  //Interrupt array
typedef void (* pin_interrupt) (void);
pin_interrupt pin_interrupt_array[] = {NULL, NULL, digital_interrupt_pin2, digital_interrupt_pin3, NULL, 
                                       digital_interrupt_pin5, digital_interrupt_pin6, digital_interrupt_pin7};
//*************************



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
}

void loop()
{
  delay(1000);
}



//******************************************************************//
//******************************MOTORS******************************//
//******************************************************************//

//Function that sets direction of motor
void set_dir(byte motorNum, byte dir){
  if( (motorNum == 0) | (motorNum == 1) ){
    switch(dir){
      case 0:
        motor_direction[motorNum] = false; 
        break;
      case 1:
        motor_direction[motorNum] = true;
        break;
      case 2:
        if(motor_direction[motorNum])
            motor_direction[motorNum] = false; 
        else
            motor_direction[motorNum] = true;
        break;
    }
  }
  update_motors();
}

//Function that sets the speed of the motor
void set_speed(byte motorNum, byte spd){
  switch(motorNum){
    case 0:
      motor_speed[0] = spd;
      break;
    case 1:
      motor_speed[1] = spd;
      break;
    default:
      break;
  }
  update_motors();
}

//Electronically sets the motor settings (ie. runs the analogwrite to run the motors)
void update_motors(){
  digitalWrite(ANALOG1, LOW);
  digitalWrite(ANALOG2, LOW);
  digitalWrite(ANALOG3, LOW);
  digitalWrite(ANALOG4, LOW);
  
  if( motor_direction[0] )
    analogWrite(ANALOG2, motor_speed[0]); 
  else
    analogWrite(ANALOG1, motor_speed[0]);
  
  if( motor_direction[1] )
     analogWrite(ANALOG4, motor_speed[1]);
  else
    analogWrite(ANALOG3, motor_speed[1]);  
}
//******************************************************************//
//******************************************************************//
//******************************************************************//





//******************************************************************//
//******************************SERVOS******************************//
//******************************************************************//
boolean set_servo_pulse_width(unsigned int servo_num, unsigned int pwm){ 
  //Check if servo value falls within range
  if(!((0 <= servo_num) & (servo_num < SERVO_COUNT))){ 
    return false;
  }
  
  //If pwm is 0, then detach the servo from pulsing
  if( pwm == 0 ){
     if(servos[servo_num].attached() == true){              
      servos[servo_num].detach();    //Attach the servo
     }
     return true;
  }
  
  //Check if pwm values fall within range, if not, return false
  if((MIN_PULSE_WIDTH > pwm) | (pwm > MAX_PULSE_WIDTH)){
    return false;
  }
  
  //Check if the servo is attached before changing the pwm, if not then attach servo
  if(servos[servo_num].attached() == false){    
    servos[servo_num].attach(SERVO_PIN_ARRAY[servo_num], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);    //Attach the servo
  }
  
  //Set the servo to the requested pwm
  servos[servo_num].writeMicroseconds(pwm);
  return true; 
}
//******************************************************************//
//******************************************************************//
//******************************************************************//




//******************************************************************//
//********************************IO********************************//
//******************************************************************//
void digital_interrupt_pin2(){digital_interrupt(2);}
void digital_interrupt_pin3(){digital_interrupt(3);}
void digital_interrupt_pin5(){digital_interrupt(5);}
void digital_interrupt_pin6(){digital_interrupt(6);}
void digital_interrupt_pin7(){digital_interrupt(7);}

//Analog write function
void analog_write( unsigned int pinNum, unsigned int value){
  pinMode(pinNum, OUTPUT);
  analogWrite(pinNum, value);
}

//Interrupt routines for each pin pass the pin number to digital_interrupt()
void digital_interrupt(int pinNum){  
  //noInterrupts();
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){   
    detachInterrupt(pinNum);
    pins_interrupted[pinNum] = true; 
    digitalWrite(pin_flag, HIGH);   
  }
  //interrupts();
}

//checks if FLAG_pin is on. go throughs pins_interrupted array[] to check if pin is flagged. 
//if no alerts are on turns off alert pin.
byte check_array(){ 
  for(byte i = 2; i <= 7; i++){
    if(pins_interrupted[i] == true){
      digitalWrite(pin_flag, HIGH);
      return i;
    }        
  }
  return 0xff;
}

//Digital write function
void cc_digital_write(unsigned char & pinNum, unsigned char & state){
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    pinMode(pinNum,OUTPUT);
    if(state== 0x01)
      digitalWrite(pinNum, HIGH);
    else
      digitalWrite(pinNum, LOW); 
  }
}

//Analog write function
void cc_analog_write(unsigned char & pinNum, unsigned char & value){
  //This board has no analog write pins usable 
  return;
}

//Digital read function
void cc_digital_read(unsigned char & pinNum, unsigned char & res){
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
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

//Analog write function
void cc_analog_read(unsigned char pinNum, unsigned char res){
  unsigned char analogPin;
  
  switch(pinNum){
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
      return;  
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

//Attaches a digital interrupt to a pin
void cc_digital_interrupt(unsigned char & pinNum, unsigned char & res, unsigned char & type){
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    
    detachInterrupt(pinNum); //Detaches any interrupt already attached
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
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    if( pins_interrupted[pinNum] == true){
      pins_interrupted[pinNum] = false;
      detachInterrupt(pinNum);  
      pinMode(pinNum, INPUT);  // UPDATED SETS PIN TO INPUT WITH NO PULLUP OR PULLDOWN
    }
  }
  
  if(check_array() == 255){    // No other interrupts pending
    digitalWrite(pin_flag, LOW); 
  }
}
//******************************************************************//
//******************************************************************//
//******************************************************************//



//******************************************************************//
//********************************I2C*******************************//
//******************************************************************//
void receiveEvent(int bytes)
{ 
  noInterrupts();
  if(bytes > 0){                          //Check is the packet is not empty
    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is

    switch(current_command_code){        //Switch statement to parse code based on the command code

    case COMMAND_CODE_SET_SERVO_PULSE_WIDTH:
      readI2C(2);
      putCRC_read_only(current_command_code, data_read_array, 2);
            
      servo_num =  0x07 & (data_read_array[0] >> 5);          //Extract the 3 bits used to identify the servo_num
      pwm = ((data_read_array[0] & 0x1F) << 8) | data_read_array[1];  //Combine the pwm value lower 5bits from first byte and all second byte combined
      
      set_servo_pulse_width(servo_num, pwm);    
     
      break;

    case COMMAND_CODE_REQUEST_SERVO_PULSE_WIDTH:
      readI2C(1);
      
      servo_num = data_read_array[0];
      
      if((0 <= servo_num) & (servo_num < SERVO_COUNT)){    //Check if servo number falls within the range
        pwm = servos[servo_num].readMicroseconds();      
        data_write_array[0] = (pwm >> 8) & 0xFF;
        data_write_array[1] = pwm & 0xFF;
        data_write_size = 2; 
        putCRC_read_write_only(0x11, data_read_array, 1, 2);   
      }
      break;
      
    case COMMAND_CODE_SET_DIR:
      readI2C(2);
      putCRC_read_only(current_command_code, data_read_array, 2);
      motorNum = data_read_array[0];
      dir = data_read_array[1];
      set_dir(motorNum, dir);
      break;
      
    case COMMAND_CODE_SET_SPD:
      readI2C(2);
      putCRC_read_only(current_command_code, data_read_array, 2);
      motorNum = data_read_array[0];
      spd = data_read_array[1];
      set_speed(motorNum, spd);
      break;
      
    case COMMAND_CODE_GET_DIR:
      readI2C(1);
      if((data_read_array[0] >= 0) & (data_read_array[0] < NUM_BRIDGES)){
        motorNum = data_read_array[0];
        data_write_array[0] = motorNum;
        data_write_array[1] = motor_direction[motorNum];
        data_write_size = 2;
        putCRC_read_write_only(0x14, data_read_array, 1, 2);
      }
      break;
      
    case COMMAND_CODE_GET_SPD:
      readI2C(1);
      if((data_read_array[0] >= 0) & (data_read_array[0] < NUM_BRIDGES)){
        motorNum = data_read_array[0];
        data_write_array[0] = motorNum;
        data_write_array[1] = motor_speed[motorNum];
        data_write_size = 2;
        putCRC_read_write_only(0x15, data_read_array, 1, 2);
      }
      break;

    //   0x19 Digitaly write to pins  
    case COMMAND_CODE_DIGITAL_WRITE_PIN: 
      readI2C(2); 
      putCRC_read_only(current_command_code, data_read_array, 2);
      cc_digital_write(data_read_array[0], data_read_array[1]);
      
      break;
    
    //  0x20 Write to pins 
    case COMMAND_CODE_ANALOG_WRITE_PIN:  
      readI2C(2);  
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
      putCRC_read_write_only(0x21, data_read_array, 2, 2);
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
      putCRC_read_write_only(0x22, data_read_array, 2, 4);

      break;
      
    case COMMAND_CODE_DIGITAL_INTERRUPT: //   0x25  *************************************************************************************************
      //FIRST bit is pin #. 
      //SECOND bit is Resistor either INPUT (0x00) . INPUT_PULLUP(0x02). INPUT_PULLDOWN(0x04). 
      //THIRD bit is Trigger interrupt on RISING (0x03) edge or FALLING edge (0x02)
      readI2C(3); 
      putCRC_read_only(current_command_code, data_read_array, 3);
      // writes out 1 bits to master.
      // choosing Resistor Type
      cc_digital_interrupt(data_read_array[0], data_read_array[1], data_read_array[2]); 
      break;

    //   0x26  Checkts status of interupted pins array writes to master what pin has flagged the interrupt.
    case COMMAND_CODE_DIGITAL_INTERRUPT_CHECK:  
      data_write_size = 1; // writes out 1 bits to master.
      data_write_array[0] = check_array();
      putCRC_read_write_only(0x26, data_read_array, 0, 1);
    break;

     //   0x27 dettaches interrupt on selected pin.
    case COMMAND_CODE_DIGITAL_INTERRUPT_CLEAR:   
      //pin # to be detached from interrupt and cleared from pins interrupted array  
      readI2C(1); 
      putCRC_read_only(current_command_code, data_read_array, 1);
      cc_digital_interrupt_clear(data_read_array[0]);   
      break;
      
    case COMMAND_CODE_TEST:              //Case when command code is COMMAND_CODE_TEST      
      data_write_array[0] = 0x69;        //Write 0x69 for testing i2c connection
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
      
    default:                                //Case when the command code doesnt match any 
      break;                                
    }
    
    //Clear out remaining bytes, prevents I2C from locking up
    while(Wire.available() > 0){
      Wire.read();
    }  
  }
  interrupts();
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

//Read i2c bytes
void readI2C(unsigned int count){
  for(unsigned int i = 0; i < count; i ++){      //Loop that reads in bytes based on the count value
    data_read_array[i] = Wire.read();  //Read the available byte into the data_read_array
  } 
}
//******************************************************************//
//******************************************************************//
//******************************************************************//




//******************************************************************//
//********************************CRC*******************************//
//******************************************************************//
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

void putCRC_read_write_only(unsigned char command_code, unsigned char *data, unsigned int rlength, unsigned int wlength){
  putCRC(getCRC_read_array(command_code, data, rlength), wlength);
}
//******************************************************************//
//******************************************************************//
//******************************************************************//


