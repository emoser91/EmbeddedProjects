/* This project is a review of the Embedded lab #6 Analog Sensor.

  !!! This is just an example of how to set up the Distance Sensor !!!

  Sample an analog voltage using the ATmega328p analog-to-digial subsystem.

  Requirments are as follows: 
  *Interface the LCD panel to the micro
  *Interface Sharp Distance Sensor (Large Sensor used in Robot to detect objects)
  *Use an LED to indicate the distance to an object
    Distance to Object (cm)  |   LED Status
          >70                      Off
          50 to 70                 Slow Blink
          30 to 50                 Medium Blink
          20 to 30                 Fast Blink
          20 to 10                 On
  *Your program must use the ADC interrupt and corresponding ISR
  *Use Atmega328p Datasheet for ADC Subsystem Register Setup

  Resister Setup
  ADMUX
  (REFS1)(REFS)(ADLAR)(-)(MUX3)(MUX2)(MUX1)(MUX0)
     0      1     0    0    0     0     0     0

   ADCSRA
   (ADEN)(ADSC)(ADATE)(ADIF)(ADIE)(ADPS2)(ADPS1)(ADPS0)
      1     0      0     0    1      1      1      1

  Distance Sensor wiring
  Input:
  Left Red Wire: 5v
  Middle Black Wire: GND
  Right White Wire (Sensor Output): ADCO/A0 Pin of Micro

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
  Small Modifications to wiring diagram in Lab6.
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

//Globlal Varriables
uint16_t result; //Variable to store my ADCW

int main (void)
{
  //Initialize LCD Screen
	lcd_init();
	lcd_clear();
	lcd_home();

  DDRD = DDRD|(1<<PORTD2);//led blue


	ADMUX |= (1<<REFS0); //Selecting Voltage Reference of AVcc = 5v
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //ADEN = ADC Enable, ADIE = ADC Interrupt Enable, ADPS2:ADPS0 = Prescalar
	PORTC &= (~(1<<PORTC0)); //turning off pull-up resistor

  sei(); //turning on global interrupts

	ADCSRA |= (1<<ADSC); // start a ADC conversion

  while(1)
  {
    lcd_home();
		lcd_printf("ADCW = %d",result); //printing ADCW as a decimal number

    //Picked ranges for good visuals from LED, did not measure distances out
    //Sensor did not give reliable value with nothing in front of it so the first case rarly happened
    //Note that the sensor used was not the original sensor used in the lab experiment. Lab sensor was better.
    if(result < 250)//>70
    {
      PORTD &= (~(1<<PORTD2));
    }
    if( (result > 250) && (result < 350) )//50to70
    {
      PORTD |= (1<<PORTD2);
      delay_ms(500);
      PORTD &= (~(1<<PORTD2));
      delay_ms(500);
    }
    if( (result > 350) && (result <450) )//30to50
    {
      PORTD |= (1<<PORTD2);
      delay_ms(250);
      PORTD &= (~(1<<PORTD2));
      delay_ms(250);
    }
    if( (result >450) && (result < 600) )//20to30
    {
      PORTD |= (1<<PORTD2);
      delay_ms(50);
      PORTD &= (~(1<<PORTD2));
      delay_ms(50);
    }
    if(result > 600)//10to20
    {
      PORTD |= (1<<PORTD2);
    }
  }

  return 0;
}

ISR( ADC_vect )//ADC interrupt for when a conversion stops
{
  result = ADCW;//setting ADC result to a global varriable
  ADCSRA |= (1<<ADSC);//starting a new ACD conversion
}