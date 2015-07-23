//This function is used when clearing an interrupt. 
//It checks if there is any other pending interrupts, if there is, it sets the flag high.
//If there isn't, then set the flag to low.

void update_flag(){
  /*
  if(check_array() != 255){          //This checks if there is an interrupt pending on a specific pin.
    digitalWrite(pin_flag, HIGH);    //Set the flag line high still
  }
  */
  //else if((cc_walk_status_check() == LEG_FUNC_STATUS_COMPLETED) | (cc_walk_status_check() == LEG_FUNC_STATUS_TIMED_OUT)){      //This checks if there is an interrupt pending on the leg functions.
  if((status == LEG_FUNC_STATUS_COMPLETED) | (status == LEG_FUNC_STATUS_TIMED_OUT)){      //This checks if there is an interrupt pending on the leg functions.
    digitalWrite(pin_flag, HIGH);    //Set the flag line high still
  }
  //Add addition checks here.
  else{
    //No interrupts, set the flag pin low.
    digitalWrite(pin_flag, LOW); 
  } 
}
