/* Basic Project to test programming to the Ardino Uno
   This project uses the Arduino Framework
   This project is compiled using Platform.io extension for VSCode
   The Check mark on the bottom of the screen in the blue banner is to build 
   The Arrow is to upload to the board. Note it will auto select COM port. 
   If a serial com is set up then the plug icon can be used to view it.
   Note that you also need to download the Aduino software first for it all to work.  

   The Basic Program flashes output digital 13/PB5 at the rate of the specified delay. 
   The program will also print out to the terminal via serial coms after each loop. 
*/


#include <Arduino.h>

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(13, HIGH);
  delay(1000);

  Serial.println("Loop Completed");
}
