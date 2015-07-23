//Library built off of kas's BT sample code: http://forum.arduino.cc/index.php?topic=173246.0

// ADD SUPPORT FOR VIRTUAL UART CONNECTIONS
// ADD BUTTON SETUP TOGGLE V MOMENTARY

#include "JoyBTCom.h"
#include <Arduino.h>

#define    STX          0x02
#define    ETX          0x03
#define    SLOW         750                            // Datafields refresh rate (ms)
#define    FAST         250                             // Datafields refresh rate (ms)

unsigned char cmd[8] = {0, 0, 0, 0, 0, 0, 0, 0};                 // unsigned chars received
unsigned char buttonStatus = 0;                                  // first unsigned char sent to Android device
int joyX = 0;
int joyY = 0;
long previousMillis = 0;                                // will store last time Buttons status was updated
long sendInterval = SLOW;                               // interval between Buttons status transmission (milliseconds)

JoyBTCom::JoyBTCom() {}

void JoyBTCom::start(int baud) {
  Serial.begin(baud);
  while(Serial.available()) {Serial.read();}           // empty RX buffer
}

void JoyBTCom::update() {
  if(Serial.available())  {                           // data received from smartphone
    delay(2);
    cmd[0] =  Serial.read();
    if(cmd[0] == STX)  {
      int i=1;
      while(Serial.available())  {
        delay(1);
        cmd[i] = Serial.read();
        if(cmd[i]>127 || i>7)                 break;     // Communication error
        if((cmd[i]==ETX) && (i==2 || i==7))   break;     // Button or Joystick data
        i++;
      }
      if     (i==2)          getButtonState(cmd[1]);    // 3 unsigned chars  ex: < STX "C" ETX >
      else if(i==7)          getJoyState(cmd);     // 6 unsigned chars  ex: < STX "200" "180" ETX >
    }
  }
}

void JoyBTCom::sendBlueToothData(String data_1, String data_2, String data_3)  {
  static long previousMillis = 0;
  long currentMillis = millis();
  if(currentMillis - previousMillis > sendInterval) {   // send data back to smartphone
    previousMillis = currentMillis;

// Data frame transmitted back from Arduino to Android device:
// < 0X02   Buttons state   0X01   DataField#1   0x04   DataField#2   0x05   DataField#3    0x03 >
// < 0X02      "01011"      0X01     "120.00"    0x04     "-4500"     0x05  "Motor enabled" 0x03 >    // example

    Serial.print((char)STX);                                             // Start of Transmission
    Serial.print(getButtonStatusString());  Serial.print((char)0x1);   // buttons status feedback
    Serial.print(data_1);                   Serial.print((char)0x4);   // datafield #1
    Serial.print(data_2);                   Serial.print((char)0x5);   // datafield #2
    Serial.print(data_3);                                         // datafield #3
    Serial.print((char)ETX);                                             // End of Transmission
  }
}

bool JoyBTCom::getButton(int bNum) {
  if (buttonStatus >>(bNum-1) & B000001) {
    return 1;
  }
  else {
    return 0;
  }
}

String JoyBTCom::getButtonStatusString()  {
  String bStatus = "";
  for(int i=0; i<6; i++)  {
    if(buttonStatus & (B100000 >>i))      bStatus += "1";
    else                                  bStatus += "0";
  }
  return bStatus;
}

void JoyBTCom::getJoyState(unsigned char data[8])    {
  joyX = (data[1]-48)*100 + (data[2]-48)*10 + (data[3]-48);       // obtain the Int from the ASCII representation
  joyY = (data[4]-48)*100 + (data[5]-48)*10 + (data[6]-48);
  joyX = joyX - 200;                                                  // Offset to avoid
  joyY = joyY - 200;                                                  // transmitting negative numbers

  if(joyX<-100 || joyX>100 || joyY<-100 || joyY>100)     return;      // commmunication error
}

signed int JoyBTCom::getJoyX() {return joyX;}
signed int JoyBTCom::getJoyX() {return joyY;}

void JoyBTCom::getButtonState(int bStatus)  {
  switch (bStatus) {
// -----------------  BUTTON #1  -----------------------
    case 'A':
      buttonStatus |= B000001;        // ON
      break;
    case 'B':
      buttonStatus &= B111110;        // OFF
      break;

// -----------------  BUTTON #2  -----------------------
    case 'C':
      buttonStatus |= B000010;        // ON
      break;
    case 'D':
      buttonStatus &= B111101;        // OFF
      break;

// -----------------  BUTTON #3  -----------------------
    case 'E':
      buttonStatus |= B000100;        // ON
      break;
    case 'F':
      buttonStatus &= B111011;      // OFF
      break;

// -----------------  BUTTON #4  -----------------------
    case 'G':
      buttonStatus |= B001000;       // ON
      break;
    case 'H':
      buttonStatus &= B110111;    // OFF
     break;

// -----------------  BUTTON #5  -----------------------
    case 'I':           // configured as momentary button
      buttonStatus |= B010000;        // ON
      break;
   case 'J':
      buttonStatus &= B101111;        // OFF
      break;

// -----------------  BUTTON #6  -----------------------
    case 'K':
      buttonStatus |= B100000;        // ON
      break;
    case 'L':
      buttonStatus &= B011111;        // OFF
      break;
  }
// ---------------------------------------------------------------
}
