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

#include <avr/io.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>
//#include <MSOE/lcd.c> //Trying to replicate my own version of this in the project
#include <Arduino.h>

//Everything before int main can be moved into a .h file and then #included
#define SetBit(a, b) (a) |= (1 << (b))
#define ClearBit(a, b) (a) &= ~(1 << (b))

#define LCD_CLR           0x01	// clear display
#define LCD_HOME			    0x02	// return to home position
#define LCD_CURSOR_RIGHT	0x14	// moves cursor right
#define LCD_CURSOR_LEFT		0x10	// moves cursor left
#define LCD_SHIFT_RIGHT		0x1C	// shifts display right
#define LCD_SHIFT_LEFT		0x18	// shifts display left

// Function prototypes
void LcdInit(void);
void LcdPortInit(void);
void LcdDataWrite(uint8_t);
void LcdCommandWrite(uint8_t);
void LcdClear(void);
void LcdHome(void);
void LcdPrintChar(uint8_t);
void LcdPrintString(const char *);
void LcdPrintUint8(uint8_t);
void LcdPrintInt8(int8_t);
void LcdPrintUint16(uint16_t);
void LcdPrintInt16(int16_t);
void LcdPrintHex(uint8_t);
void LcdPrintFloat(float);
// void LcdPrintf();
// void LcdGoToXY(uint8_t, uint8_t);
// void LcdGoToSecondLine(void);


int main (void)
{
  // MSOE Library Lcd Commands
	// lcd_init();
	// lcd_clear();
	// lcd_home();
  // lcd_printf("HI");

  LcdInit();
  LcdClear();
  LcdHome();

  //LcdDataWrite(0x48); //0x48=Ascii letter "H"
  //LcdDataWrite(0x49); //0x48=Ascii letter "I"

  //LcdPrintChar('H');
  //LcdPrintChar('I');

  //LcdPrintString("Questing");

  // uint8_t num = 255;
  // LcdPrintUint8(num);

  // int8_t negNum = -128;
  // LcdPrintInt8(negNum);

  // uint16_t numlarge = 65535;
  // LcdPrintUint16(numlarge);

  // int16_t negNumlarge = -32768;
  // LcdPrintInt16(negNumlarge);

  // LcdPrintHex(0x0A);
  // LcdPrintHex(0xFF);

  while(1)
  {
    //Do nothing
  }

  return 0;
}

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
		LcdDataWrite(y + '0'); //Need to add Null character at the end
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
		LcdDataWrite(y + '0'); //Need to add Null character at the end
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
		LcdDataWrite(y + '0'); //Need to add Null character at the end
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
		LcdDataWrite(y + '0'); //Need to add Null character at the end
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
		lo = lo + 7; //Adding 7 to skip from the ascii value of 9 through 7 symbols to the starting A
	}

	LcdDataWrite(hi);
	LcdDataWrite(lo);
}

// LcdPrintf()
// {

// }
