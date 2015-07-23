//SERVO RELATED VARIABLES
#define MIN_PULSE_WIDTH      544            //Min pulse width value
#define MAX_PULSE_WIDTH      2400        //Max pulse width value
#define DEFAULT_PULSE_WIDTH  1500    //Default pulse width value when attached
#define SERVO_COUNT 2
  //Stores the servo pin numbers
const unsigned char SERVO_PIN_ARRAY[] = {P2_6, P2_7};
  //Stores the 8 servo objects
Servo servos[SERVO_COUNT]; 
  //temporary storage for servonum and pwm
unsigned int servo_num, pwm;

//*************************

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
