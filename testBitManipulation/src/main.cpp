/*
*testBitManipulation.c
*Demostrates easy manipulation of setting, clearing and toggling bits
*
*Demostation is that of the setup for Lab2LEDandPushbuttons.c where there are 5 output LEDs and two pushbutton inputs
*You can use this in any code by simply adding the MSOE Libary or just the bit.c file from the library
*
*Modified MSOE supplied code from bit.c in MSOE Library 
*/

#include <Arduino.h>

#include <avr/io.h>
#include <inttypes.h>

#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))
#define tbi(a, b)  (((a) & (1<<(b)))>>(b))

#define bit_set(a,m)    ((a) |= (m))
#define bit_clear(a,m)  ((a) &= ~(m))
#define bit_toggle(a,m) ((a) ^= (m))
#define bit_test(a, m)  ((a) & (m))

int main(void)
{
  //Setting up outputs to LEDs
  sbi(DDRB, PORTB0);
  sbi(DDRB, PORTB1);
  sbi(DDRB, PORTB2);
  sbi(DDRB, PORTB3);
  sbi(DDRB, PORTB4);

  //Setting up inputs for buttons
  cbi(DDRC, PORTC0);
  cbi(DDRC, PORTC1);
  //Turning on internal pullup resistors
  sbi(PORTC, PORTC0);
  sbi(PORTC, PORTC1);

  return 0;
}