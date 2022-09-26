/* This project is a test of Serial Monitoring
   We are using the Arduino Sketch style code for an inital test
   This same code can be copied and pasted into Arduino IDE and will work

   Once code is loaded, you can click on the PlatformIO: Serial Monitor button (plug button)
   to open a serial communication to the COM PORT associated with the Arduino Uno connected.

   Note: If the Arduino is power cycled or communication inturrupted in any way, a new Serial Terminal will need to be opened. 

   You can also open Putty to open your own Serial monitoring to the Arduino associated COM PORT
   Keep in mind that only one program can be connected to the COM PORT for communication at a time
   ie. Only the PlatformIO or the Putty Terminal can be open, not both. 
   You can also use the Arduino IDE Serial Monitor

   Wiring:
   We only need the USB to program and power the Arduino connected to the PC
   Check to see what COM PORT the Arduino is conencted to under Device Manager in Windows

*/

#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  while(!Serial); //waits for serial to be ready to use
  Serial.println("\nStarting Serial Monitor");
  int count = 0;
}

void loop() {
  delay(1000);
  Serial.println("\nTesting Serial Monitor");
  // Serial.print(count);
  // count++;
}


