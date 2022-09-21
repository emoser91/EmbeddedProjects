/* This project is to get a LCD 16 by 2 working in parallel communication
  This will use the 4bit mode where we send half of a 8bit command, wait a 
  specific amount of time then send the other half of the 4 bits

  Note: This project is trying to replicate the original MSOE lcd.c Library
  that was used in all the EE2920 class labs.

  Interface the LCD panel 16x2 to the Arduino Uno
  Goal: To print out Hello World on the LCD screen

  LCD 16x2 Wiring:
  RS to PB5, set to a 0 for initalization commands and set to 1 when sending data we want to display
  R/w to GND, as we are just doing writes
  E to PB4, Enable bit: tells lcd when data is comming when it is set to 1 
  DB 4:7 to PD4:7
  Vss/GND to GND
  Vee/CV to 1k potentiometer wired to 5v, GND (LCD contrast)
  Vdd/VCC to 5v
  LCD Backlight (optional depending on display): LED+ through resistor (Used 1k but anything is fine) to 5v and LED- to GND

  Order of Operations:
  1. Set RS bit (PB5 pin) (low for lcd commands and high for data)
  2. Raise Enable bit to 1 (PB4)
  3. Set Data bits to the first upper 4bit half of the 8bit command (PD4, PD5, PD6, PD7 pins)
  4. Lower Enable bit to 0 (PB4 pin)
  5. Wait at least minimum time for lcd to recieve the data (used 2ms)
  6. Repeat process 2 through 5 to send the last lower 4bit half of the 8bit command (PD4, PD5, PD6, PD7 pins)
  7. Repeat the entire process to send a new command/data

  8bit operation would just send all the Data at once but because we are using 4bit, we have to send in two sections

  Youtube video great for explaining how the display works https://www.youtube.com/watch?v=cXpeTxC3_A4

  A wiring diagram is included in the file structure.
*/

#ifndef _LCD_C
#define _LCD_C

#include <math.h>
#include <inttypes.h>
#include <avr/io.h>				// is needed for IO port declarations
#include <avr/interrupt.h>
#include <stdlib.h>
#include "lcd.h"

void LcdInit(void)
{
	delay_ms(100);             // wait 100ms for the LCD to come out of reset
	LcdPortInit();             // initalize the port pins
	LcdCommandWrite(0x33);	   // set DDRAM address: init LCD to 4bit interface
	LcdCommandWrite(0x32);	   // set DDRAM address: init LCD to 4bit interface
	LcdCommandWrite(0x28);	   // set DDRAM address: set two-line display
	LcdCommandWrite(0x0C);     // Display On/Off: display on, curser off, curser blink off
	LcdCommandWrite(0x06);     // Entry Mode Set: increment DD RAM after each write/read, increment curser right after each write/read
	LcdCommandWrite(LCD_CLR);	 // Clear Display: 0x01 clear display and returns curser to the home position
	LcdCommandWrite(LCD_HOME); // Return Home: 0x02 return home
}

void LcdPortInit(void)
{
	//LCD Pins setup for Outputs
	DDRB |= (1<<PORTB4); //Enable bit
	DDRB |= (1<<PORTB5); //RS bit
	PORTB &= (~(1<<PORTB4)) & (~(1<<PORTB5)); //Setting Enable and RS bits to 0 (Just being safe)

	DDRD |= (1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7); //Data bits
	PORTD &= (~(1<<PORTD4)) & (~(1<<PORTD5)) & (~(1<<PORTD6)) & (~(1<<PORTD7)); //Setting all Data bits to 0 (Just being safe)
}

void LcdCommandWrite(uint8_t cmd)
{
	PORTB &= ~(1<<PORTB5); //Lower RS bit for Commands
	delay_ms(2); 

	PORTB |= (1<<PORTB4); //Raise Enable bit
	PORTD = (PORTD & 0x0F) | (cmd & 0xF0); //Mask for the Data bits and set to upper nibble (4bits)
	PORTB &= ~(1<<PORTB4); //Lower Enable bit
	delay_ms(2); //Wait for Data to get uploaded the LCD

	PORTB |= (1<<PORTB4); //Raise Enable bit
	PORTD = (PORTD & 0x0F) | ((cmd<<4) & 0xF0); //Mask for the Data bits and set to lower nibble (4bits)
	PORTB &= ~(1<<PORTB4); //Lower Enable bit
	delay_ms(2); //Wait for Data to get uploaded the LCD

	//Can Raise RS bit then remove the raise from the LcdDataWrite function, speed up data write but slows down command writes
	//PORTB |= (1<<PORTB5); //Raise RS bit end of command
	//delay_ms(2);
}

