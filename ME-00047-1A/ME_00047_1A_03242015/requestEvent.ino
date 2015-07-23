//Request data event
//This function is called when the I2C Master requests data from the I2C slave device(this device)
//The data to write and size must be set before the I2C Master requests data, else the requested data will be junk
void requestEvent()
{
  Wire.write(data_write_array, data_write_size);    //Write data to the I2C Master. data_write_array contains data bytes to write
                                                    //data_write_size contains the number of bytes to write
  data_write_size = 0;                              //After writing data, set the data_write_size to 0
}
