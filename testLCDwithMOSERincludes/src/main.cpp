/*   !!!! THIS PROJECT RUNS THE SAME CODE AS LCD16by2ParallelCom4bitMode.c BUT JUST HAS THE FILES INCLUDED WITH MY OWN BUILD LIBRARY NAMED MOSER !!!!

  This project is to get a LCD 16 by 2 working in parallel communication
  This will use the 4bit mode where we send half of a 8bit command, wait a 
  specific amount of time then send the other half of the 4 bits

  Note: This project is trying to replicate the original MSOE lcd.c Library
  that was used in all the EE2920 class labs.

  Interface the LCD panel 16x2 to the Arduino Uno
  Goal: To print out Hello World on the LCD screen

  LCD 16x2 Wiring:
  RS to PB5, set to a 0 for initalization commands and set to 1 when sending data we want to display
  R/w to GND, as we are just doing writes
  E to PB4, Enable bit: tells lcd when data is comming when it is set to 1 
  DB 4:7 to PD4:7
  Vss/GND to GND
  Vee/CV to 1k potentiometer wired to 5v, GND (LCD contrast)
  Vdd/VCC to 5v
  LCD Backlight (optional depending on display): LED+ through resistor (Used 1k but anything is fine) to 5v and LED- to GND

  Order of Operations:
  1. Set RS bit (PB5 pin) (low for lcd commands and high for data)
  2. Raise Enable bit to 1 (PB4)
  3. Set Data bits to the first upper 4bit half of the 8bit command (PD4, PD5, PD6, PD7 pins)
  4. Lower Enable bit to 0 (PB4 pin)
  5. Wait at least minimum time for lcd to recieve the data (used 2ms)
  6. Repeat process 2 through 5 to send the last lower 4bit half of the 8bit command (PD4, PD5, PD6, PD7 pins)
  7. Repeat the entire process to send a new command/data

  8bit operation would just send all the Data at once but because we are using 4bit, we have to send in two sections

  Youtube video great for explaining how the display works https://www.youtube.com/watch?v=cXpeTxC3_A4

  A wiring diagram is included in the file structure.
*/

#include <avr/io.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>

#include <MOSER/lcd.c> //My own build library based on the MSOE lcd library

#include <Arduino.h>

int main (void)
{
	int count;

	//MOSER Functions
	LcdInit();
	LcdClear();
	LcdHome();
	LcdPrintf("Hello World");

	count = 0;
	while(1)
	{
		LcdGoToXY(0,1);
		LcdPrintf("%d",count);
		delay_ms(1000);
		count++;
	}
}