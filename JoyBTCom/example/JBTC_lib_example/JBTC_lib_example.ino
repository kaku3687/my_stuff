#include <JoyBTCom.h>

typedef union buttons_union
{
  struct
  {
    boolean b1;
    boolean b2;
    boolean b3;
    boolean b4;
    boolean b5;
    boolean b6;
  }
  buttons;
  struct
  {
    uint8_t buttonByte;
  }
  groups;
};

JoyBTCom JBTC;
buttons_union phone_remote;

void setup()
{
  JBTC.start(9600);                 //Starting up the UART communication
  pinMode(P1_0, OUTPUT);            // LED TEST PIN
  digitalWrite(P1_0, LOW);
}

void loop()
{
  JBTC.update();
  
  phone_remote.groups.buttonByte = JBTC.getButtonStatus();
  
  if (phone_remote.buttons.b6)
  {
    JBTC.sendBlueToothData("1", "1", "Active");
    if (phone_remote.buttons.b1)
    {
      digitalWrite(P1_0, HIGH);
    }
    else
    {
      digitalWrite(P1_0, LOW);
    }
  }
  delay(50);
}
