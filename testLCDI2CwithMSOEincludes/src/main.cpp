/* Test of I2C Backpack for 16by2 LCD using MSOE I2C Library

  LCD 16x2 with LCD Backpack Wiring:
  DAT(Data) to SDA/PC4/A4 Pin
  CLK(Clock) to SCL/PB5/A5 Pin
  5v to 5v Pin
  GND to GND Pin

*/

#include <Arduino.h>

#include <avr/io.h>
#include <inttypes.h>
#include "MSOE_I2C/delay.h"
#include "MSOE_I2C/bit.h"
#include "MSOE_I2C/lcd.h"

int main(void)
{
 	lcd_init();
	lcd_clear();
	lcd_home();
	lcd_printf("Hellooo World");

	int count = 0;
	while(1)
	{
		lcd_goto_xy(0,1);
		lcd_printf("%d",count);
		delay_ms(1000);
		count++;
	}

  return 0;
}