void LcdDataWrite(uint8_t data)
{
	PORTB |= (1<<PORTB5); //Raise RS bit for Data
	delay_ms(2);

	PORTB |= (1<<PORTB4); //Raise Enable bit
	PORTD = (PORTD & 0x0F) | (data & 0xF0); //Mask for the Data bits and set to upper nibble (4bits)
	PORTB &= ~(1<<PORTB4); //Lower Enable bit
	delay_ms(2); //Wait for Data to get uploaded the LCD

	PORTB |= (1<<PORTB4); //Raise Enable bit
	PORTD = (PORTD & 0x0F) | ((data<<4) & 0xF0); //Mask for the Data bits and set to lower nibble (4bits)
	PORTB &= ~(1<<PORTB4); //Lower Enable bit
	delay_ms(2); //Wait for Data to get uploaded the LCD
}

//Call Lcd Command Clear Display: 0x01 clear display and returns curser to the home position
void LcdClear(void)
{
	LcdCommandWrite(LCD_CLR);
}

//Call Lcd Command Return Home: 0x02 return home
void LcdHome(void)
{
	LcdCommandWrite(LCD_HOME);
}

//Print a single character based on ascii value to lcd (uint8_t = char)
//Accepts any bin/dec/hex ascii value or char (such as 'x')
//Doesn't accept double quotes "x" as it would be a string literal.
//A string literal "x" is a string, it contains character 'x' and a null terminator '\0'. So "x" is two character array.
void LcdPrintChar(uint8_t character)
{
	LcdDataWrite(character);
}

//Prints a string/array of chars (such as "Testing")
//char *string will point to the first char in a string, which is just an array of chars
//Doesn't need string.h library at all
void LcdPrintString(const char *string)
{
	//Iterates from the begining of the string/array of char to the end
	while(*string != 0) //Since the string is terminated by '\0', which is 0 in ASCII value
	{
		LcdDataWrite(*string);
		string++;
	}
}

//Prints uint8 value to lcd
//Range 0 to 255 (dec)
void LcdPrintUint8(uint8_t number)
{
	uint8_t y;
	uint8_t leading;
	leading = 1;

	//Printing Hundreds place
	y = number / 100;
	if(y > 0)
	{
		LcdDataWrite(y + '0'); //Need to add ascii '0' to get to the start of digits in ascii
		leading = 0; //Force printing of tens place for number like 105 where the zero needs to be printed
	}

	//Printing Tens place
	number = number - (y * 100); //removing hundreds place value
	y = number / 10;
	if((y > 0) || (leading == 0))
	{
		LcdDataWrite(y + '0');
		leading = 1;
	}

	//Printing Ones place
	number = number - (y * 10); //removing tens place value
	LcdDataWrite(number + '0');
}

//Prints int8 value to lcd
//Handles the negitive sign if the value is negitive
//Range -128 to 127 (dec)
void LcdPrintInt8(int8_t number)
{
	uint8_t y;
	uint8_t leading;
	leading = 1;

	if(number < 0) //negitive number
	{
		LcdPrintString("-");
		y = number / (-100); //Finding Hundreds value and making it positive
		number = -(number + (y * 100)); //removing hundreds place value and making value positive
	}
	else //positive number
	{
		y = number / 100; //Finding Hundreds value
		number = number - (y * 100); //removing hundreds place value
	}

	//Printing Hundreds place
	if(y > 0)
	{
		LcdDataWrite(y + '0'); //Need to add ascii '0' to get to the start of digits in ascii
		leading = 0; //Force printing of tens place for number like 105 where the zero needs to be printed
	}

	//Printing Tens place
	y = number / 10;
	if((y > 0) || (leading == 0))
	{
		LcdDataWrite(y + '0');
		leading = 1;
	}

	//Printing Ones place
	number = number - (y * 10); //removing tens place value
	LcdDataWrite(number + '0');
}

