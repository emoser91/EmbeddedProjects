/* This project is a review of the Embedded lab #6 Analog Sensor.

  !!! This is just an example of how to set up the Photoresistor Sensor !!!

  Ignore "Red Problems" as I tried to build without MSOE Libary the first time

  Sample an analog voltage using the ATmega328p analog-to-digial subsystem.

  Requirments are as follows: 
  *Interface the LCD panel to the micro
  *Interface Photoresistor Sensor
  *Use 4 LEDs to create a light meter
   Light Level         |   LED Status
    Bright                  All 4 LEDs On
    Somewhat Bright         Three LEDs On
    Somewhat Dim            Two LEDs On
    Dim                     One LED On
    Completely Dark         All LEDs Off

  *Your program must use the ADC interrupt and corresponding ISR
  *Use Atmega328p Datasheet for ADC Subsystem Register Setup

  Resister Setup
  ADMUX
  (REFS1)(REFS)(ADLAR)(-)(MUX3)(MUX2)(MUX1)(MUX0)
     0      1     0    0    0     0     0     0

   ADCSRA
   (ADEN)(ADSC)(ADATE)(ADIF)(ADIE)(ADPS2)(ADPS1)(ADPS0)
      1     0      0     0    1      1      1      1

  Inputs:
  *PC0 (Photoresistor): 
  A 3.3k ohm resistor is wired from 5V to the Photoresistor and the other leg of the photoresistor is grounded.
  The sensor data is taken from the middle between the 3.3k ohm resistor and the photoresistor.
  See Wiring Diagram for futher clarification.
  
  Outputs:
  *PC2 (LED1), PC3 (LED2), PC4 (LED3), PC5 (LED4): All LEDs run through their own 300ohm resistor then to ground.

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

  DDRC |= (1<<PORTC2) | (1<<PORTC3) | (1<<PORTC4) | (1<<PORTC5); //4 LEDs

	ADMUX |= (1<<REFS0); //Selecting Voltage Reference of AVcc = 5v
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //ADEN = ADC Enable, ADIE = ADC Interrupt Enable, ADPS2:ADPS0 = Prescalar
	PORTC &= (~(1<<PORTC0)); //turning off pull-up resistor

  sei(); //turning on global interrupts

	ADCSRA |= (1<<ADSC); // start a ADC conversion

  while(1)
  {
    lcd_home();
		lcd_printf("LIGHT ADCW = %d",result);//printing ADCW as a decimal number

    //This is the easy way to do it. 
    //You could put this in an else statment at the end and then add the LED Off commands to each if statement
    PORTC &= (~(1<<PORTC2)) & (~(1<<PORTC3)) & (~(1<<PORTC4)) & (~(1<<PORTC5));//leds off=completely dark

		if(result < 200)//bright
		{
			PORTC |= (1<<PORTC2) | (1<<PORTC3) | (1<<PORTC4) | (1<<PORTC5);
		}
		else if(result < 250)//somewhat bright
		{
			PORTC |= (1<<PORTC2) | (1<<PORTC3) | (1<<PORTC4);
		}
		else if(result < 400)//somewhat dim
		{
			PORTC |= (1<<PORTC2) | (1<<PORTC3);
		}
		else if(result < 650)//dark
		{
			PORTC |= (1<<PORTC2);
		}
  }

  return 0;
}

ISR( ADC_vect )//ADC interrupt for when a conversion stops
{
  result = ADCW;//setting ADC result to a global varriable
  ADCSRA |= (1<<ADSC);//starting a new ACD conversion
}