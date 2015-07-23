void receiveEvent(int bytes)
{ 
  noInterrupts();
  
  if(bytes > 0){                          //Check is the packet is not empty
    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is

    switch(current_command_code){        //Switch statement to parse code based on the command code
      
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
        putCRC_read_write_only(COMMAND_CODE_GET_DIR, data_read_array, 1, 2);
      }
      break;
      
    case COMMAND_CODE_GET_SPD:
      readI2C(1);
      if((data_read_array[0] >= 0) & (data_read_array[0] < NUM_BRIDGES)){
        motorNum = data_read_array[0];
        data_write_array[0] = motorNum;
        data_write_array[1] = motor_speed[motorNum];
        data_write_size = 2;
        putCRC_read_write_only(COMMAND_CODE_GET_SPD, data_read_array, 1, 2);
      }
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
