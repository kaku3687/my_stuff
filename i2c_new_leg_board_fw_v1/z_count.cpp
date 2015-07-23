#include "z_count.h"

#include <Energia.h> 

uint8_t z_count_pin_rot;
int dEBUG;

void z_count_setup(uint8_t _z_count_pin_rot){
  dEBUG = 0;
  z_count_pin_rot = _z_count_pin_rot;
  pinMode(z_count_pin_rot, INPUT);
  attachInterrupt(z_count_pin_rot, z_falling, FALLING); 
}

void z_falling(){
  detachInterrupt(z_count_pin_rot); 
  attachInterrupt(z_count_pin_rot, z_rising, RISING);
}

void z_rising(){
  detachInterrupt(z_count_pin_rot);
  attachInterrupt(z_count_pin_rot, z_falling, FALLING);
  dEBUG ++;
}
