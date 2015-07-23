/*
Inconsistent location on zeroing using the limit switch. 
*This could be the way the limit switch is hit. It is -/+ 1 thread

There is no timeout checking for zeroing function.
*This can cause a problem if the limit switch is already depress which would cause the motor to keep spinning inward
*The limit switch never triggers the falling interrupt, since it is already depressed :(

There could be a possible problem when sending a function command and the packet was corrupted via transportation
*This can cause a function call with unsafe/corrupted parameters
*The CRC check resend might be too late, since it is already executing before the CRC check, and limit switches may have already been triggered and this may cause problems.

Need to work on
*Possible limit switch timeout
*C++ Raspberry Pi libraries for the functionalize changes
*(On MSP430 firmware) Optimize memory usage, currently using over 15kb out of 16kb. No more space for stuff! The math library is costly in memory(???).
*

Inaccurate counts
*Once lead screw is too far away. The lead screw starts to wobble which creates problems when reading the optical switch.
*Values seems to be greater than 12, once the bottom rods are past the plastic holes.
*It looks like the lead screw can come loose and cause inaccurate reading of the optical switch.

Motor Spindown
*When the motor is stopped after a click is detected it can continue to spin far enough to land on the switch or past it again
*The code is setup so when a movement is not being done counts are not being counted. Therefore, the motor is still spinning it can cause a drift because it can go past the sensor.
*Not a bulletproof, but somewhat reliable patch was to drop the speed down before the expected last click so that the motor will stop because it hits the sensor.
*Stalling code still in testing. If the motor stalls when the sensor is held (middle of the sqaure wave), the motor will spin in the reverse direction to clear the the switch. If the limit switch is not clear with the reverse direction, an interrupt is never sent to the RPI.

Stalls/Timeouts
*Stalls and timeouts can cause drift because the stall can occur right at the sensor which causes problems with counts.


Memory problem... Running out of memory causes crashes. This usually occurs during an I2C read/write when the receiveEvent function is called

06/03/2014 - Fixed problem when FSR threshold was reached and switch was held. This would cause the motor to stop, but continue after the backdrive(clearing the switch). Currently fixed.
            *Need to test and fix possible problems with keeping track of coutns with FSR.
             ->Changed code for the way that counts are kept track of. Testing needs to be done to ensure it works correctly. 
             ->Works by checking if the motor direction is the same during switch pass thru. If not, (because of a stall or FSR threshold), the count is negated. If the same it adds or subs accordingly.
             ->Removed attempts to clear switch by back driving. Should handle any counting problems.
*/
