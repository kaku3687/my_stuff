void cc_setSpeed(int cc_speed)
{
  release();
  thisStepper.setSpeed(cc_speed);
}

void cc_step(int cc_steps)
{
  release();
  thisStepper.step(cc_steps); 
}

void cc_oneRev()
{
  release();
  cc_step(NUM_STEPS); 
}

void release()
{
   digitalWrite(MOTOR1, LOW); 
   digitalWrite(MOTOR2, LOW); 
   digitalWrite(MOTOR3, LOW); 
   digitalWrite(MOTOR4, LOW); 
}
