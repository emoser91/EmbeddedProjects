/* This project is a review of the Embedded lab #3 LCD and Pushbutton Counter
  Note: This progject wasnt linking the header files for the longest time
  I finally got them to all link and not really sure what the issue was

  Interface the LCD panel 16x2 and a pair of pushbuttons to the Arduino Uno
  and display the number of button pressses on the LCD

  Requirments are as follows: 
  *Interface the LCD panel to the micro
  *Interface a pair of pushbuttons to the micro inputs
  *When button 1 is pressed, the count on the LCD increases by exactly 1
  *When button 2 is pressed, the count on the LCD decreases by exactly 1
  *Should display for example "count = 21"
  *The count should increment/decrement only once per press. Doesnt continue when held down
  *The count can be either positive or negative

  Inputs:
  *PC0 (button1), PC1(button2)

  Outputs: 
  LCD 16x2 Wiring:
	rs-PB5
    R/w-GND
    E-PB4
    DB 4:7-PD4:7
    Vss-GND
    Vee-pot-(5v,GND)
    Vdd-5v
    PC0-button1
    PC1-button2

*/


#include <avr/io.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

#include <Arduino.h>


int main (void )
{
	// int count;

	lcd_init();
	// lcd_print_int8(1);
	lcd_clear();
	lcd_home();
	lcd_printf("HELLO WORLD");

	// count = 0;
	// while(1)
	// {
	// 	lcd_goto_xy(0,1);
	// 	lcd_printf("%d",count);
	// 	delay_ms(1000);
	// 	count++;

	// }
}