/* This project is a review of the Embedded lab #5 Introduction to Analog-to-Digital Conversion

  Sample an analog voltage using the ATmega328p analog-to-digial subsystem. 

  Requirments are as follows: 
  *Interface the LCD panel to the micro
  *The ADC input will be the voltage from a voltage divider circuit that uses a fixed 1k ohm resistor
   and a potentiometer (ie a varriable resistor)
  *Your program must display the ADC result on the LCD as follows:
      ADCW = 561
      V = 1.39820E0
  *Note that float type casting is being used in the voltage value that is being displayed
  *Your program must use the ADC interrupt and corresponding ISR
  *Use Atmega328p Datasheet for ADC Subsystem Register Setup

  Resister Setup
  ADMUX
  (REFS1)(REFS)(ADLAR)(-)(MUX3)(MUX2)(MUX1)(MUX0)
     0      1     0    0    0     0     0     0

   ADCSRA
   (ADEN)(ADSC)(ADATE)(ADIF)(ADIE)(ADPS2)(ADPS1)(ADPS0)
      1     0      0     0    1      1      1      1

  Potentiometer wiring
  Input:
  Left Leg of Pot: GND
  Right Leg of Pot: through 1k ohm resistor to 5v
  Middle Leg of Pot: ADCO/A0 Pin of Micro

  LCD Input/Output is the same as in Lab3

  Inputs:
  *PC0 (button1), PC1(button2): Both buttons are then wired to ground

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

  A wiring diagram is included in the file structure.
*/

#include <avr/io.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

#include <Arduino.h>

//Globlal Varriables
uint16_t result; //Variable to store my ADCW

int main (void)
{
  //Initialize LCD Screen
	lcd_init();
	lcd_clear();
	lcd_home();

	float volts;

	ADMUX |= (1<<REFS0); //Selecting Voltage Reference of AVcc = 5v
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //ADEN = ADC Enable, ADIE = ADC Interrupt Enable, ADPS2:ADPS0 = Prescalar
	PORTC &= (~(1<<PORTC0)); //turning off pull-up resistor

  sei(); //turning on global interrupts

	ADCSRA |= (1<<ADSC); // start a ADC conversion

  while(1)
  {
    lcd_home();
		lcd_printf("ADCW = %d",result); //printing ADCW as a decimal number

    //Have to use (float) to type cast the uint16_t value to float so the calculation is done in floating point math
		volts = (float)result*5/1023; //calculating voltage

		lcd_goto_xy(0,1);
		lcd_printf("v = ");
		lcd_goto_xy(4,1);
		lcd_print_float(volts); //printing voltage
  }

  return 0;
}

ISR( ADC_vect )//ADC interrupt for when a conversion stops
{
  result = ADCW;//setting ADC result to a global varriable
  ADCSRA |= (1<<ADSC);//starting a new ACD conversion
}