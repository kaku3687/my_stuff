//Command codes for I2C Message protocol, CC stands for command code
#define COMMAND_CODE_TEST                      0x01
#define COMMAND_CODE_MODULE_ID                 0x02
#define COMMAND_CODE_MODULE_VERSION            0x03
#define COMMAND_CODE_MODULE_RESET              0x04

#define COMMAND_CODE_DIGITAL_WRITE_PIN         0x19
#define COMMAND_CODE_ANALOG_WRITE_PIN          0x20
#define COMMAND_CODE_DIGITAL_READ              0x21
#define COMMAND_CODE_ANALOG_READ               0x22
#define COMMAND_CODE_DIGITAL_INTERRUPT         0x25
#define COMMAND_CODE_DIGITAL_INTERRUPT_CHECK   0x26
#define COMMAND_CODE_DIGITAL_INTERRUPT_CLEAR   0x27

#define COMMAND_CODE_SET_VREF                  0x30
