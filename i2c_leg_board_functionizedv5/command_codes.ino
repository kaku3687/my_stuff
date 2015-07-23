//COMMAND CODES
#define COMMAND_CODE_TEST                       0x01
#define COMMAND_CODE_MODULE_ID                  0x02  
#define COMMAND_CODE_MODULE_VERSION             0x03
#define COMMAND_CODE_MODULE_RESET               0x04

#define COMMAND_CODE_SET_SERVO_PULSE_WIDTH      0x10     //Set servo to requested pulse width between min/max values as attached, Packet format(4 bytes): CC  |  Servo#(3Bits) + PWM_Upper(5Bits)  |  PWM(Lower)
#define COMMAND_CODE_REQUEST_SERVO_PULSE_WIDTH  0x11     //Request a servo PWM, Packet format(2 bytes): CC  |  Servo#
                                                         //Return data(2 bytes):  PWM_Upper  |  PWM_Lower
                                                         
#define COMMAND_CODE_SET_DIR                    0x12
#define COMMAND_CODE_SET_SPD                    0x13
#define COMMAND_CODE_GET_DIR                    0x14
#define COMMAND_CODE_GET_SPD                    0x15

#define COMMAND_CODE_DIGITAL_WRITE_PIN          0x19
#define COMMAND_CODE_ANALOG_WRITE_PIN           0x20
#define COMMAND_CODE_DIGITAL_READ               0x21
#define COMMAND_CODE_ANALOG_READ                0x22
#define COMMAND_CODE_DIGITAL_INTERRUPT          0x25
#define COMMAND_CODE_DIGITAL_INTERRUPT_CHECK    0x26
#define COMMAND_CODE_DIGITAL_INTERRUPT_CLEAR    0x27
#define COMMAND_CODE_DIGITAL_INTERRUPT_DEBOUNCE 0x28

#define COMMAND_CODE_SET_VREF                   0x30


#define COMMAND_CODE_LEG_INIT                  0x40
#define COMMAND_CODE_LEG_ZERO                  0x41
#define COMMAND_CODE_LEG_MOVE                  0x42
#define COMMAND_CODE_LEG_STEP                  0x43
#define COMMAND_CODE_LEG_STOP                  0x47
#define COMMAND_CODE_LEG_FUNC_STATUS_CHECK     0x48
#define COMMAND_CODE_LEG_FUNC_STATUS_CLEAR     0x49
#define COMMAND_CODE_LEG_SET_THRESHOLD         0x4A
#define COMMAND_CODE_LEG_SET_THRESHOLD_ENABLE  0x4B
#define COMMAND_CODE_LEG_MOVE_Z		       0X4C
#define COMMAND_CODE_LEG_SET_MAINTAINED_HEIGHT 0x4D
