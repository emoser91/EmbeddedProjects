/*
 * i2cportprogram.c
 *
 *  Created on: Nov 18, 2011
 *      Author: williams
 */

#include "MSOE_I2C/delay.h"
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
  // Loop Forever

  return 0;
}
