// Project Description:
// This project will program an ATtiny85 flash 3 leds in sequence based on a potentiometer input
// This project can be found at https://www.instructables.com/Learn-How-to-Use-the-ATtiny85-in-UNDER-4-Minutes/

// Steps to gettting Arduino IDE Setup for ATtiny85 that already have bootloader on them
// 1. Go to File/Examples/11.ArduinoISP and click to open the example. Upload the program an Arduino Uno with the example code
//    and it is now an ISP programmer. We are using SPI serial protocol to program.
// 2. Open a new Sketch
// 3. Go to File/Preferences and in window paste the following into the "Additional Board Manager URL" section and click OK
//    https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json
// 4. Go to Tool/Board:/Board Manager search for ATtiny and install
// 5. Go to Tools/Board and select ATtiny25/45/85
// 6. Go to Tools/Processor and select ATtiny85
// 7. Go to Tools/Clock and select 1Mhz
// 8. Go to Tools/COM Port and select the port for the Arduino Uno
// 9. Go to Tools/Programmer and select Arduino as ISP
// 10. Wire project up and then click to upload code

// Arduino as ISP Wiring:
// Vcc to Arduino 5v
// GND to Arduino GND
// PB5 to Arduino Digital 10
// PB0 to Arduino Digital 11
// PB1 to Arduino Digital 12
// PB2 to Arduino Digital 13

// ATtiny85 Wiring for Project:
// Vcc to Arduino 5v (or other power source)
// GND to Arduino GND 
// PB0 to resistor to LED to GND
// PB1 to resistor to LED to GND
// PB2 to resistor to LED to GND
// PB3/ADC3 to middle of 10k pot for ADC input (10k pot has one outer side to 5v and the other to GND)

// Original Creater Comments:
// Written by Martin Vrana, 2017, you are free to modify this code
// Because the Attiny85 has only some statements that are guaraneed to work in every configuration,I wrote this code to utilize as many pins and make it interesting.
// Try other commands and modify this one if you want, but be wary that some code might not work.


int val = 0; //This integer variable will serve as a way to store the analog value aquired from the potentiometer

void setup() {
  //The pins set up to serve as Digital output pins for LED's   
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
}

void loop() {
  //First the analog value on analog pin 3 is read and stored into val
  val = analogRead(3);
  
  //Then the first LED is turned on for the delay set by the potentiometer reading val
  digitalWrite(0, HIGH);
  delay(val);
  digitalWrite(0, LOW);
  
  //After that the LED is turned off, and the process repeats
  digitalWrite(1, HIGH);
  delay(val);
  digitalWrite(1, LOW);
  
  digitalWrite(2, HIGH);
  delay(val);
  digitalWrite(2, LOW);
} 
