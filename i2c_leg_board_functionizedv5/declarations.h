
//Declalralljsalkjdasljdsaljdslajdlkasjlkdjxjkddjkashdksa
//Declaration for callback type for zfunc_leg
#define TIMEOUT_TIME               2000
typedef void (*zcallBack)(void);
volatile unsigned char status;

//used for analog reference
//Keeps track of current voltage reference
unsigned char vref = 33;


int rot_count = 0;  //Keeps track of the current rotation count, temporary set to 0 to test.
//PROGMEM prog_uint16_t aaa = 3;