//Prints uint16 value to lcd
//Range 0 to 65535 (dec)
void LcdPrintUint16(uint16_t number)
{
	uint16_t y;
	uint8_t leading;
	leading = 1;

	//Printing Ten Thousands place
	y = number / 10000;
	if(y > 0)
	{
		LcdDataWrite(y + '0'); //Need to add ascii '0' to get to the start of digits in ascii
		leading = 0; //Force printing of tens place for number like 105 where the zero needs to be printed
	}

	//Printing Thousands place
	number = number - (y * 10000); //removing hundreds place value
	y = number / 1000;
	if((y > 0) || (leading == 0))
	{
		LcdDataWrite(y + '0');
		leading = 0;
	}

	//Printing Hundreds place
	number = number - (y * 1000); //removing hundreds place value
	y = number / 100;
	if((y > 0) || (leading == 0))
	{
		LcdDataWrite(y + '0');
		leading = 0;
	}

	//Printing Tens place
	number = number - (y * 100); //removing hundreds place value
	y = number / 10;
	if((y > 0) || (leading == 0))
	{
		LcdDataWrite(y + '0');
		leading = 1;
	}

	//Printing Ones place
	number = number - (y * 10); //removing tens place value
	LcdDataWrite(number + '0');
}

//Prints int16 value to lcd
//Handles the negitive sign if the value is negitive
//Range -32768 to 32767 (dec)
void LcdPrintInt16(int16_t number)
{
	uint16_t y;
	uint8_t leading;
	leading = 1;

	if(number < 0) //negitive number
	{
		LcdPrintString("-");
		y = number / (-10000); //Finding Hundreds value and making it positive
		number = -(number + (y * 10000)); //removing hundreds place value and making value positive
	}
	else //positive number
	{
		y = number / 10000; //Finding Hundreds value
		number = number - (y * 10000); //removing hundreds place value
	}

	//Printing Ten Thousands place
	if(y > 0)
	{
		LcdDataWrite(y + '0'); //Need to add ascii '0' to get to the start of digits in ascii
		leading = 0; //Force printing of tens place for number like 105 where the zero needs to be printed
	}

	//Printing Thousands place
	y = number / 1000;
	if((y > 0) || (leading == 0))
	{
		LcdDataWrite(y + '0');
		leading = 0;
	}

	//Printing Hundreds place
	number = number - (y * 1000); //removing hundreds place value
	y = number / 100;
	if((y > 0) || (leading == 0))
	{
		LcdDataWrite(y + '0');
		leading = 0;
	}

	//Printing Tens place
	number = number - (y * 100); //removing hundreds place value
	y = number / 10;
	if((y > 0) || (leading == 0))
	{
		LcdDataWrite(y + '0');
		leading = 1;
	}

	//Printing Ones place
	number = number - (y * 10); //removing tens place value
	LcdDataWrite(number + '0');
}

//Print float value on lcd
//Relies on functions from the math.h library
void LcdPrintFloat(float number)
{
	uint8_t	x,i;
	int8_t	e;
	float	y;

	if(number < 0)
	{
		LcdPrintString("-");
		number = (-number);
	}
	e = log10(number);

	if(e != 0)
	{
		x = number / (pow(10, e));
		y = number / (pow(10, e));
	}
	else
	{
		x = number;
		y = number;
	}
	if(x < 1)
	{
		e--;
		x = number / (pow(10, e));
		y = number / (pow(10, e));
	}
	LcdPrintUint8(x);
	LcdPrintString(".");

	for(i = 0; i < 5; i++)
	{
		y = (y - x) * 10;
		x = y;
		LcdPrintUint8(x);
		
	}

	LcdPrintString("E");
	LcdPrintInt8(e);
}

