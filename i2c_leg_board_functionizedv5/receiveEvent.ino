void receiveEvent(int bytes)
{ 
  
  
  noInterrupts();
  //servos[0].detach();
  //servos[1].detach();
  if(bytes > 0){                          //Check is the packet is not empty
    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is
    
    if(current_command_code <= COMMAND_CODE_SET_VREF){
      receiveEvent_object();
    }
    else if(current_command_code >= COMMAND_CODE_LEG_INIT){
      receiveEvent_functions();
    }
    
    /*
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
      //FIRST byte is pin #. 
      //SECOND byte is Resistor either INPUT (0x00) . INPUT_PULLUP(0x02). INPUT_PULLDOWN(0x04). 
      //THIRD byte is Trigger interrupt on RISING (0x03) edge or FALLING edge (0x02)
      readI2C(3); 
      putCRC_read_only(current_command_code, data_read_array, 3);
      // writes out 1 bits to master.
      // choosing Resistor Type
      cc_digital_interrupt(data_read_array[0], data_read_array[1], data_read_array[2]); 
      break;
    
    case COMMAND_CODE_DIGITAL_INTERRUPT_DEBOUNCE: // 0x28
      //FIRST byte is pin #.
      //SECOND byte is upper debounce time in ms
      //THIRD byte is lower debounce time in ms
      readI2C(3);
      putCRC_read_only(current_command_code, data_read_array, 3);
      cc_digital_interrupt_debounce(data_read_array[0], data_read_array[1], data_read_array[2]);
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
     
    case COMMAND_CODE_LEG_INIT:
      readI2C(7);
      putCRC_read_only(current_command_code, data_read_array, 7);
      cc_leg_func_init(data_read_array[0], data_read_array[1], data_read_array[2], data_read_array[3], data_read_array[4], data_read_array[5], data_read_array[6], 3, 2);
      break;
      
    case COMMAND_CODE_LEG_ZERO:
      putCRC_read_only(current_command_code, data_read_array, 0);
      cc_leg_zero();
      break;
      
    case COMMAND_CODE_LEG_MOVE:
      readI2C(9);
      putCRC_read_only(current_command_code, data_read_array, 9);
      cc_leg_move(array2float(data_read_array), array2float(data_read_array + 4), (int)data_read_array[8]);      //might cause an error...
                                                                                                                 //May cause a problem if incorrect data is sent and it attempts to run the code with the incorrect data
      break;
    
    case COMMAND_CODE_LEG_STEP:
      readI2C(9);
      putCRC_read_only(current_command_code, data_read_array, 9);
      cc_leg_step(array2float(data_read_array), array2float(data_read_array + 4), (int)data_read_array[8]);      //might cause an error...
      break;
      
    case COMMAND_CODE_LEG_FUNC_STATUS_CHECK:
        data_write_size = 1;
        data_write_array[0] = cc_walk_status_check();
        putCRC_read_write_only(0x48, data_read_array, 0, 1);
      break;
      
    case COMMAND_CODE_LEG_FUNC_STATUS_CLEAR:
        putCRC_read_only(current_command_code, data_read_array, 0);
        cc_walk_status_clear();
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
    */
    //Clear out remaining bytes, prevents I2C from locking up
    while(Wire.available() > 0){
      Wire.read();
    }  
  }
  interrupts();
}

void receiveEvent_object(){
  
  switch(current_command_code){        //Switch statement to parse code based on the command code
  /*
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
*/    
    /*  
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
    */
    //  0x21 Read either 0 or 1 from
    /*
    case COMMAND_CODE_DIGITAL_READ:   
      data_write_size=2;// writes out 2 bits to master.
      // 1st bit for pin #. 
      //2nd bit  sets either INPUT (0x00) or INPUT_PULLUP(0x02) or INPUT_PULLDOWN(0x04). 
      readI2C(2); 
      cc_digital_read(data_read_array[0], data_read_array[1]);
      putCRC_read_write_only(0x21, data_read_array, 2, 2);
      break;
      */
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
     /*
    case COMMAND_CODE_DIGITAL_INTERRUPT: //   0x25  *************************************************************************************************
      //FIRST byte is pin #. 
      //SECOND byte is Resistor either INPUT (0x00) . INPUT_PULLUP(0x02). INPUT_PULLDOWN(0x04). 
      //THIRD byte is Trigger interrupt on RISING (0x03) edge or FALLING edge (0x02)
      readI2C(3); 
      putCRC_read_only(current_command_code, data_read_array, 3);
      // writes out 1 bits to master.
      // choosing Resistor Type
      cc_digital_interrupt(data_read_array[0], data_read_array[1], data_read_array[2]); 
      break;
    
    case COMMAND_CODE_DIGITAL_INTERRUPT_DEBOUNCE: // 0x28
      //FIRST byte is pin #.
      //SECOND byte is upper debounce time in ms
      //THIRD byte is lower debounce time in ms
      readI2C(3);
      putCRC_read_only(current_command_code, data_read_array, 3);
      cc_digital_interrupt_debounce(data_read_array[0], data_read_array[1], data_read_array[2]);
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
      
    /*
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
    */
    default:                                //Case when the command code doesnt match any 
      break;                                
    }
    
}

