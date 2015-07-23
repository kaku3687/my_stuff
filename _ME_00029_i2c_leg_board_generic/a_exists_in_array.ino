boolean exists_in_array(uint8_t* array1, int count, uint8_t a){
  if(find_array_position_of_pin(array1, count, a) != -1){
    return true;
  }
  else{
    return false;
  }
}

int find_array_position_of_pin(uint8_t* array1, int count, uint8_t a){
  
  for(int i = 0; i < count; ++i){
    if(array1[i] == a){
      return i;
    }
  }
  
  return -1;
}
