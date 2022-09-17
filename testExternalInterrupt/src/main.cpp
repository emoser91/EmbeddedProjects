/*
*testExternalInterrupt.c
*Demostrates and tests use of External Interrupts on the ATmega328p.
*Implements a simple counter. Each time an external pushbutton is pressed,
*  the count value is incremented. Current count is displayed on LCD panel.
*
*Pushbutton connected to INT0 (PD2). Must use hardware debouncing.
*
*MSOE supplied code
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

int8_t count;

int main(void)
{
  DDRD &= ~(1<<PORTD2); //Pushbutton Input
  PORTD |= (1<<PORTD2); //Pullup on

  lcd_init;
  lcd_home;
  lcd_clear;

  EIMSK = (1<<INT0); //External Interrupt Mask Register: enable for INT0 = Pin PD2
  EICRA = (1<<ISC01); //External Interrupt Control Register: falling edge trigger generates a interrupt request

  count = 0;
  sei(); //Global interrupts on

  while(1)
  {
    lcd_home;
    lcd_printf("%d",count);
    delay_ms(100);
  }

  return 0;
}

//External Interrupt for INT0
ISR(INT0_vect)
{
  ++count;
}