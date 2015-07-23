void receiveEvent(int bytes)
{ 
  if(bytes > 0){                          //Check is the packet is not empty
    current_command_code = Wire.read();  //Read in the command code to see what type of packet it is
    
    switch(current_command_code){   
      case COMMAND_CODE_WRITE_REGISTER:
        break;
      case COMMAND_CODE_WRITE_MULTIPLE_REGISTERS:
        break;
      case COMMAND_CODE_READ_REGISTER:
        break;
      case COMMAND_CODE_READ_MULTIPLE_REGISTERS:
        break;
      case COMMAND_CODE_EXECUTE_FUNCTION:
        break;
      default:                                //Case when the command code doesnt match any 
        break;                                
    }
  }
}
