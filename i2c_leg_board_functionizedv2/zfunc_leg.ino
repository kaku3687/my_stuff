#define LEG_FUNC_STATUS_READY      1
#define LEG_FUNC_STATUS_COMPLETED  2
#define LEG_FUNC_STATUS_BUSY       3
#define LEG_FUNC_STATUS_TIMED_OUT  4
#define LEG_FUNC_STATUS_ZEROING    5
//#define M_PI                      3.141593
#define DEBOUNCE_TIME              3
#define TIMEOUT_TIME               400
#define ZERO_TIMEOUT_TIME          20000
#define OUTWARD                    0              //Production models = 0, test leg = 1
#define INWARD                     1
#define SLOW_MOTORSPEED            75


zcallBack z_cb;
zcallBack lim_cb;

//This contains the current status of the leg object in terms of movement
//status = LEG_FUNC_STATUS_READY;       //1 = Ready/Waiting
                                        //2 = Completed Successfully, this triggers an interrupt
                                        //3 = Busy
                                        //4 = Timed Out, this triggers an interrupt
                                        //5 = Zeroing
                                       
                                        
//The servo values that correspond to (0, 0)
//on the x,y plane the robot is walking on.
//Values are in degrees.                                        
byte set_x, set_y; //Offset for servo driving leg in x and y-axis.

byte setHt;    //Set height to maintain, anywhere between 1-20
byte lift;     //Value above the setHt to lift leg to, any value < setHt

//Direction/orientation of the servos.
char dirx, diry;        //Will store -1/1

//Speed to run motors
byte motorSpeed;
byte slowMotorSpeed = SLOW_MOTORSPEED;

//Pin numbers for rotation and limit sensors
byte rot_pin;
byte lim_pin;
byte fsr_pin = pin_analog_read_1;

//variable to hold what state of debounce the switch is
volatile boolean switch_held = false;

//variable to hold if the leg motor is stalled
volatile boolean stalled = false;

//variable to hold if z move is occuring
volatile boolean z_moving = false;

unsigned int z_debounce_time = DEBOUNCE_TIME;  //?? milliseconds seems to be a good debounce time, depends on the physical setup and how quality control it is
unsigned long z_previous_time = 0;
unsigned int z_timeout_time = TIMEOUT_TIME ; //Timeout time between rotation clicks. This allow to test for timeouts
int rot_count = 0;  //Keeps track of the current rotation count, temporary set to 0 to test.
int rot_goal;  //Keeps track of the goal rot_count, this is set in the z_move function

//leg_move/leg_step variables
float thetaX, thetaY; 
float length;
float arg_x, arg_y;
float mag, height;
int clicks;

//fsr treshold
boolean fsr_reached = false;
boolean threshold_enabled = true;
unsigned int fsr_threshold = 100;

void cc_leg_func_init(byte _set_x, byte _set_y, byte _setHt, byte _lift, char _dirx, char _diry, byte _motorSpeed, byte _slowMotorSpeed, byte _rot_pin, byte _lim_pin, byte _z_debounce_time, byte _z_timeout_time){      //Set parameters for the leg functions
  set_x = _set_x;
  set_y = _set_y;
  setHt = _setHt;
  lift = _lift;
  dirx = _dirx;
  diry = _diry;
  motorSpeed = _motorSpeed;
  slowMotorSpeed = _slowMotorSpeed;
  rot_pin = _rot_pin;
  lim_pin = _lim_pin;
  z_debounce_time = _z_debounce_time;
  z_timeout_time = _z_timeout_time * 10;
  status = LEG_FUNC_STATUS_READY;
  
  TwoMsTimer::set(50, fsr_check);
  pinMode(3, INPUT);                                            //Change this... depending if using a mechanical/optical switch. Optical = no resistor, mechanical = pullup resistor
  z_previous_time = millis() - z_debounce_time - 1;               //Set z_previous_time to be less than the current milli and debounce.
  attachInterrupt(rot_pin, z_falling, FALLING);                 //Attach interrupt
}

void leg_func_setup(){  //This function is used to setup leg func things, such as clearing interrupts and setting the status, this should be called before each leg function
  z_cb = no_callback;
  //detachInterrupt(rot_pin);
  //detachInterrupt(lim_pin);
  set_speed(0, 0);
  z_previous_time = millis() - z_debounce_time - 1; 
  status = LEG_FUNC_STATUS_BUSY;

}

