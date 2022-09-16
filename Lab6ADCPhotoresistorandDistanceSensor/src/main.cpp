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
  MUX0 is set to 0 for the ADCO/A0 Pin of Micro for the Distance Sensor
  MUX0 is changed to a 1 for the ADC1/A1 Pin of Micro for the Photoresistor

   ADCSRA
   (ADEN)(ADSC)(ADATE)(ADIF)(ADIE)(ADPS2)(ADPS1)(ADPS0)
      1     0      0     0    1      1      1      1

  Distance Sensor wiring
  Input:
  Left Red Wire: 5v
  Middle Black Wire: GND
  Right White Wire (Sensor Output): ADCO/A0 Pin of Micro

  Photoresistor Wiring
  Inputs:
  *ADC1/A1/PC1 Pin of Micro (Photoresistor): 
  A 3.3k ohm resistor is wired from 5V to the Photoresistor and the other leg of the photoresistor is grounded.
  The sensor data is taken from the middle between the 3.3k ohm resistor and the photoresistor.
  See Wiring Diagram for futher clarification.
  
  Outputs:
  *PC2 (LED1), PC3 (LED2), PC4 (LED3), PC5 (LED4): All LEDs run through their own 300ohm resistor then to ground.

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
  Small Modifications to wiring diagram in Lab6.
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

//Globlal Varriables
uint16_t resultDistance; //Variable to store Distance sensor ADCW result
uint16_t resultPhotoresistor; //Variable to store Photoresistor ADCW result

#define DISTANCESENSOR 0
#define PHOTORESISTOR 1

uint8_t caseFlag = DISTANCESENSOR;

int main (void)
{
  //Initialize LCD Screen
	lcd_init();
	lcd_clear();
	lcd_home();

  DDRD = DDRD|(1<<PORTD2);//led blue for Distance Sensor
  DDRC |= (1<<PORTC2) | (1<<PORTC3) | (1<<PORTC4) | (1<<PORTC5); //4 LEDs for Photoresistor

	ADMUX |= (1<<REFS0); //Selecting Voltage Reference of AVcc = 5v
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //ADEN = ADC Enable, ADIE = ADC Interrupt Enable, ADPS2:ADPS0 = Prescalar
  //Note that MUX0 is default set to 0 so ADC on ADCO/A0/PC0 will happen first for the Distance sensor

	PORTC &= (~(1<<PORTC0)) & (~(1<<PORTC1)); //turning off pull-up resistors
  sei(); //turning on global interrupts

	ADCSRA |= (1<<ADSC); // start a ADC conversion

  while(1)
  {
    //I have noticed that if the Distance LED is blinking then there is long delays being called and the Photoresitor LEDs are not going to operate quickly.
    //The best way to test is if the Distance LED is in solid Off or On positions then testing the Photoresistor. 
    if(caseFlag == DISTANCESENSOR)
    {
      lcd_home();
      lcd_printf("DIST ADCW = %d",resultDistance); //printing ADCW as a decimal number

      //Picked ranges for good visuals from LED, did not measure distances out
      //Sensor did not give reliable value with nothing in front of it so the first case rarly happened
      //Note that the sensor used was not the original sensor used in the lab experiment. Lab sensor was better.
      if(resultDistance < 250)//>70
      {
        PORTD &= (~(1<<PORTD2));
      }
      if( (resultDistance > 250) && (resultDistance < 350) )//50to70
      {
        PORTD |= (1<<PORTD2);
        delay_ms(500);
        PORTD &= (~(1<<PORTD2));
        delay_ms(500);
      }
      if( (resultDistance > 350) && (resultDistance <450) )//30to50
      {
        PORTD |= (1<<PORTD2);
        delay_ms(250);
        PORTD &= (~(1<<PORTD2));
        delay_ms(250);
      }
      if( (resultDistance >450) && (resultDistance < 600) )//20to30
      {
        PORTD |= (1<<PORTD2);
        delay_ms(50);
        PORTD &= (~(1<<PORTD2));
        delay_ms(50);
      }
      if(resultDistance > 600)//10to20
      {
        PORTD |= (1<<PORTD2);
      }
    }

    else if(caseFlag == PHOTORESISTOR)
    {
      lcd_goto_xy(0,1);
      lcd_printf("LIGHT ADCW = %d",resultPhotoresistor);//printing ADCW as a decimal number

      //This is the easy way to do it. 
      //You could put this in an else statment at the end and then add the LED Off commands to each if statement
      PORTC &= (~(1<<PORTC2)) & (~(1<<PORTC3)) & (~(1<<PORTC4)) & (~(1<<PORTC5));//leds off=completely dark

      if(resultPhotoresistor < 200)//bright
      {
        PORTC |= (1<<PORTC2) | (1<<PORTC3) | (1<<PORTC4) | (1<<PORTC5);
      }
      else if(resultPhotoresistor < 250)//somewhat bright
      {
        PORTC |= (1<<PORTC2) | (1<<PORTC3) | (1<<PORTC4);
      }
      else if(resultPhotoresistor < 400)//somewhat dim
      {
        PORTC |= (1<<PORTC2) | (1<<PORTC3);
      }
      else if(resultPhotoresistor < 650)//dark
      {
        PORTC |= (1<<PORTC2);
      }
    }
		
  }

  return 0;
}

//There is only One ADC on the Arduino Uno so we have to switch between inputs
ISR( ADC_vect )//ADC interrupt for when a conversion stops
{
  if(caseFlag == DISTANCESENSOR)
  {
    resultDistance = ADCW;//setting ADC result to a global varriable

    //Switch to let the Photoresistor do a ADC
    ADMUX |= (1<<MUX0);//turn on ADC for ADC1/PC1
    caseFlag = PHOTORESISTOR;

    ADCSRA |= (1<<ADSC);//starting a new ACD conversion

  }
  else if (caseFlag == PHOTORESISTOR)
  {
    resultPhotoresistor = ADCW;//setting ADC result to a global varriable

    //Switch to let the Distance Sensor do a ADC
    ADMUX &= (~(1<<MUX0));//turn on ADC for ADC0/PC0
    caseFlag = DISTANCESENSOR;

    ADCSRA |= (1<<ADSC);//starting a new ACD conversion
  }
}