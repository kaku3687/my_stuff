#ifndef _Z_COUNT_
#define _Z_COUNT_

#include <Energia.h> 


extern uint8_t z_count_pin_rot;
extern int dEBUG;

void z_count_setup(uint8_t _z_count_pin_rot);
void z_falling(void);
void z_rising(void);


#endif
