/* This project is a review of the Embedded lab #7 Waveform Generation using the Timer/Counter Subsystem.

  !!! This is the first part of the lab to generate a CTC Waveform !!!

  PART 1: Square-wave Generator
  Requirments are as follows:
  *For this part of the lab, you will write a program that generates and outputs a square wave of a desired frequency.
  *The frequency will be selectable via two pushbuttons.
  *The pushbutton settings and corresponding frequencies are:
   Pushbutton 1    |   Pushbutton 2    |   Frequency
     Not Pressed         Not Pressed         262.63 Hz
     Not Pressed         Pressed             329.63 Hz
     Pressed             Not Pressed         392.00 Hz
     Pressed             Pressed             523.25 Hz
  *Use Atmega328p Datasheet for Timer/Counter Register Setup
  *These frequencies represent a C Major chord, with the lowest frequency being that of middle C
  *Use Timer/Counter 0 in CTC mode for generating the waveform
  *Match the frequency as close as possible
  *Feel free to connect a speaker to the output to hear the notes if you wish
  *Use an Oscilloscope to confirm waveforms

  Resister Setup
  TCCR0A
  (COM0A1)(COM0A0)(COM0B1)(COM0B0)(-)(-)(WGM01)(WGM00)
     0        1       0      0     0  0    1      0

  TCCR0B
  (FOC0A)(FOC0B)(-)(-)(WGM02)(CS02)(CS01)(CS00)
     0     0     0  0    1      1     0     0

  Wiring
  Inputs:
  *PC0 (button1), PC1(button2): Both buttons are then wired to ground

  Outputs:
  *PD6/PWM/OC0A: Waveform output

*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

#define PUSHBUTTON_1 3
#define PUSHBUTTON_2 2
#define PUSHBUTTON_1_AND_2 1
#define PUSHBUTTON_RESET 4

/* Simple Solution to the Problem */
// int main (void)
// {
//   // Set up digital inputs
//   DDRC &= (~(1<<PORTC0)) & (~(1<<PORTC1));
//   PORTC |= (1<<PORTC0) | (1<<PORTC1); //Setting pull up resistors
//   DDRD|=(1<<PORTD6); //Wave output for OC0A

// 	int buttonScan;

//   //CTC Wave Resister Setup
// 	TCCR0A=(1<<COM0A0)|(1<<WGM01); //Toggle on compare match and ctc mode
//   TCCR0B=(1<<CS02); //256 prescaler

//   while(1)
//   {
//     buttonScan = PINC & (0b00000011); //Checking status of buttons

//     if(buttonScan == 0b00000001) //Button 1 Pressed
//     {
//       OCR0A = 94; //392.00Hz
//     }

//     if(buttonScan == 0b00000010) //Button 2 Pressed
//     {
//       OCR0A = 79; //329.63Hz
//     }

//     if(buttonScan == 0b00000000) //Button 1 and 2 Pressed
//     {
//       OCR0A = 59; //523.25Hz
//     }

//     if(buttonScan == 0b00000011) //No Buttons Pressed
//     {
//       OCR0A = 118; //261.63Hz
//     }
//   }

//   return 0;
// }


/* More Complicated Solution */
int main (void)
{
  // Set up digital inputs
  DDRC &= (~(1<<PORTC0)) & (~(1<<PORTC1));
  PORTC |= (1<<PORTC0) | (1<<PORTC1); //Setting pull up resistors
  DDRD|=(1<<PORTD6); //Wave output for OC0A

  int button1resetflag = 0;
  int button2resetflag = 0;
	int buttonScan;

  // Function prototypes
	int pushbuttonScan(void);

  //CTC Wave Resister Setup
	TCCR0A=(1<<COM0A0)|(1<<WGM01); //Toggle on compare match and ctc mode
  TCCR0B=(1<<CS02); //256 prescaler

  while(1)
  {
    //The flagging system was over engineering, but it makes it so statement is only ran once with a button push
    //Holding the button will not continue to execute the statement just as in Lab3 Code. 

    buttonScan = pushbuttonScan(); //Checking status of buttons

    if((buttonScan == PUSHBUTTON_1) & (!button1resetflag)) //Button 1 Pressed
    {
      OCR0A = 94; //392.00Hz
      button1resetflag = 1;
    }

    if((buttonScan == PUSHBUTTON_2) & (!button2resetflag)) //Button 2 Pressed
    {
      OCR0A = 79; //329.63Hz
      button2resetflag = 1;
    }

    if((buttonScan == PUSHBUTTON_1_AND_2) & (!button1resetflag) & (!button2resetflag)) //Button 1 and 2 Pressed
    {
      OCR0A = 59; //523.25Hz
      button1resetflag = 1;
      button2resetflag = 1;
    }

    if(buttonScan == PUSHBUTTON_RESET)//No Buttons Pressed
    {
      OCR0A = 118; //261.63Hz
      //Could have individual resets instead but it shouldnt matter
      button1resetflag = 0;
      button2resetflag = 0;
    }
  }

  return 0;
}

int pushbuttonScan(void)
{
  int buttonPressStatus = 0;

  //Bit Masking inputs for each button
  //Note that if a button is not pressed it reads high because of the pullup resistors
  uint8_t button1and2Check = PINC & 0b00000011;
  uint8_t button1Check = PINC & 0b00000001;
  uint8_t button2Check = PINC & 0b00000010;
  
  //Delay solves issue that you have to exactly press the two at the same time or it will go to one of the single button options
  delay_ms(200);

  if(button1and2Check == 0b00000000)
  {
    buttonPressStatus = PUSHBUTTON_1_AND_2;
  }

  else if(button1Check == 0b00000000)
  {
    buttonPressStatus = PUSHBUTTON_1;
  }

  else if(button2Check == 0b00000000)
  {
    buttonPressStatus = PUSHBUTTON_2;
  }

  else if(button1and2Check == 0b00000011)
  {
    buttonPressStatus = PUSHBUTTON_RESET;
    delay_ms(50); //For Debouncing of button
  }

  return buttonPressStatus;

}