void cc_leg_zero(){
  leg_func_setup();
  status = LEG_FUNC_STATUS_ZEROING;
  servos[0].write(set_x);  //Set servos to 0
  servos[1].write(set_y);  //
  
  set_dir(0, INWARD);                                        //Drive the motor inward so it hits the limit switch
  set_speed(0, 150);                                         //Start motor
  pinMode(lim_pin, INPUT_PULLUP);                            //Set limit pin to input pullup.
  attachInterrupt(lim_pin, cc_leg_zero_cb0, FALLING);        //Set the interrupt for when the limit switch is hit.
}

void cc_leg_zero_cb0(){      //Interrupt is called when limit switch is hit
  detachInterrupt(lim_pin);  //Detach the limit switch interrupt
  
  set_speed(0, 10);          //Set speed to low 
  set_dir(0, OUTWARD);       //Set direction outward, reversing direction to prevent drift
  set_speed(0, 0);           //Turn off motor
  
  rot_count = -1;            //Set rot_count to -1, so the first click counts as the zero point.
  walk_func_complete();      //Trigger interrupt ready
}

void calc_leg_vars(float x, float y, int z){
  //calculate the leg length given the set height to
  //maintain.
  length = (float)2.0 + ((float)setHt*(float)(0.05));
	
  //calculate the angle to move the servo driving the
  //leg along the x-axis. 
  arg_x = x/length;
  thetaX = (dirx)*atan(arg_x)*((float)180/M_PI) + (float)set_x;
  
  //calculate the angle to move the servo driving the
  //leg along the y-axis. 
  arg_y = (float)y/(float)length;
  thetaY = (float)(diry)*atan(arg_y)*(180/M_PI) + (float)set_y;
  
  	
  //Calculate z height to maintain robot height.
  
  if(z == 255){	//Automatically calculate z height if z value is 255
    //Overall vector magnitude.	
    mag = (float)sqrt(pow(x, 2.0) + pow(y, 2.0));
    //Height given vector magnitude.
    height = (float)sqrt(pow(length, 2.0) + pow(mag, 2.0));
    //Converting height to discrete count value.
    clicks = (int)((height - 2.0)*20.0 + 0.5f);
  }
  else{         //Else use the z value
   clicks = z; 
  }
}

void cc_leg_move(float x, float y, int z){
  leg_func_setup();

  calc_leg_vars(x, y, z);
  
  z_cb = cc_leg_move_cb0;
  z_move(clicks);
}