void receiveEvent_functions(){
  switch(current_command_code){        //Switch statement to parse code based on the command code
  case COMMAND_CODE_LEG_INIT:
      readI2C(10);
      putCRC_read_only(current_command_code, data_read_array, 10);
      
      char dirx, diry;
      
      if(data_read_array[4] == 2)
        dirx = -1;
      else
        dirx = 1;
        
      if(data_read_array[5] == 2)
        diry = -1;
      else
        diry = 1;
     //void cc_leg_func_init(byte _set_x, byte _set_y,         byte _setHt,       byte  _lift, char _dirx, char _diry, byte _motorSpeed, byte _slowMotorSpeed byte _rot_pin, byte _lim_pin, byte _z_debounce_time, byte _z_timeout_time){      
      cc_leg_func_init(data_read_array[0], data_read_array[1], data_read_array[2], data_read_array[3], dirx, diry, data_read_array[6], data_read_array[7], 3, 2, data_read_array[8], data_read_array[9]);
      break;
      
    case COMMAND_CODE_LEG_ZERO:
      putCRC_read_only(current_command_code, data_read_array, 0);
      cc_leg_zero();
      break;
      
    case COMMAND_CODE_LEG_MOVE:  
      readI2C(9);
      putCRC_read_only(current_command_code, data_read_array, 9);
      cc_leg_move_ifsr(array2float(data_read_array), array2float(data_read_array + 4), (int)data_read_array[8]);      //might cause an error...
                                                                                                                 //May cause a problem if incorrect data is sent and it attempts to run the code with the incorrect data   
      break;
    
    case COMMAND_CODE_LEG_STEP:
      readI2C(9);
      putCRC_read_only(current_command_code, data_read_array, 9);
      cc_leg_step(array2float(data_read_array), array2float(data_read_array + 4), (int)data_read_array[8]);      //might cause an error...
      break;
    
    case COMMAND_CODE_LEG_STOP:
      putCRC_read_only(current_command_code, data_read_array, 0);
      cc_leg_stop();
      break;
      
    case COMMAND_CODE_LEG_FUNC_STATUS_CHECK:
      data_write_size = 1;
      data_write_array[0] = status; //cc_walk_status_check();
      putCRC_read_write_only(0x48, data_read_array, 0, 1);
      break;
      
    case COMMAND_CODE_LEG_FUNC_STATUS_CLEAR:
      putCRC_read_only(current_command_code, data_read_array, 0);
      cc_walk_status_clear();
      break;
      
    case COMMAND_CODE_LEG_SET_THRESHOLD:
      readI2C(2);
      putCRC_read_only(current_command_code, data_read_array, 2);
      cc_set_threshold((data_read_array[0] << 8) | data_read_array[1]);
      break;
    
    case COMMAND_CODE_LEG_SET_THRESHOLD_ENABLE:
      readI2C(1);
      putCRC_read_only(current_command_code, data_read_array, 1);
      cc_set_threshold_enable(data_read_array[0]);
      break;
      
    case COMMAND_CODE_LEG_MOVE_Z:
      readI2C(1);
      putCRC_read_only(current_command_code, data_read_array, 1);
      cc_leg_move_z((int)data_read_array[0]);
      break;
      
    case COMMAND_CODE_LEG_SET_MAINTAINED_HEIGHT:
      readI2C(1);
      putCRC_read_only(current_command_code, data_read_array, 1);
      cc_set_maintained_height(data_read_array[0]);
      break;
      
    default:                                //Case when the command code doesnt match any 
      break;                                
    }
}
