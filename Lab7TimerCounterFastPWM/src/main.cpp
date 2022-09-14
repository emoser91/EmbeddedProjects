/* This project is a review of the Embedded lab #7 Waveform Generation using the Timer/Counter Subsystem.

  !!! This is the Second part of the lab to generate a Fast PWM Waveform !!!

  PART 2: PWM Signaling
  Requirments are as follows:
  *For this part of the lab, you will write a program that generates and outputs a Pulse-Width Modulated (PWM) signal.
  *Use Timer/Counter 0 in Fast PWM mode for generating the waveforms.
  *The frequency of the PWM waveform must be 2k Hz.
  *Match the duty cycles closely as possible
  *The Duty Cycle of the waveform will be selectable via two pushbuttons.
  *The pushbutton settings and corresponding duty cycles are:
   Pushbutton 1    |   Pushbutton 2    |   Duty Cycle
     Not Pressed         Not Pressed         15%
     Not Pressed         Pressed             33%
     Pressed             Not Pressed         60%
     Pressed             Pressed             95%
  *Use Atmega328p Datasheet for Timer/Counter Register Setup
  *Use an Oscilloscope to confirm waveforms

  Resister Setup
  TCCR0A
  (COM0A1)(COM0A0)(COM0B1)(COM0B0)(-)(-)(WGM01)(WGM00)
     0        0       1      0     0  0    1      1

  TCCR0B
  (FOC0A)(FOC0B)(-)(-)(WGM02)(CS02)(CS01)(CS00)
     0     0     0  0    1      0     1     1

  Wiring
  Inputs:
  *PC0 (button1), PC1(button2): Both buttons are then wired to ground

  Outputs:
  *PD5/PWM/OC0B: Waveform output, !!Different Pin from Part 1 of Lab7!! 
  *Not sure if PD6/PWM/OC0A is usable because it is the top of our Fast PWM.
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

/* Simple Solution to the Problem */
int main (void)
{
  // Set up digital inputs
  DDRC &= (~(1<<PORTC0)) & (~(1<<PORTC1));
  PORTC |= (1<<PORTC0) | (1<<PORTC1); //Setting pull up resistors
  DDRD |= (1<<PORTD5); //Wave output

	int buttonScan;

  //Fast PWM Resister Setup
  TCCR0A = (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);//fast pwm mode top=OC0A and clear OC0B on compare match set OC0B at Bottom
	TCCR0B = (1<<CS01) | (1<<CS00) | (1<<WGM02);//prescale 64
	OCR0A = 124;//top = 2k Hz (Max value is 8 bits or 255)

  while(1)
  {
    buttonScan = PINC & (0b00000011); //Checking status of buttons

    if(buttonScan == 0b00000001) //Button 1 Pressed
    {
      OCR0B = 41; //60% Duty Cycle
    }

    if(buttonScan == 0b00000010) //Button 2 Pressed
    {
      OCR0B = 74; //33% Duty Cycle
    }

    if(buttonScan == 0b00000000) //Button 1 and 2 Pressed
    {
      OCR0B = 118; //95% Duty Cycle
    }

    if(buttonScan == 0b00000011) //No Buttons Pressed
    {
      OCR0B = 19; //15% Duty Cycle
    }
  }

  return 0;
}