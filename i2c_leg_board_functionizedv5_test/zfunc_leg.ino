#define LEG_FUNC_STATUS_READY      1
#define LEG_FUNC_STATUS_COMPLETED  2
#define LEG_FUNC_STATUS_BUSY       3
#define LEG_FUNC_STATUS_TIMED_OUT  4
#define LEG_FUNC_STATUS_ZEROING    5
//#define M_PI                      3.141593
#define DEBOUNCE_TIME              3
#define TIMEOUT_TIME               400
#define ZERO_TIMEOUT_TIME          20000
#define OUTWARD                    1              //Production models = 0, test leg = 1
#define INWARD                     0
#define SLOW_MOTORSPEED            75
#define COUNT_LIMIT                20            //Sets the limit in which the rotation count can reach
#define FSR_POLL_SPEED             40
#define FSR_NUM                    1              //One or two fsrs. Uses pin_analog_read_1, pin_analog_read_2
#define FSR_POLL_LIMIT             2              //number of times this fsr could be read


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
byte lift_percentage;

//Direction/orientation of the servos.
char dirx, diry;        //Will store -1/1

//Speed to run motors
byte motorSpeed;
byte slowMotorSpeed = SLOW_MOTORSPEED;

//Pin numbers for rotation and limit sensors
byte rot_pin;
byte lim_pin;
const byte fsr_pin_1 = pin_analog_read_1;
const byte fsr_pin_2 = pin_analog_read_0;

//variable to hold if the leg motor is stalled
volatile boolean stalled = false;

//variable to hold if z move is occuring
volatile boolean z_moving = false;

volatile boolean switch_dir;

unsigned int z_debounce_time = DEBOUNCE_TIME;  //?? milliseconds seems to be a good debounce time, depends on the physical setup and how quality control it is
unsigned long z_previous_time = 0;
unsigned int z_timeout_time = TIMEOUT_TIME ; //Timeout time between rotation clicks. This allow to test for timeouts

int rot_goal;  //Keeps track of the goal rot_count, this is set in the z_move function
int rot_limit = COUNT_LIMIT; //Sets a limit to the valuet hat rot count can reach, useful if FSR is enabled
boolean rot_limit_enabled = true; //enabled or disable rot limit count

//leg_move/leg_step variables
float thetaX, thetaY; 
//float length;
//float arg_x, arg_y;
//float mag, height;
int clicks;

//fsr treshold
boolean threshold_enabled = false;
unsigned int fsr_threshold = 0;

//count limit, limits the count z can reach.

//stuff related to new loop
const unsigned char ifsr_degree_increments = 2;
const unsigned char ifsr_threshold_percentage = 5;
int ifsr_threshold_min, ifsr_threshold_max;
int init_x, init_y;
int diff_x, diff_y;
unsigned char ifsr_calc_steps;
unsigned char ifsr_current_step;
boolean incremental_move = false;
int fsr_poll_count = 0;
//double ifsr_step_multiplier;
//int new_x_servo, new_y_servo;


boolean stabilization = false;


void cc_leg_func_init(byte _set_x, byte _set_y, byte _setHt, byte _lift_percentage, char _dirx, char _diry, byte _motorSpeed, byte _slowMotorSpeed, byte _rot_pin, byte _lim_pin, byte _z_debounce_time, byte _z_timeout_time){      //Set parameters for the leg functions
  set_x = _set_x;
  set_y = _set_y;
  setHt = _setHt;
  lift_percentage = _lift_percentage;
  lift = (byte)((float)((float)lift_percentage / 100) * (float)setHt);
  dirx = _dirx;
  diry = _diry;
  motorSpeed = _motorSpeed;
  slowMotorSpeed = _slowMotorSpeed;
  rot_pin = _rot_pin;
  lim_pin = _lim_pin;
  z_debounce_time = _z_debounce_time;
  z_timeout_time = _z_timeout_time * 10;
  status = LEG_FUNC_STATUS_READY;
  
  TwoMsTimer::set(FSR_POLL_SPEED, fsr_check);                                //Setup timer for force sensitive resistor which will periodically(in milliseconds) check the force sensor to see if the threshold was triggered.
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
  incremental_move = false;
  fsr_poll_count = 0;
  
  stabilization = false;
  TwoMsTimer::stop();
  
}

void cc_set_maintained_height(byte _setHt){
  setHt = _setHt; 
  lift = (byte)((float)((float)lift_percentage / 100) * (float)setHt);
}

void cc_set_threshold(unsigned int threshold){
  if( threshold <= 1023 )
    fsr_threshold = threshold;
  else
    threshold = 1023;
}

void cc_set_threshold_enable(byte set_enable){
  if(set_enable == 0)
    threshold_enabled  = false;
  else
    threshold_enabled = true;
}

