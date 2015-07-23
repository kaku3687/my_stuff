//******************************************************************//
//********************************CRC*******************************//
//******************************************************************//
unsigned int getCRC_read_array(unsigned char command_code, unsigned char *data, unsigned int length){
  unsigned int count;
  unsigned int crc = 0xffff;
  unsigned int temp;

  temp = (command_code ^ (crc >> 8)) & 0xff;
  crc = crc_table[temp] ^ (crc << 8);

  for (count = 0; count < length; ++count)
   {
     temp = (*data++ ^ (crc >> 8)) & 0xff;
     crc = crc_table[temp] ^ (crc << 8);
   }
   
  crc = (crc ^ 0x0000);
   
  return crc;
}

unsigned int getCRC_write_array(unsigned int packet_crc, unsigned char *data, unsigned int length){
  unsigned int count;
  unsigned int crc = 0xffff;
  unsigned int temp;

  temp = (((crc >> 8) & 0xFF) ^ (crc >> 8)) & 0xff;
  crc = crc_table[temp] ^ (crc << 8);
  
  temp = ((crc & 0xFF) ^ (crc >> 8)) & 0xff;
  crc = crc_table[temp] ^ (crc << 8);

  for (count = 0; count < length; ++count)
   {
     temp = (*data++ ^ (crc >> 8)) & 0xff;
     crc = crc_table[temp] ^ (crc << 8);
   }
   
  crc = (crc ^ 0x0000);
   
  return crc;
}

void putCRC(unsigned int crc, unsigned char start_index){
   data_write_array[start_index] = (crc >> 8) & 0xFF;
   data_write_array[start_index + 1] = crc & 0xFF;
   data_write_size = data_write_size + 2;
}

void putCRC_read_only(unsigned char command_code, unsigned char *data, unsigned int length){
  putCRC(getCRC_read_array(command_code, data, length), 0);
}

void putCRC_read_write_only(unsigned char command_code, unsigned char *data, unsigned int rlength, unsigned int wlength){
  putCRC(getCRC_read_array(command_code, data, rlength), wlength);
}
//******************************************************************//
//******************************************************************//
//******************************************************************//


