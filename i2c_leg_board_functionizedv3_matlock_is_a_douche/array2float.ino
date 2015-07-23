float array2float(unsigned char* floatarray){                      //Returns a float from an array, pass starting pointer, reads 4 bytes to interrupt as float  
  union u_tag {
      byte b[4];
      float fval;
  } u;
  
  u.b[0] = floatarray[0];
  u.b[1] = floatarray[1];
  u.b[2] = floatarray[2];
  u.b[3] = floatarray[3];

  return u.fval;
}

/*
void float2array(float f, unsigned char* targetArray ){  //Converts an input float to 4 bytes and puts it in the target array
  union u_tag {
      byte b[4];
      float fval;
  } u;
  
  u.fval = f; 
  
  targetArray[0] = u.b[0];
  targetArray[1] = u.b[1];
  targetArray[2] = u.b[2];
  targetArray[3] = u.b[3]; 
}
*/
