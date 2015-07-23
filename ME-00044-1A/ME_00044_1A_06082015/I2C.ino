//Stores command code for request data sending, see command code list above
unsigned int current_command_code = 0; 

//Size of the data to be written to I2C Master
unsigned int data_write_size = 0;

//A buffer array for the write bytes to the I2C Master
unsigned char data_write_array[16] = { 
  0};
  
//A buffer array for the read in bytes from I2C Master
unsigned char data_read_array[16] = {
  0};
  
//Function call that will read data from the I2C Master to the I2C slave device(this device)
//One parameter is need, the number of bytes to read
void readI2C(unsigned int count){
  for(unsigned int i = 0; i < count; i ++){      //Loop that reads in bytes based on the count value
    data_read_array[i] = Wire.read();  //Read the available byte into the data_read_array
  } 
}
