//IO RELATED VARIABLES
  //Keeps track of current voltage reference
//unsigned char vref = 33;
  //Keeps track of which pins were interrupted

/*
boolean pins_interrupted[8]={false};
  //Interrupt array
typedef void (* pin_interrupt) (void);
pin_interrupt pin_interrupt_array[] = {NULL, NULL, digital_interrupt_pin2, digital_interrupt_pin3, NULL, 
                                       digital_interrupt_pin5, digital_interrupt_pin6, digital_interrupt_pin7};
                                       
pin_interrupt pin_interrupt_falling_array[] = {NULL, NULL, digital_interrupt_falling_pin2, digital_interrupt_falling_pin3, NULL, 
                                       digital_interrupt_falling_pin5, digital_interrupt_falling_pin6, digital_interrupt_falling_pin7};
                                       
pin_interrupt pin_interrupt_rising_array[] = {NULL, NULL, digital_interrupt_rising_pin2, digital_interrupt_rising_pin3, NULL, 
                                       digital_interrupt_rising_pin5, digital_interrupt_rising_pin6, digital_interrupt_rising_pin7};
                                       
unsigned int pin_debounce_time[8] = {0};
unsigned long pin_previous_time[8] = {0};
//*************************

//******************************************************************
//********************************IO********************************
//******************************************************************
void digital_interrupt_pin2(){digital_interrupt(2);}
void digital_interrupt_pin3(){digital_interrupt(3);}
void digital_interrupt_pin5(){digital_interrupt(5);}
void digital_interrupt_pin6(){digital_interrupt(6);}
void digital_interrupt_pin7(){digital_interrupt(7);}

void digital_interrupt_falling_pin2(){digital_interrupt_falling(2);}
void digital_interrupt_falling_pin3(){digital_interrupt_falling(3);}
void digital_interrupt_falling_pin5(){digital_interrupt_falling(5);}
void digital_interrupt_falling_pin6(){digital_interrupt_falling(6);}
void digital_interrupt_falling_pin7(){digital_interrupt_falling(7);}

void digital_interrupt_rising_pin2(){digital_interrupt_rising(2);}
void digital_interrupt_rising_pin3(){digital_interrupt_rising(3);}
void digital_interrupt_rising_pin5(){digital_interrupt_rising(5);}
void digital_interrupt_rising_pin6(){digital_interrupt_rising(6);}
void digital_interrupt_rising_pin7(){digital_interrupt_rising(7);}

//Interrupt routines for each pin pass the pin number to digital_interrupt()
void digital_interrupt(int pinNum){  
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){   
    detachInterrupt(pinNum);
    pins_interrupted[pinNum] = true; 
    //update_flag();
    digitalWrite(pin_flag, HIGH);   
  }
}

void digital_interrupt_falling(int pinNum){  
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){ 
    if(digitalRead(pinNum) == LOW){
      pin_previous_time[pinNum] = millis();
      detachInterrupt(pinNum);
      attachInterrupt(pinNum, pin_interrupt_rising_array[pinNum], RISING);  
    }
  }
}

void digital_interrupt_rising(int pinNum){  
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    if( (millis() - pin_previous_time[pinNum]) > pin_debounce_time[pinNum] ){
        detachInterrupt(pinNum);
        pins_interrupted[pinNum] = true; 
        //update_flag();
        digitalWrite(pin_flag, HIGH);   
    }       
  }
}

//Analog write function
void analog_write( unsigned int pinNum, unsigned int value){
  pinMode(pinNum, OUTPUT);
  analogWrite(pinNum, value);
}

//checks if FLAG_pin is on. go throughs pins_interrupted array[] to check if pin is flagged. 
//if no alerts are on turns off alert pin.
byte check_array(){ 
  for(byte i = 2; i <= 7; i++){
    if(pins_interrupted[i] == true){
      //update_flag();
      //digitalWrite(pin_flag, HIGH);
      return i;
    }        
  }
  return 0xff;
}

//Digital write function
void cc_digital_write(unsigned char & pinNum, unsigned char & state){
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    pinMode(pinNum,OUTPUT);
    if(state == 0x01)
      digitalWrite(pinNum, HIGH);
    else
      digitalWrite(pinNum, LOW); 
  }
}

//Analog write function
void cc_analog_write(unsigned char & pinNum, unsigned char & value){
  //This board has no analog write pins usable 
  return;
}
*/
//Digital read function
void cc_digital_read(unsigned char & pinNum, unsigned char & res){
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    switch(res){
      case 0:
        pinMode(pinNum, INPUT);
        break;
      case 2:
        pinMode(pinNum, INPUT_PULLUP);
        break;
      case 4:
        pinMode(pinNum, INPUT_PULLDOWN);
        break;
      default:
        data_write_array[0]=0xff;
        data_write_array[1]=0xff;
        return;
    }
  }
    
  data_write_array[0] = pinNum;
    
  if (digitalRead(pinNum) == HIGH)
    data_write_array[1]=0x01;
  else
    data_write_array[1]=0x00;     
    
  return; 
}

