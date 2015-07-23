void receiveEvent(int bytes){
  
  if(bytes >0){
    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is

    switch(current_command_code){

    case COMMAND_CODE_FUNC_STATUS_CLEAR:                        //0x22
      putCRC_read_only(current_command_code, data_read_array, 0);
      cc_status_clear();
      break;
    
    case COMMAND_CODE_FUNC_STATUS_CHECK:                        //0x23
      data_write_size = 1;
      data_write_array[0] = status;
      putCRC_read_write_only(0x23, data_read_array, 0, 1);
      break;
      
    case COMMAND_CODE_SET_SPEED:                                //0x32
      readI2C(2);
      putCRC_read_only(current_command_code, data_read_array, 2);
      cc_setSpeed( (data_read_array[0] << 8) | data_read_array[1] );
      break;
    
    case COMMAND_CODE_STEP:                                     //0x33
      readI2C(2);
      putCRC_read_only(current_command_code, data_read_array, 2);
      cc_step( (data_read_array[0] << 8) | data_read_array[1] );
      //cc_step( (int)25 );
      break;
      
    case COMMAND_CODE_ONE_REV:                                  //0x34
       putCRC_read_only(current_command_code, data_read_array, 0);
       cc_oneRev();
       break;
    
    case COMMAND_CODE_STEPSREV:
        readI2C(2);
        putCRC_read_only(current_command_code, data_read_array, 2);
        cc_stepsrev( (data_read_array[0] <<8)  | data_read_array[1] );
        break;
        
    case COMMAND_CODE_GETSTEPSREV:
        data_write_size = 2;
        data_write_array[0] = (NUM_STEPS>>8);//upper
        data_write_array[1] = NUM_STEPS;//lower
        //putCRC_read_write_only(0x36, data_read_array, 0,1);
        break;
        
    
    case COMMAND_CODE_TEST:              //Case when command code is COMMAND_CODE_TEST      
      data_write_array[0] = 0x69;        //Write 1 for testing i2c connection
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
   
    default:
     break;   
    } 
    
    /*
    while(Wire.available() > 0){
      Wire.read();
    }
    */
  } 
 
}


//Request data event
//This function is called when the I2C Master requests data from the I2C slave device(this device)
//The data to write and size must be set before the I2C Master requests data, else the requested data will be junk
void requestEvent()
{
  Wire.write(data_write_array, data_write_size);    //Write data to the I2C Master. data_write_array contains data bytes to write, data_write_size contains the number of bytes to wr
  data_write_size = 0;                              //After writing data, set the data_write_size to 0
}


