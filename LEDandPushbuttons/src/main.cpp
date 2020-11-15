/* This project is a review of the Embedded lab #2 Bouncing LEDS 
  Interface pushbuttons and LEDs to the microcontroller Arduino Uno to control 
  the LED illumination in a manner which minics the Knight Rider pattern.

  Requirments are as follows: 
  *Interface 5 LEDs to digital output ports. Each LED must have a 300ohm resistor
  *Begin with the left most LED lit
  *Move the illuminated LED to the right one position and turn off the previous LED
  *Continue the pattern in the opposite direction once you reach the end
  *Repeat the pattern forever
  *Interface two push buttons to digital inputs
  *Use internal pullup resistors for the digital inputs
  *If push button 1 is pressed, the movement stops
  *If push button 2 is pressed, the LED movement rate increases
  *The push buttons must be read within a funciton
  *If both push buttons are pressed at the same time then reset and restart the sequence at the original speed

  Inputs:
  *PC0 (button1), PC1(button2)
  
  Outputs:
  *PB0 (LED1), PB1 (LED2), PB2 (LED3), PB3 (LED4), PB4 (LED5)

  Author: Eric Moser 11/2020

*/

#include <Arduino.h>

#define CYCLES_PER_US 16				// cpu cycles per microsecond
#define PUSHBUTTON_1 3
#define PUSHBUTTON_2 2
#define PUSHBUTTON_1_AND_2 1

int main (void)
{
  // Set all of the output pins for the LEDs DDRB = 0b00011111
  DDRB |= (1<<PORTB0) | (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4);

  // Set up digital inputs
  DDRC &= (~(1<<PORTC0)) & (~(1<<PORTC1));
  PORTC |= (1<<PORTC0) | (1<<PORTC1); //setting pull up resistors

	uint8_t delayValue = 100;
	int buttonScan;

  // Function prototypes
	int pushbuttonScan(void);
  void delay_ms(uint16_t ms);
  void delay_us(uint16_t us);

  void shiftRight(int ledposition, uint8_t delayValue);
  void shiftLeft(int ledposition, uint8_t delayValue);

  while(1)
  {
    for(int i = 0; i <= 7; i++)
    {
      // Checking status of buttons
      buttonScan = pushbuttonScan();

      while(buttonScan == PUSHBUTTON_1_AND_2)
      {
        PORTB = 0b00011111;
        delayValue = 100;
        i = 0;
        delay_ms(delayValue);
        buttonScan = pushbuttonScan();
      }

      while(buttonScan == PUSHBUTTON_1)
      {
        // Wait until the button is no longer pressed
        buttonScan = pushbuttonScan();
      }

      if(buttonScan == PUSHBUTTON_2)
      {
        // Reset the delay value if it reaches max speed
        // Assumes press and hold of button for visable changes in speed
        if(delayValue == 0)
        {
          delayValue = 100;
        }
        else
        {
          // Reduce the delay thus speeding up the pattern
          delayValue = delayValue - 1;
        }
      }

      // Running LED output
      if(i <= 4)
      {
        shiftRight(i, delayValue);
      }
      else if (i <= 7)
      {
        shiftLeft(i, delayValue);
      }

    }

  }

  return 0;
}

// Functions 

void shiftRight(int ledposition, uint8_t delayValue)
{
  PORTB = (0b00000001 << ledposition);
  delay_ms(delayValue);
}

void shiftLeft(int ledposition, uint8_t delayValue)
{
  //Assumes that ledposition is at the 5th position
  PORTB = (0b00010000 >> (ledposition - 4));
  delay_ms(delayValue);
}

int pushbuttonScan(void)
{
	int buttonPressStatus = 0;

  //Bit Masking inputs for each button
  //Note that if a button is not pressed it reads high because of the pullup resistors
  uint8_t button1and2Check = PINC & 0b00000011;
  uint8_t button1Check = PINC & 0b00000001;
  uint8_t button2Check = PINC & 0b00000010;
  
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

	return buttonPressStatus;

}

// delay for us micro-seconds 
// delay times are only half-way accurate if optimization is turned on to level 3!
// max value for us is 65535/4*CYCLES_PER_US
// which is app. 17777 for 14.7456MHZ
void delay_us(uint16_t us)
{
	uint16_t _count;
	_count=us/4*CYCLES_PER_US;
	
	asm volatile (
		"1: sbiw %0,1" "\n\t"
		"brne 1b"
		: "=w" (_count)
		: "0" (_count)
	);
}

void delay_ms(uint16_t ms) 
{
	volatile uint16_t i;

	for(i = ms; i > 0; i--)
	{
		delay_us(1000);
	}
} 
