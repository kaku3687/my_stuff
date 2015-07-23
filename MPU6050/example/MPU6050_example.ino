#include <Wire.h>
#include <MPU6050.h>

// Prints degree change as read by the IMU

// Structure for reading and storing pair bytes from IMU
typedef union accel_t_gyro_union
{
  struct
  {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  }
  reg;
  struct
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  }
  value;
};

MPU6050 IMU;                      // Creating IMU Object
accel_t_gyro_union accel_t_gyro;  // Creating the IMU data Union

// DEFINING VARIABLES
//double gy_ofst_x, gy_ofst_y, gy_ofst_z;
uint8_t gy_sensitivity = 131;      // Gyroscope Sensitivity Mode divider
float t_last;
int dirAngle;
int count = 0;


void setup()
{
  int error;
  uint8_t c;

  // ********************************************* //
  //                   IMU SETUP                   //
  Serial.begin(9600);
  Serial.println(F("InvenSense MPU-6050"));
  Serial.println(F("June 2012"));


  // Clearing I2C Clock Line
  pinMode(P1_7, INPUT);
  if(!digitalRead(P1_7))
  {
    pinMode(P1_6, OUTPUT);
    while (!digitalRead(P1_7))
    {
      digitalWrite(P1_6, HIGH);
      digitalWrite(P1_6, LOW);
      Serial.println("Clock cycling...");
    }
  }

  // Initialize the 'Wire' class for the I2C-bus.
  Wire.begin();


  //    default at power-up:
  //    Gyro at 250 degrees second
  //    Acceleration at 2g
  //    Clock source at internal 8MHz
  //    The device is in sleep mode.

  error = IMU.MPU_read (MPU6050_WHO_AM_I, &c, 1);
  Serial.print(F("WHO_AM_I : "));
  Serial.print(c,HEX);
  Serial.print(F(", error = "));
  Serial.println(error,DEC);

  // According to the datasheet, the 'sleep' bit
  // should read a '1'.
  // That bit has to be cleared, since the sensor
  // is in sleep mode at power-up.
  error = IMU.MPU_read (MPU6050_PWR_MGMT_1, &c, 1);
  Serial.print(F("PWR_MGMT_1 : "));
  Serial.print(c,HEX);
  Serial.print(F(", error = "));
  Serial.println(error,DEC);


  // Clear the 'sleep' bit to start the sensor.
  IMU.MPU_write_reg (MPU6050_PWR_MGMT_1, 0);
  //                 End IMU Setup                 //
  // ********************************************* //
  delay(1000);
}

void loop()
{
  int error;
  double dT;

  // Read the raw values.
  // Read 14 bytes at once,
  // containing acceleration, temperature and gyro.
  // With the default settings of the MPU-6050,
  // there is no filter enabled, and the values
  // are not very stable.
  error = IMU.MPU_read(MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));

  swapAll();

  float t_now = millis();

  dT = (t_now - t_last)/1000;

  // Calculate degrees from raw gyro values
  // refined var =   raw value  / gyro sensitivity * change in time
  int gy_ang_x = (accel_t_gyro.value.x_gyro / gy_sensitivity) * dT;
  int gy_ang_y = (accel_t_gyro.value.y_gyro / gy_sensitivity) * dT;
  int gy_ang_z = (accel_t_gyro.value.z_gyro / gy_sensitivity) * dT;

  t_last = t_now;

  Serial.print(F("gyro dt,x,y,z : "));
  Serial.print(dT);
  Serial.print(F(", "));
  Serial.print(gy_ang_x, DEC);
  Serial.print(F(", "));
  Serial.print(gy_ang_y, DEC);
  Serial.print(F(", "));
  Serial.print(gy_ang_z, DEC);
  Serial.print(F(", "));
  Serial.println(F(""));

  delay(50);
}

void swapAll()
{
  // Swap all high and low bytes.
  // After this, the registers values are swapped,
  // so the structure name like x_accel_l does no
  // longer contain the lower byte.
  uint8_t swap;
  #define SWAP(x,y) swap = x; x = y; y = swap

  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);
}
