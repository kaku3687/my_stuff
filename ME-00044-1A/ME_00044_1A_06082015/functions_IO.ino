void analog_write( unsigned int pinNum, unsigned int value){
  pinMode(pinNum,OUTPUT);
  analogWrite(pinNum,value);
}

// interrupt routines for each pin pass the pin number to digital_interrupt()
void digital_interrupt(int pin){  
  noInterrupts();
  detachInterrupt(pin);
  pins_interrupted[pin]=true; 
  pinMode(FLAG_pin, OUTPUT);
  digitalWrite(FLAG_pin, HIGH);  
  interrupts();  
}

void digital_interrupt_pin2(){digital_interrupt(2);}
void digital_interrupt_pin3(){digital_interrupt(3);}
void digital_interrupt_pin4(){digital_interrupt(4);}
void digital_interrupt_pin5(){digital_interrupt(5);}
void digital_interrupt_pin6(){digital_interrupt(6);}
void digital_interrupt_pin7(){digital_interrupt(7);}
void digital_interrupt_pin9(){digital_interrupt(9);}
void digital_interrupt_pin10(){digital_interrupt(10);}
void digital_interrupt_pin13(){digital_interrupt(13);}
void digital_interrupt_pin19(){digital_interrupt(19);}


//checks if FLAG_pin is on. go throughs pins_interrupted array[] to check if pin is flagged. 
//if no alerts are on turns off alert pin.
byte check_array(){ 
  for(byte i = 2;i <= 19;i++){
    if(pins_interrupted[i]==true){
      pinMode(FLAG_pin, OUTPUT);
      digitalWrite(FLAG_pin, HIGH);
      return i;
    }        
  }
  
  //pinMode(FLAG_pin, OUTPUT);
  //pinMode(FLAG_pin, LOW);
  //pinMode(FLAG_pin, INPUT);
  return 0xff;
}


//

void cc_digital_write(unsigned char & pinNum, unsigned char & state){
  if( ((pinNum >= 2) & (pinNum <= 7)) | (pinNum == 9) | (pinNum == 10) | (pinNum == 13) | (pinNum == 19) ){
    pinMode(pinNum,OUTPUT);
    if(state== 0x01){
      digitalWrite(pinNum,HIGH);
    }
    else{
      digitalWrite(pinNum,LOW);
    }  
  }
}

void cc_analog_write(unsigned char & pinNum, unsigned char & value){
   if( (pinNum == 4) | (pinNum == 9) | (pinNum == 10) | (pinNum == 13) | (pinNum == 19) ){
    pinMode(pinNum,OUTPUT);
    analogWrite(pinNum, value);
   }
}

void cc_digital_read(unsigned char & pinNum, unsigned char & res){
  if( ((pinNum >= 2) & (pinNum <= 7)) | (pinNum == 9) | (pinNum == 10) | (pinNum == 13) | (pinNum == 19) ){
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

void cc_analog_read(unsigned char & pinNum, unsigned char & res){
  unsigned char analogPin;
  
  switch(pinNum){
    case 2:
      analogPin = A0;
      break; 
    case 3:
      analogPin = A1;
      break; 
    case 4:
      analogPin = A2;
      break; 
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
      break;  
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

void cc_digital_interrupt(unsigned char & pinNum, unsigned char & res, unsigned char & type){
  if( ((pinNum >= 2) & (pinNum <= 7)) | (pinNum == 9) | (pinNum == 10) | (pinNum == 13) | (pinNum == 19) ){
    detachInterrupt(pinNum);
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

void cc_digital_interrupt_check(){
  
}

void cc_digital_interrupt_clear(unsigned char & pinNum){
  if( ((pinNum >= 2) & (pinNum <= 7)) | (pinNum == 9) | (pinNum == 10) | (pinNum == 13) | (pinNum == 19) ){
    if( pins_interrupted[pinNum] == true){
      pins_interrupted[pinNum] = false;
      detachInterrupt(pinNum); 
      //pinMode(pinNum, OUTPUT);
      //digitalWrite(pinNum, LOW); 
      pinMode(pinNum, INPUT);  // UPDATED SETS PIN TO INPUT WITH NO PULLUP OR PULLDOWN
    }
  }
  
  if(check_array() == 255){
    pinMode(FLAG_pin, OUTPUT);
    digitalWrite(FLAG_pin, LOW); 
    pinMode(FLAG_pin, INPUT);  // UPDATED SETS PIN TO INPUT WITH NO PULLUP OR PULLDOWN
  }
}






