// Controlling movements of two servos using a Joystick
// Joystick's x- and y- axes are connected to A0 and A1 analog pins of Arduino.
// Servos are connectd to PWM Pins 9 and 10.
// By Explore Labs

#include <Servo.h>

//PWM range is 544 to 2400, stay within that range when defining the max/min pwm
int MIN_PWM = 600;
int MAX_PWM = 2400;

//MAKE SURE YOUR OFFSETS DOESN'T PUT THE PWM OUT OF RANGE
double X_OFFSET = 0;
double Y_OFFSET = 0;

Servo tilt, pan; // create servo object to control a servo
Servo loopX, loopY;
Servo centerX, centerY;

int joyX = A3; // analog pin used to connect the X - axis of Joystick
int joyY = A4; // analog pin used to connect the Y - axis of Joystick
int x, y; // variables to read the values from the analog pins   

int offsetX = A0;
int offsetY = A1;

unsigned int c = 0;

void setup()
{   
  Serial.begin(9600); 
  
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(offsetX, INPUT);
  pinMode(offsetY, INPUT);
  
  tilt.attach(9); // attaches the tilt servo on pin 9 to the servo object 
  pan.attach(10); // attaches the pan servo on pin 10 to the servo object 
  
  loopX.attach(19);
  loopY.attach(18);
  
  loopX.write(600);
  loopY.write(600);
  
  centerX.attach(12);
  centerY.attach(11);
  
  centerX.write(1500);
  centerY.write(1500);
} 

void loop()
{   
  
  X_OFFSET = (double)analogRead(offsetX) / 1023.0;
  Y_OFFSET = (double)analogRead(offsetY) / 1023.0;
 
  //unsigned int minA = MIN_PWM + (900*X_OFFSET);
  //unsigned int minB = MAX_PWM - (900*Y_OFFSET);
  
  //Serial.print(minA);
  //Serial.print("\t");
  //Serial.println(minB);
  
  x = map(analogRead(joyX), 0, 1023, MIN_PWM + (900*X_OFFSET), MAX_PWM - (900*X_OFFSET)); // scale it to use with the servo b/w 900 usec to 2100 usec
  y = map(analogRead(joyY), 0, 1023, MIN_PWM + (900*Y_OFFSET), MAX_PWM - (900*Y_OFFSET));

  
  tilt.write(x); // sets the servo position according to the scaled value 
  pan.write(y);
  
   
  
  if( c == 66 ){
     //Move x to 180
     loopX.write(map(1023, 0, 1023, MIN_PWM + (900*X_OFFSET), MAX_PWM - (900*X_OFFSET)));
     
  }
  else if( c == 132 ){
    //Move y to 180
    loopY.write(map(1023, 0, 1023, MIN_PWM + (900*Y_OFFSET), MAX_PWM - (900*Y_OFFSET)));
  }
  else if( c == 198){
    //Move x to 0
    loopX.write(map(0, 0, 1023, MIN_PWM + (900*X_OFFSET), MAX_PWM - (900*X_OFFSET)));
  }
  else if( c == 264){
    //Move y to 0
    loopY.write(map(0, 0, 1023, MIN_PWM + (900*Y_OFFSET), MAX_PWM - (900*Y_OFFSET)));
  }
  else if( c  > 264){
    //Reset c
    c = 0;
  }
  
  c = c+1; 
     
  delay(15); // waits for the servos to get there
}
