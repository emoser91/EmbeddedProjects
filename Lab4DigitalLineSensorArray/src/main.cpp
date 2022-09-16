/* This project is a review of the Embedded lab #4 Digital Line Sensor Array

  Interface the digital line sensor array QTR-8RC and write software that displays
  the line sensor output on the lcd panel. 

  Requirments are as follows: 
  *Interface the LCD panel to the micro
  *Interface the ATR-8RC Digital Line Sensor Array to the micro
  *Your Software must sequentially read from 4 of the sensors and display the binary value of each sensor
   on the LCD.
  *For example, if sensors 1 and 2 are seeing a white surface and sensor 3 and 4 are seeing a dark surface, the
   LCD should display 0011
  *If the IR light from the sensor bounces back it will read a 0 for a white surface
  *If the IR light is absorbed on the surface then the light will not bounce back and it will read a 1 in the case for a black surface
  *If the sensor is too far from a surface, it will just read all 1s
  *There is a diagram explaining the physics in the file structure
  *Make sure to test with a piece of black electrical tape on a white piece of paper

  *Sensor works by sending a 5v output to the sensor for 10us then switching to a input, waiting 1ms then reading the line.

  Line Sensor wiring
  Input/Output:
  Sensor GND: GND
  Sensor 5v: 5v
  Sensor 1: PB0/Digital14 (Note that the original lab I used PC0)
  Sensor 2: PB1/Digital15
  Sensor 3: PB2/Digital16
  Sensor 4: PB3/Digital17

  LCD Output is the same as in Lab3

  Outputs: 
  LCD 16x2 Wiring:
	rs-PB5
  R/w-GND
  E-PB4
  DB 4:7-PD4:7
  Vss-GND
  Vee-pot-(5v,GND)
  Vdd-5v

  A wiring diagram is included in the file structure.
*/

#include <avr/io.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

#include <Arduino.h>

int main (void)
{
  //Initialize LCD Screen
	lcd_init();
	lcd_clear();
	lcd_home();

  //Initial LCD Print
  lcd_printf("Line Sensor");

  while(1)
  {
    //Sensor 1
    DDRB  |= (1<<PORTB0);          //set pin for output
    PORTB |= (1<<PORTB0);        //write a 1 to pin
    delay_us(10);                       //delay
    DDRB &= (~(1<<PORTB0));       //set pin to input
    delay_ms(1);                        //delay
    bool sensor1data = PINB & (1 << 0); //mask and read input
    lcd_goto_xy(0,1);
    lcd_printf("%d",sensor1data);

    //Sensor 2
    DDRB |= (1<<PORTB1);
    PORTB |= (1<<PORTB1);
    delay_us(10);
    DDRB &= (~(1<<PORTB1));
    delay_ms(1);
    bool sensor2data = PINB & (1 << 1); //force to bool otherwise value is 0010 = 2
    lcd_goto_xy(1,1);
    lcd_printf("%d",sensor2data);

    //Sensor 3
    DDRB |= (1<<PORTB2);
    PORTB |= (1<<PORTB2);
    delay_us(10);
    DDRB &= (~(1<<PORTB2));
    delay_ms(1);
    bool sensor3data = PINB & (1 << 2);
    lcd_goto_xy(2,1);
    lcd_printf("%d",sensor3data);

    // //Sensor 4
    DDRB |= (1<<PORTB3);
    PORTB |= (1<<PORTB3);
    delay_us(10);
    DDRB &= (~(1<<PORTB3));
    delay_ms(1);
    bool sensor4data = PINB & (1 << 3);
    lcd_goto_xy(3,1);
    lcd_printf("%d",sensor4data);
  }

  return 0;
}

