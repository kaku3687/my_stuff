//MOTOR RELATED VARIABLES
#define NUM_BRIDGES        2

byte motor_speed[NUM_BRIDGES] = {0, 0};
boolean motor_direction[NUM_BRIDGES] = {false, false};
unsigned char motorNum, spd, dir;
//*************************

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
  digitalWrite(MOTOR0_A, LOW);
  digitalWrite(MOTOR0_B, LOW);
  digitalWrite(MOTOR1_A, LOW);
  digitalWrite(MOTOR1_B, LOW);
  
  if( motor_direction[0] )
    analogWrite(MOTOR0_A, motor_speed[0]); 
  else
    analogWrite(MOTOR0_B, motor_speed[0]);
  
  if( motor_direction[1] )
    analogWrite(MOTOR1_A, motor_speed[1]);
  else
    analogWrite(MOTOR1_B, motor_speed[1]);  
}
//******************************************************************//
//******************************************************************//
//******************************************************************//