void cc_set_vref(unsigned char & _vref){
  switch(_vref){
    case 33:
     analogReference(DEFAULT);    //Set analog reference
     vref = 33;
     break;
     
    case 15:
     analogReference(INTERNAL1V5);    //Set analog reference
     vref = 15;
     break; 
     
    case 25:
     analogReference(INTERNAL2V5);    //Set analog reference
     vref = 25;
     break; 
     
    case 255:
     analogReference(EXTERNAL);    //Set analog reference
     vref = 255;
     break;
     
    default:
      break;
  }
}

//Analog write function
void cc_analog_read(unsigned char pinNum, unsigned char res){
  unsigned char analogPin;
  
  switch(pinNum){
    case 5:
      analogPin = A3;
      break; 
    case 6:
      analogPin = A4;
      break; 
    case 7:
      analogPin = A5;
      break; 
    default:
      data_write_array[0]=0xff;
      data_write_array[1]=0xff;
      data_write_array[2]=0xff;
      data_write_array[3]=0xff;
      return;  
  }
  
   switch(res){
    case 0:
      pinMode(analogPin, INPUT);
      break;
    case 2:
      pinMode(analogPin, INPUT_PULLUP);
      break;
    case 4:
      pinMode(analogPin, INPUT_PULLDOWN);
      break;
    default:
      data_write_array[0]=0xff;
      data_write_array[1]=0xff;
      data_write_array[2]=0xff;
      data_write_array[3]=0xff;
      return;
   }
   
  unsigned int aRead = analogRead(analogPin);
  
  data_write_array[0] = pinNum;
  data_write_array[1] = vref;
  data_write_array[2]= (aRead >> 8) & 0x03; 
  data_write_array[3]= 0xFF & aRead;  

  return; 
}
/*
//Attaches a digital interrupt to a pin
void cc_digital_interrupt(unsigned char & pinNum, unsigned char & res, unsigned char & type){
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    
    detachInterrupt(pinNum); //Detaches any interrupt already attached
    int resType, edgeType;
    
    switch(res){
      case 0:
        resType = INPUT;
        break;
      case 2:
        resType = INPUT_PULLUP;
        break;
      case 4:
        resType = INPUT_PULLDOWN;
        break;
      default:
        data_write_array[0]=0xff;
        return;
    }
    
    switch(type){
      case 2:
        edgeType = FALLING;
        break;
      case 3:
        edgeType = RISING;
        break;
      default:
        data_write_array[0]=0xff;
        return;
    }
    
    pinMode(pinNum, resType);
    attachInterrupt(pinNum, pin_interrupt_array[pinNum], edgeType);   
  }
  
  return;
}

void cc_digital_interrupt_debounce(unsigned char & pinNum, unsigned char & debounce_upper, unsigned char & debounce_lower){
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    detachInterrupt(pinNum); //Detaches any interrupt already attached
    
    pin_debounce_time[pinNum] = (debounce_upper << 8) + debounce_lower; //Combines two bytes into the debounce time in ms
    pinMode(pinNum, INPUT_PULLUP);
    attachInterrupt(pinNum, pin_interrupt_falling_array[pinNum], FALLING);      
  }
}

void cc_digital_interrupt_check(){
  
}

void cc_digital_interrupt_clear(unsigned char & pinNum){
  if( ((pinNum >= 2) & (pinNum <= 7)) & (pinNum != 4) ){
    if( pins_interrupted[pinNum] == true){
      pins_interrupted[pinNum] = false;
      detachInterrupt(pinNum);  
      //pinMode(pinNum, INPUT);  // UPDATED SETS PIN TO INPUT WITH NO PULLUP OR PULLDOWN
    }
  }
  
  update_flag();
  
  //if(check_array() == 255){    // No other interrupts pending
  //  digitalWrite(pin_flag, LOW); 
  //}
}
*/
//******************************************************************//
//******************************************************************//
//******************************************************************//