void cc_leg_zero(){
  leg_func_setup();
  status = LEG_FUNC_STATUS_ZEROING;
  servos[0].write(set_x);  //Set servos to 0
  servos[1].write(set_y);  //
  

  
  set_dir(0, INWARD);                                        //Drive the motor inward so it hits the limit switch
  set_speed(0, motorSpeed);                                  //Start motor
  pinMode(lim_pin, INPUT_PULLUP);                            //Set limit pin to input pullup.
  attachInterrupt(lim_pin, cc_leg_zero_cb0, FALLING);        //Set the interrupt for when the limit switch is hit.
}

void cc_leg_zero_cb0(){      //Interrupt is called when limit switch is hit
  detachInterrupt(lim_pin);  //Detach the limit switch interrupt
  
  //set_speed(0, 10);          //Set speed to low 
  //set_dir(0, OUTWARD);       //Set direction outward, reversing direction to prevent drift
  set_speed(0, 0);           //Turn off motor
  
  rot_count = -1;            //Set rot_count to -1, so the first click counts as the zero point.
  walk_func_complete();      //Trigger interrupt ready
}

void calc_leg_vars(float x, float y, int z){
  float length, mag, height; 
  float arg_x, arg_y;
  
  //calculate the leg length given the set height to
  //maintain.

  length = (float)2.0 + ((float)setHt*(float)(0.05));
	
  //calculate the angle to move the servo driving the
  //leg along the x-axis. 
  arg_x = x/length;
  //thetaX = (dirx)*atan(arg_x)*((float)180/M_PI) + (float)set_x;
  thetaX = ((float)dirx)* FastArcTan(arg_x)*(180.0/M_PI) + (float)set_x;
  
  //calculate the angle to move the servo driving the
  //leg along the y-axis. 
  arg_y = (float)y/(float)length;
  //thetaY = (float)(diry)*atan(arg_y)*(180/M_PI) + (float)set_y;
  thetaY = ((float)diry)* FastArcTan(arg_y)*(180.0/M_PI) + (float)set_y;
  	
  //Calculate z height to maintain robot height.
  
  if(z == 255){	//Automatically calculate z height if z value is 255
    //Overall vector magnitude.	
    //mag = (float)sqrt(pow(x, 2.0) + pow(y, 2.0));
    mag = (float)SquareRootFloat(pow(x, 2.0) + pow(y, 2.0));
    
    //Height given vector magnitude.
    //height = (float)sqrt(pow(length, 2.0) + pow(mag, 2.0));
    height = (float)SquareRootFloat(pow(length, 2.0) + pow(mag, 2.0));
    
    //Converting height to discrete count value.
    clicks = (int)((height - 2.0)*20.0 + 0.5f);
  }
  else{         //Else use the z value
   clicks = z; 
  }
}

