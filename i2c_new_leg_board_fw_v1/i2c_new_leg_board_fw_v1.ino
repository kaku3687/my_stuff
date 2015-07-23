#include "pin_definitions.h"
#include "z_count.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600); 
  z_count_setup(P1_4);
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.print("Count = " ); 
  Serial.println(dEBUG);
  delay(100);
}
