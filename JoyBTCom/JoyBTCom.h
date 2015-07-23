#ifndef JoyBTCom_h
#define JoyBTCom_h

#include <Arduino.h>

class JoyBTCom {
public:
    JoyBTCom();
    void start(int baud);
    void update();
    void sendBlueToothData(String data_1, String data_2, String data_3);
    bool getButton(int bNum);
    String getButtonStatusString();
    signed int getJoyX();
    signed int getJoyY();

private:
    void getButtonState(int bStatus);
    void getJoyState(unsigned char data[8]);
    void read();
};

#endif