//Prints uint8 value in hex on lcd
//Range 0x00 to 0xFF (0 to 255 dec)
void LcdPrintHex(uint8_t hex)
{
	uint8_t hi;
	uint8_t lo;

	//Printing Upper hex value
	hi = hex & 0xF0; //Forcing lower half of hex to 0s
	hi = hi >> 4;    //Removing lower half of hex value
	hi = hi + '0';   //Adding Null character
	if(hi > '9')     //Handling A, B, C, D, E, F
	{
		hi = hi + 7;
	}

	//Printing Lower hex value
	lo = hex & 0x0F; //Forcing upper half of hex to 0s resulting in just lower
	lo = lo + '0';
	if(lo > '9')
	{
		lo = lo + 7; //Adding 7 to skip from the ascii value of 9 through 7 symbols to the starting letter A
	}

	LcdDataWrite(hi);
	LcdDataWrite(lo);
}

//Required Includes to do variable arguments
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// Implements a simple printf for the LCD
// supported formats:
//  %d,%i	signed 16bit integer
//  %c		character
//  %f		float
//  %s		null-terminated string
//	%o		octal number
//  %x		hex number
//  %u		unsigned 16bit integer
//  %%		%
//
// doubles are not supported (floats and doubles are the same for gcc for the AVR
// no formatting is implemented
// \n \t etc not yet supported
//
// Requires a special function called a Variadic Function or a function that takes in variable arguments
// Basically just a wrapper function that calls many different print functions based on the types
void LcdPrintf(char *fmt, ...)
{
	va_list ap;       //Holds list of arguments
	char *p;
	char *sval;       //Stores string value
	int8_t cval;      //Stores char value
	int16_t	ival;     //Stores int16 value
	double dval;	  //Store double which is float value
	uint16_t base;    //Stores base of value
	uint16_t uval;    //Stores uint16 value
	char scratch[16];

	va_start(ap, fmt); //Macro from stdarg.h that points were the arguments were stored when the call was made
	for(p = fmt; *p; p++)
	{
		if(*p != '%')
		{
			if(*p == '\n')
			;
			else if(*p == '\t')
			;
			else if(*p == '\b')
			;
			else
				LcdPrintChar(*p);
			continue;
		}

		switch(*++p)
		{
			//Signed Decimal/Integer 16bit
			case 'd':
			case 'i':
				ival = va_arg(ap, int16_t);
				LcdPrintInt16(ival);
				break;

			//Char
			case 'c':
				cval = va_arg(ap, int16_t);
				LcdPrintChar(cval);
				break;

			//Float 
			case 'f':
				dval = va_arg(ap, double);
				LcdPrintFloat(dval);
				break;

			//String
			case 's':
				sval = va_arg(ap, char *);
				LcdPrintString(sval);
				break;

			//Hex (base 16)
			case 'x':
			case 'X':
				base = 16;
				sval = scratch + 16;
				*--sval = 0;
				uval = va_arg(ap, uint16_t);
				do {
					char ch = uval % base + '0';
					if (ch > '9')
						ch += 'a' - '9' - 1;
					*--sval = ch;
					uval /= base;
				} while (uval);
				LcdPrintString(sval);
				break;

			//Octal (Base 8)
			case 'o':
				base = 8;
				sval = scratch + 16;
				*--sval = 0;
				uval = va_arg(ap, uint16_t);
				do {
					char ch = uval % base + '0';
					if (ch > '9')
					ch += 'a' - '9' - 1;
					*--sval = ch;
					uval /= base;
				} while (uval);
				LcdPrintString(sval);
				break;

			//Unsigned 16bit
			case 'u':
				base = 10;
				sval = scratch + 16;
				*--sval = 0;
				uval = va_arg(ap, uint16_t);
				do {
					char ch = uval % base + '0';
					if (ch > '9')
						ch += 'a' - '9' - 1;
					*--sval = ch;
					uval /= base;
				} while (uval);
				LcdPrintString(sval);
				break;

			default:
				LcdPrintChar(*p);
				break;
			}
	}
	va_end(ap); //Required for some compilers to call the end macro
}

//Sets Lcd cursor to the position x,y (x range 0-1, y range 0-15)
void LcdGoToXY(uint8_t x, uint8_t y)
{
	uint8_t DDRAMAddr;
	 
	switch(y)
	{
		case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR + x; break; //Get starting address and add offset
		case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR + x; break;
		default: DDRAMAddr = LCD_LINE0_DDRAMADDR + x;
	}

	//Set data address
	LcdCommandWrite(1<<LCD_DDRAM | DDRAMAddr);
}


#endif
