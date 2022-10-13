/* This Project gets an Atmega328p chip working on a breadboard

  The Atmega328p chip in the Arduino Uno already has a bootloader so we do not have to worry about that.
  If you had a bare chip there is documentation in the folder on how to flash the bootloader to it with
  the use of a second Arduino Uno as a programmer.

  You can just program the Atmega328p chip sitting in the Arduino Uno board with this program and then
  remove it and place it on the breadboard and wire it up as shown in the wiring digram pictures
  in the documents folder of this project. Alternatively, you can also program the chip already wired
  up on the breadboard with a second Arduino Uno which is shown in the Arduino Document in the folder.
  I believe you might also be able to use the PIC3 Programmer I own to program it as well but it might
  need a little more setup to do so.

  It is imporatant to use the right load capacitors with you osillator crystal and there are documents
  included that show how to go about that calculation with a formula.

  As for powering the Atmega328p chip, you can either use a second Arduino Uno and run power and Ground
  wires from it or you can use the Power Supply breakout board that comes with the Sensor kit that can
  accept multiple different power connectors. The barrel DC connector from the Arduino power adaptor
  might work well.

  It is also a good idea to use a voltage regulator circuit to ensure safe voltage reaches the
  Atmega328p chip.

  The Atmega328p chip in the Arduino Uno also has modified FUSE BITS to set it up to run an external
  clock source as well as a few other settings. The Atmega328p has an internal 8Mhz clock that can be
  set by setting the FUSE BITS. I guess the internal 8Mhz clock has a horrible rate of error of 10%
  so it isnt really useful for anything that needs accurate timing. You will need to use the Arduino IDE
  to set the fuse bits as that what all the examples use. The following Youtube video shows
  how you can set FUSE BITS: https://www.youtube.com/watch?v=Q2DakPocvfs. There is a second method
  of setting FUSE BITS shown in the Arduino Document in the folder. It has you download a hardware
  configuration file and then place it in a specific location to allow the Arduino IDE to set the
  FUSE BITS when it programs.

  Link to Arduino Document: https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoToBreadboard
  Link to Wiring Setup: https://www.instructables.com/How-to-Build-an-Arduino-Uno-on-a-BreadBoard/

*/
// AVR
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// C
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// #include <Arduino.h>

// Prepare LED pin
void PortInit(void)
{
    //PORTB5/Digital 13 on-board LED ON
    DDRB |= (1<<PORTB5);
}

void FlashLED(void)
{
  PORTB |= (1<<PORTB5); //LED ON
  _delay_ms(200); 
  PORTB &= ~(1<<PORTB5); //LED OFF
  _delay_ms(200);
}

int main(void)
{
  PortInit();

  while(1)
  {
    FlashLED();
  }

  return 0;
}