void cc_leg_move_cb0(){
  if(!servos[0].attached()){
     servos[0].attach(SERVO_PIN_ARRAY[0], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  }

  if(!servos[1].attached()){
     servos[1].attach(SERVO_PIN_ARRAY[1], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  }
  
  servos[0].write((int)thetaX);
  servos[1].write((int)thetaY);
  
  walk_func_complete();
}

void cc_leg_step(float x, float y, int z){ 
  leg_func_setup();
  
  calc_leg_vars(x, y, z);

 //Move leg up a count value above set height
  z_cb = cc_leg_step_cb0;
  z_move(setHt - lift); //HOLD
}

void cc_leg_step_cb0(){
  if(!servos[0].attached()){
     servos[0].attach(SERVO_PIN_ARRAY[0], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  }

  if(!servos[1].attached()){
     servos[1].attach(SERVO_PIN_ARRAY[1], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  }
    
  servos[0].write((int)thetaX);
  servos[1].write((int)thetaY);
  z_cb = walk_func_complete;
  z_move(clicks);
}

boolean z_move(int _z){  
        rot_goal = _z;    //Make z global
        byte mtr_dir_;    
  
	//Determine direction to move motor given
	//z height goal and current height.
	if(_z < rot_count){
		mtr_dir_ = INWARD;
	}else if(_z > rot_count){
		mtr_dir_ = OUTWARD;
                if(threshold_enabled){
                  fsr_reached = false;
                  TwoMsTimer::start();
                }
	}else{
                z_cb();
                return true;
	}	

	//Start motor if leg is not already at goal
	//position.
        set_dir(0, mtr_dir_);
        
	//if(_z != rot_count){
        //  set_speed(0, motorSpeed);
	//}
        z_previous_time = millis() - z_debounce_time - 1; 
        
        if(((rot_count - rot_goal) == 1 )| ((rot_count - rot_goal) == -1)){    //Slows down motor before hitting last limit
          set_speed(0, slowMotorSpeed);
        }
        else{
          set_speed(0, motorSpeed);                                            //Otherwise run fullspeed
        }
}

void z_falling(){  
    if(((millis() - z_previous_time) > z_debounce_time) & (digitalRead(rot_pin) == LOW)){
      z_previous_time = millis();
      detachInterrupt(rot_pin);
      attachInterrupt(rot_pin, z_rising, RISING); 
      switch_held = true;
    }
}

void z_rising(){  
    if( ((millis() - z_previous_time) > z_debounce_time) &  (digitalRead(rot_pin) == HIGH)){
        detachInterrupt(rot_pin);  //Detach the interrupt

        //Check how to modify rotation count
        boolean motorDir;
        if( OUTWARD == 0 )
          motorDir = false;
        else
          motorDir = true;
        
        if(!stalled & !fsr_reached){                                                              //If motor is not stalled, then this is a complete count
          if(motor_direction[0] == motorDir)                                       //Add to rot count if motor_direction is outward
            rot_count ++; 
          else                                                                      //Subtract to rot count if motor direction is inward
            rot_count --; 
        }
        else if(fsr_reached){
          rot_goal = rot_count;
          fsr_reached = false;
            //set_speed(0, 0);                                                      //Goal is reached, stop motor, don't know if it will stop the motor before it passes the switch????
           // z_cb(); 
        }
        else if (stalled){                                                                      //Motor was stalled, therefore the motor was driven backwards, negate this count
          walk_timeout_complete();                                                //Stop motor                                                                                //Stall switch is cleared
        }
        
        if(status == LEG_FUNC_STATUS_BUSY){
          if(((rot_count - rot_goal) == 1 )| ((rot_count - rot_goal) == -1)){    //Slows down motor before hitting last limit
            set_speed(0, slowMotorSpeed);
          }
          
          //Check if z has reached its goal
          if(rot_count == rot_goal){                                          //Call the callback if goal was reached
            set_speed(0, 0);                                                  //Goal is reached, stop motor, don't know if it will stop the motor before it passes the switch????
            z_cb();                                                         //Call the callback
          }
        }
        
        z_previous_time = millis();                                       //Record time for checking if the z_rising is properly debounce (clears switch)
        attachInterrupt(rot_pin, z_falling, FALLING);                     //Set falling interrupt again if rotation count is not reached. To keep counting rotations
        switch_held = false;
    }       
}
  
void cc_leg_stop(){
  z_cb = no_callback;
  //detachInterrupt(rot_pin);
  //detachInterrupt(lim_pin);
  set_speed(0, 0);
  status = LEG_FUNC_STATUS_READY;
}
        
boolean walk_timeout(){
  if( ((millis() - z_previous_time) > z_timeout_time) & (status == LEG_FUNC_STATUS_BUSY )){
    z_cb = no_callback;
    
    if(switch_held){
      set_speed(0, motorSpeed);
      set_dir(0, 2);              //Flip direction to clear switch
      stalled = true;
    }
    else{
      set_speed(0, 0);
      status = LEG_FUNC_STATUS_TIMED_OUT;
      digitalWrite(pin_flag, HIGH);
    } 
    return true;
  }
  return false;
}

void walk_timeout_complete(){
  set_speed(0, 0);
  stalled = false;
  status = LEG_FUNC_STATUS_TIMED_OUT;
  digitalWrite(pin_flag, HIGH);
}

void cc_walk_status_clear(){
  status = LEG_FUNC_STATUS_READY; 
  update_flag();
}


void walk_func_complete(){
  status = LEG_FUNC_STATUS_COMPLETED;
  digitalWrite(pin_flag, HIGH);
  TwoMsTimer::stop();
}

void fsr_check(){
  unsigned int fsr_reading = analogRead(fsr_pin);
  
  if(fsr_reading > fsr_threshold){
    //z_previous_time = millis();
    //switch_held = true;
    if(switch_held){          //Case when switch is midstrike
      TwoMsTimer::stop();
      set_speed(0, motorSpeed);
      set_dir(0, 2);              //Flip direction to clear switch
      fsr_reached = true;
      
    }
    else{
      TwoMsTimer::stop();
      set_speed(0, 0);                                                  //Goal is reached, stop motor, don't know if it will stop the motor before it passes the switch????
      z_cb(); 
    }
   //do something like stop motor. 
  }
}

void no_callback(){
}


