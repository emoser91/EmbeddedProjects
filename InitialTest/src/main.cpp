/*
  This is a basic Test File to test the basic connection to a Arduino Uno
  This code will blink the on board LED on the Arduino Uno
  This was taken as an example from the Arduino Program Examples
  This relises on the Arduino IDE Program and the Sketch file system
  This can be ran on either the Arduino IDE or with the Addon PIO within Visual Studio Code. 
*/

#include <Arduino.h>

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(100);

  Serial.println("Loop Completed");
}