/* LCD and Pushbutton Counter
NOTE: This project was dificult to get to build. 

*/


#include <avr/io.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

#include <Arduino.h>


int main (void )
{
	int count;

	lcd_init();
	lcd_clear();
	lcd_home();
	lcd_printf("HELLO WORLD");

	count = 0;
	while(1)
	{
		lcd_goto_xy(0,1);
		lcd_printf("%d",count);
		delay_ms(1000);
		count++;

	}
}