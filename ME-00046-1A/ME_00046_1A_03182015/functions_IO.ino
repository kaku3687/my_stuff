callBack cb;

void func_setup()
{
  cb = no_callback;
  status = STEPPER_STATUS_BUSY;
}

void cc_setSpeed(int cc_speed)
{
  release();
  thisStepper.setSpeed(cc_speed);
}

void cc_step(int cc_steps)
{
  func_setup();
  cb = stepping_complete;
  release();
  thisStepper.step(cc_steps); 
  cb();
}

void cc_oneRev()
{
  func_setup();
  cb = stepping_complete;
  release();
  cc_step(NUM_STEPS); 
  cb();
}

void release()
{
   status = STEPPER_STATUS_READY;
   digitalWrite(MOTOR1, LOW); 
   digitalWrite(MOTOR2, LOW); 
   digitalWrite(MOTOR3, LOW); 
   digitalWrite(MOTOR4, LOW); 
}

void stepping_complete()
{
   status = STEPPER_STATUS_COMPLETED;
   digitalWrite(FLAG_PIN, HIGH); 
}

void cc_status_clear()
{
   status = STEPPER_STATUS_READY;
   update_flag(); 
}

void no_callback()
{
}
