/* LCD and Pushbutton Counter
NOTE: This project was dificult to get to build. 
I added the MSOE folder to the include folder with just copy and paste. 
I then refreshed the directory and cleaned the build then built
I had to close and reopen the program for it to finally be happy. 

It wouldnt accept the include files for the longest time as well as
programmed garbage to the LCD until it finally started working. 
*/

#include <avr/io.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

#include <Arduino.h>


int main (void)
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