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
  //update_motors();
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
  //update_motors();
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
