/* This project is a review of the Embedded lab #8 Design and Simulate the steering wheel in a drive by wire vehicle.

  Project uses two Timer/Counters and one ADC
  
  *A potentiometer will be used to simulate the user input from a steering wheel.
  *The analog voltage at the output of the potentiometer circuit must be in the range 0 to 5v.
  *A servo motor will be used to simulate the position of the front tires on an automobile.
  *The range of rotation will be between +/- 45 degrees.
  *Analog voltages from the potiometer must be mapped to the servo motor as shown in the plot bellow (See Instructions image in code folder)
  *Note that the plot is used to generate a formula y = 1.5X + 2240
  *ADC must sample the analog input voltage at the rate of once every 10ms = 0.01s. This is done with a CTC Timer/Counter setup to Toc = 0.01 with an interrupt
  *Use Atmega328p Datasheet for Timer/Counter Register Setup
  *Use an Oscilloscope to confirm waveforms

  *!!!SEE FILES LABELED "MATH" IN ORDER TO UNDERSTAND THE SETTINGS FOR THE TIMER/COUNTER RESISTERS!!!
  *!!!IGNORE THE MATH IN OTHER FILES AS IT IS MISLEADING OR WRONG!!!

  *A wiring diagram is included in the file structure.

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

  ADC Setup:
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
  Right Leg of Pot: to 5v (No resistor like lab5 had)
  Middle Leg of Pot: ADCO/A0 Pin of Micro

  CTC Timer/Counter Register Setup:
  Resister Setup
  TCCR2A
  (COM2A1)(COM2A0)(COM2B1)(COM2B0)(-)(-)(WGM21)(WGM20)
     0        1       0      0     0  0    1      0

  TCCR2B
  (FOC2A)(FOC2B)(-)(-)(WGM22)(CS22)(CS21)(CS20)
     0     0     0  0    1      1     1     1

  Outputs:
  *digital 11/OC2A output for 10ms: Waveform output

  Fast PWM Timer/Counter Register Setup
  TCCR1A
  (COM1A1)(COM1A0)(COM1B1)(COM1B0)(-)(-)(WGM11)(WGM10)
     0        0       1      0     0  0    1      1

  TCCR1B
  (ICnC1)(ICES1)(-)(WGM13)(WGM12)(CS12)(CS11)(CS10)
     0     0     0     1     1      0     1     0

  Outputs:
  *digital 10 PWM/OC1B output to servo: Waveform output
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

//GLOBAL VARRIBLES
uint16_t result;//Variable to store my ADCW

int main (void )
{
	//LCD SET UP AND VARRIBLES
	lcd_init(); lcd_clear(); lcd_home();//LCD enable
	DDRB |= (1<<PORTB2);//digital 10 PWM/OC1B output to servo
	DDRB |= (1<<PORTB3);//digital 11/OC2A output for 10ms
	DDRC &= (~(1<<PORTC0)); //input for potentiometer
	PORTC &= (~(1<<PORTC0));//turning off pull-up resistor

	//SETTING UP THE ADC
	ADMUX |= (1<<REFS0);///Selecting Voltage Reference of AVcc = 5v and port C0 for input
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //ADEN = ADC Enable, ADIE = ADC Interrupt Enable, ADPS2:ADPS0 = Prescalar=128

	//SETTING UP T/C 2
	TCCR2A |= (1<<WGM21) | (1<<COM2A0);//CTC mode on TC 2 and toggle on compare
	TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);//Prescale set to 1024
	OCR2A = 155; //top value=50Hz giving the wanted Toc (time on compare) = 0.01s = 10ms (Max value is 8 bits or 255)
	TIMSK2 |= (1<<OCIE2B);//Timer/Counter2 Interrupt Mask Register set to Compare Match A interrupt enable ISR(TIMER2_COMPA_vect)

	//SETTING UP T/C 1
	TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);//fast pwm mode top=OC1A and clear OC1B on compare match set OC1B at Bottom
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<WGM13);//Prescale set to 8
	OCR1A = 39999;//top=50 Hz (Max value is 16 bits or 65535)
	TIMSK1 |= (1<<OCIE1A);//Timer/Counter2 Interrupt Mask Register set to Compare Match A interrupt enable ISR(TIMER1_COMPA_vect)

	//STARTING GLOBAL INTURRUPTS
  //Dont have to start first ADC conversion because it will happen in Timer/Counter Interrupt every 10ms
	sei();

	while(1)
	{
    lcd_home();
		lcd_printf("ADCW = %d",result);//printing ADCW as a decimal number
		lcd_goto_xy(0,1);//move down one row in LCD
		lcd_printf("OCR1B = %d",OCR1B);//printing voltage
	}

  return 0;
}

ISR(ADC_vect)//ADC interrupt for when a conversion stops
{
  result = ADCW;//writing ADC result to a global
}

ISR(TIMER2_COMPB_vect) //Called every 0.01s = 10ms
{
  ADCSRA |= (1<<ADSC);//starting a new ADC conversion
}

ISR(TIMER1_COMPA_vect)
{
  //Based on equation for servo shown in notes
  OCR1B = (1.564*(float)result)+2240;//changing duty cycle
}