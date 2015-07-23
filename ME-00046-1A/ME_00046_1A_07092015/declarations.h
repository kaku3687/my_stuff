#define STEPPER_STATUS_READY          1
#define STEPPER_STATUS_COMPLETED      2
#define STEPPER_STATUS_BUSY           3

//Callback type for functions
typedef void (*callBack)(void);
volatile unsigned char status;

volatile unsigned int NUM_STEPS = 64; //default
