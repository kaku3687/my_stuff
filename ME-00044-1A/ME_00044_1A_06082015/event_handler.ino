void receiveEvent(int bytes){
  if(bytes >0){

    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is

    switch(current_command_code){

    //   0x19 Digitaly write to pins  
    case COMMAND_CODE_DIGITAL_WRITE_PIN: 
      readI2C(2); 
      //putCRC(data_read_array, 2);
      putCRC_read_only(current_command_code, data_read_array, 2);
      cc_digital_write(data_read_array[0], data_read_array[1]);
      
      break;
    
    //  0x20 Write to pins 4,9,or 19
    case COMMAND_CODE_ANALOG_WRITE_PIN:  
      readI2C(2);  
      //putCRC(data_read_array, 2);
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
      //FIRST bit is pin #. 
      //SECOND bit is Resistor either INPUT (0x00) . INPUT_PULLUP(0x02). INPUT_PULLDOWN(0x04). 
      //THIRD bit is Trigger interrupt on RISING (0x03) edge or FALLING edge (0x02)
      readI2C(3); 
      //putCRC(data_read_array, 3);
      putCRC_read_only(current_command_code, data_read_array, 3);
      // writes out 1 bits to master.
      // choosing Resistor Type
      cc_digital_interrupt(data_read_array[0], data_read_array[1], data_read_array[2]); 
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
      //putCRC(data_read_array, 1);
      putCRC_read_only(current_command_code, data_read_array, 1);
      cc_digital_interrupt_clear(data_read_array[0]);   
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