void cc_leg_move_ifsr(float x, float y, int z){

  
  leg_func_setup();
  incremental_move = true;
  
  //Calculate x, y, z variables
  calc_leg_vars(x, y, z);
  
  //Calculate the min/max fsr values that will adjust the leg
  ifsr_threshold_min = fsr_threshold - (((float)ifsr_threshold_percentage)/100.0) * 1023;
  ifsr_threshold_max = fsr_threshold + (((float)ifsr_threshold_percentage)/100.0) * 1023;

  //Attach servos if they aren't already, set them initialize to 90 degrees, the assume position of the servos.  
  if(!servos[0].attached()){
     servos[0].attach(SERVO_PIN_ARRAY[0], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
     servos[0].write((int)90);
     
  }

  if(!servos[1].attached()){
     servos[1].attach(SERVO_PIN_ARRAY[1], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
     servos[1].write((int)90);
     
  }
  
  //Record the initial servo values
  init_x = servos[0].read();
  init_y = servos[1].read();

  
  //Calculate the difference between the current angle of the servos and the goal angle of the servos
  //take the absolute value of each
  diff_x = (int)thetaX - init_x;
  diff_y = (int)thetaY - init_y;
  
  //Determine which has a bigger difference and use that as the number of increments for this move function
  if( abs(diff_x) > abs(diff_y) ){
    ifsr_calc_steps = (unsigned char)abs(diff_x);
  }
  else{
    ifsr_calc_steps = (unsigned char)abs(diff_y);
  }
  
  ifsr_current_step = 0;
  fsr_poll_count = 0;
  cc_leg_move_ifsr_cb0();
}

void cc_leg_move_ifsr_cb0(){
  //Stop the fsr_check
  TwoMsTimer::stop();
  
  
  //Check if goal is reached, if it is move function is complete
  if(ifsr_current_step > ifsr_calc_steps){    
    walk_func_complete();
    incremental_move = false;
    stabilization = true;
    TwoMsTimer::start();
  }
  
  else{
  //Calculate the percentage in which to move the servo's difference
  double ifsr_step_multiplier;
  
  //If ifsr_calc_steps is equal to 0, then we get 0/0 which is undefined which causes the multiplier to be a really high number(?)
  //Which then sends a servo off to the extreme side.
  //If ifsr_calc_steps is equal 0, then set the multiplier to zero so it can still trigger the fsr z height adjusting.
  if( ifsr_calc_steps != 0 ){
    ifsr_step_multiplier = (double)ifsr_current_step/(double)ifsr_calc_steps;
  }
  else{
     ifsr_step_multiplier = 0; 
  }
  
  
  int new_x_servo, new_y_servo;
  //Calcuate the individual servo values
  new_x_servo = ifsr_step_multiplier * diff_x + init_x;
  new_y_servo = ifsr_step_multiplier * diff_y + init_y;
  
  //Set the new values to the servos
  servos[0].write(new_x_servo);
  servos[1].write(new_y_servo);
  

  
  //Set the callback
  z_cb = cc_leg_move_ifsr_cb0;
  
  if( (ifsr_current_step < ifsr_calc_steps) & ((ifsr_current_step + ifsr_degree_increments) > ifsr_calc_steps) ){
    ifsr_current_step = ifsr_calc_steps;
  }
  else{
    ifsr_current_step += ifsr_degree_increments;
  } 

  //Reset the poll count
  fsr_poll_count = 0;

  //Start the timer

  TwoMsTimer::start();  
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
  z_move((int)(setHt - lift)); //HOLD
  //z_move(0);
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

void cc_leg_move_z(int _z){
  leg_func_setup();
  z_cb = walk_func_complete;
  z_move(_z);
}

boolean z_move(int _z){  
        rot_goal = _z;    //Make z global
        byte mtr_dir_;    
  
	//Determine direction to move motor given
	//z height goal and current height.
	if(_z < rot_count){
		mtr_dir_ = INWARD;
	}
        else if(_z > rot_count){
		mtr_dir_ = OUTWARD;
                if(threshold_enabled){
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
      
      boolean motorDir;
      if( OUTWARD == 0 )
        motorDir = false;
      else
        motorDir = true;
      
      //Record the direction the switch was triggered from
      if(motor_direction[0] == motorDir)                                       //Add to rot count if motor_direction is outward
        switch_dir = true; 
      else                                                                      //Subtract to rot count if motor direction is inward
        switch_dir = false; 
    }
}

void z_rising(){  
  noInterrupts();  
    if( ((millis() - z_previous_time) > z_debounce_time) &  (digitalRead(rot_pin) == HIGH)){
        detachInterrupt(rot_pin);  //Detach the interrupt

        //Check how to modify rotation count
        boolean motorDir;
        if( OUTWARD == 0 )
          motorDir = false;
        else
          motorDir = true;
        
        //Check which direction the switch is being triggered. Used to compare with the z_falling switch direction
        boolean switch_dir_compare;
        if(motor_direction[0] == motorDir)                                       //Add to rot count if motor_direction is outward
          switch_dir_compare = true; 
        else                                                                      //Subtract to rot count if motor direction is inward
          switch_dir_compare= false; 
        
        //Modify the rotation count value if the switch pass-through was the same during the falling and rising stages
        if( switch_dir_compare == switch_dir){
          if(motor_direction[0] == motorDir)                                       //Add to rot count if motor_direction is outward
            rot_count ++; 
          else                                                                      //Subtract to rot count if motor direction is inward
            rot_count --; 
        }
        
        if(status == LEG_FUNC_STATUS_BUSY){
          if(((rot_count - rot_goal) == 1 )| ((rot_count - rot_goal) == -1)){    //Slows down motor before hitting last limit
            set_speed(0, slowMotorSpeed);
          }
          
          //Check if z has reached its goal
          if((rot_count == rot_goal) & !incremental_move & !stabilization){                                          //Call the callback if goal was reached
            if(!threshold_enabled | !(motor_direction[0] == motorDir)){       //Check if threshold is enabled or if the direction is going in
              set_speed(0, 0);                                                  //Goal is reached, stop motor, don't know if it will stop the motor before it passes the switch????
              z_cb();                                                         //Call the callback
            }
          }
          else if( ((rot_count >= rot_limit) & rot_limit_enabled) ){
              if(motor_direction[0] == motorDir){  //Only stop motor if the direction is outward, FIXED BUG on 10/29/2014
                set_speed(0, 0);                                                  //stop motor because of limit count reached
                z_cb();
              }
          }
        }
        
        z_previous_time = millis();                                       //Record time for checking if the z_rising is properly debounce (clears switch)
        attachInterrupt(rot_pin, z_falling, FALLING);                     //Set falling interrupt again if rotation count is not reached. To keep counting rotations
    }  
  interrupts();    
}
  
void cc_leg_stop(){
  leg_func_setup();
  z_cb = no_callback;
  servos[0].detach();
  servos[1].detach();
  set_speed(0, 0);
  status = LEG_FUNC_STATUS_READY;
}
        
boolean walk_timeout(){
  if( ((millis() - z_previous_time) > z_timeout_time) & (status == LEG_FUNC_STATUS_BUSY )){
    z_cb = no_callback;
    set_speed(0, 0);
    status = LEG_FUNC_STATUS_TIMED_OUT;
    digitalWrite(pin_flag, HIGH); 
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
  incremental_move = false;
  status = LEG_FUNC_STATUS_COMPLETED;
  digitalWrite(pin_flag, HIGH);
  TwoMsTimer::stop();
  
  fsr_poll_count = 0;
}

void fsr_check(){

  unsigned int fsr_reading = 0;
  
  //Check how many fsrs are being used as defined in #DEFINE FSR_NUM
  if(FSR_NUM >= 1)
    fsr_reading += analogRead(fsr_pin_1);
  if(FSR_NUM >= 2)
    fsr_reading += analogRead(fsr_pin_2);
  
  //Average the fsrs, may use some weighted function later.
  fsr_reading /= FSR_NUM;  
  
  //Check if in incremental move mode which is on during cc_leg_move_ifsr
  if(incremental_move | stabilization){
    

    //This is a boolean that is true if the rot count is greater than the rot limit and if the rot limit is enabled
    //Used to check if the motor is going to screw off the leg
    boolean rot_count_over_and_limit_enabled = (rot_count >= (rot_limit - 3)) & rot_limit_enabled;
    //rot_count_over_and_limit_enabled = (rot_count >= (setHt + 2)) & rot_limit_enabled;
    
    //This is a boolean that checks if the rot count is under or equal to 0
    //Used to check if the motor is gonna go past and hit itself
    boolean rot_count_equal_or_under_0 = (rot_count <= (setHt - 2));
    
    //fsr timeout
    if( (fsr_poll_count >= FSR_POLL_LIMIT ) & !stabilization){
      //stop_timer_and_motor_and_call_callback();
        TwoMsTimer::stop();
        set_speed(0, 0);                                                  //Goal is reached, stop motor, don't know if it will stop the motor before it passes the switch????
        //analogWrite(ANALOG2, 0); 
        //analogWrite(ANALOG1, 0); 
        z_cb(); 
    }
    
    
    //Check if fsr reading is greater than threshold max
    //Check to only go inward if the rot count is not equal or below 0, Else stop the motor since it cannot go inward anymore
    //Move motor inwards if it is
    else if( (fsr_reading > ifsr_threshold_max) & !rot_count_equal_or_under_0){
      set_speed(0, motorSpeed);
      set_dir(0, INWARD); //Go inwards
    }
    
    //Check if the fsr reading is less than the threshold min
    //Check to only go outward if the rot count is not over the limit and limit is enabled, Else stop the motor since it cannot go outward anymore
    //Move motor outwards if it is
    else if( (fsr_reading < ifsr_threshold_min) & !rot_count_over_and_limit_enabled){
      set_speed(0, motorSpeed);
      set_dir(0, OUTWARD); //Go outwards
    }
    
    //Else stop the motor since it is at target fsr
    //Else stop the motor since it cannot go inward anymore
    //Else stop the motor since it cannot go outward anymore
    else{
       //stop_timer_and_motor_and_call_callback();
       set_speed(0, 0);
        //analogWrite(ANALOG2, 0); 
        //analogWrite(ANALOG1, 0); 
    }  
    
    fsr_poll_count ++;
  }
  
  //FSR check used during z_moves and leg_steps
  else{
    if((fsr_reading > fsr_threshold) & (rot_count >= rot_goal)){
      //stop_timer_and_motor_and_call_callback();
      TwoMsTimer::stop();
      set_speed(0, 0);                                                  //Goal is reached, stop motor, don't know if it will stop the motor before it passes the switch????
      z_cb(); 
    }
  }

}


//Function that stops the two ms timer and sets the motor to off and calls the callback
//Used in the fsr_check function
/*
void stop_timer_and_motor_and_call_callback(){
  TwoMsTimer::stop();
  set_speed(0, 0);                                                  //Goal is reached, stop motor, don't know if it will stop the motor before it passes the switch????
  z_cb(); 
}
*/

void no_callback(){
}


