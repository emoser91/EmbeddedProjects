/* This project is to get a LCD 16 by 2 working in I2C communication
  This will use the 4bit mode where we send half of a 8bit command, wait a 
  specific amount of time then send the other half of the 4 bits

  Note: This project is trying to replicate the original MSOE_I2C lcd.c Library
  that was used in all the EE2920 class labs.

  This project uses an Adafruit I2C/SPI LCD Backpack to take in I2C data and relay it to the LCD
  The LCD is still techically operating in parallel communication, we are just using the Backpack to let us
  send the data to it with I2C and save a lot of wiring

  Make sure to attach the LCD Backpack in the correct orintation, Pins all aligned and board tucked behind LCD screen
  Interface the LCD panel 16x2 with I2C Backpack attached to the Arduino Uno
  Goal: To print out Hello World on the LCD screen

  LCD 16x2 with LCD Backpack Wiring:
  DAT(Data) to SDA/PC4/A4 Pin
  CLK(Clock) to SCL/PB5/A5 Pin
  5v to 5v Pin
  GND to GND Pin

  Note, my first I2C Backpack fried itself so be careful wiring up

  Order of Operations:
  (Instead of setting outputs like in lcd parallel example, we will set bits in a temp register then send the data via I2C)
  1. Set RS bit (first bit) (low for lcd commands and high for data)
  2. Raise Enable bit to 1 (second bit)
  3. Set Data bits to the first upper 4bit half of the 8bit command (4 bits)
  4. Lower Enable bit to 0
  5. Wait at least minimum time for lcd to recieve the data (used 2ms)
  6. Repeat process 2 through 5 to send the last lower 4bit half of the 8bit command (4 bits)
  7. Repeat the entire process to send a new command/data

  8bit operation would just send all the Data at once but because we are using 4bit, we have to send in two sections

  Youtube video great for explaining how the display works https://www.youtube.com/watch?v=cXpeTxC3_A4

  A wiring diagram is included in the file structure.
*/

#include <math.h> //Required for float Math
#include <avr/io.h>
#include <inttypes.h>
#include "MSOE_I2C/delay.h"
// #include "MSOE_I2C/lcd.h" //Trying to replicate my own version of this in the project
#include <stdlib.h>
#include <Arduino.h> //Probably dont need, just for VS Code/Plateform IO 

///////////////////////////////////////////////////
/* SETUP FOR TWO WIRE INTERFACE SUBSYSTEM (TWI) */
//////////////////////////////////////////////////
//#defines from avr TWI subsystem
#include <compat/twi.h> //WinAVR Library in avr/include/util/twi.h

#define MAX_TRIES 50

//MCP23008 SPI/I2C LCD backpack driver
#define MCP23008_ID    0x40  // MCP23008 Device Identifier
#define MCP23008_ADDR  0x00  // MCP23008 Device Address


#define IODIR 0x00           // MCP23008 I/O Direction Register
#define GPIO  0x09           // MCP23008 General Purpose I/O Register
#define OLAT  0x0A           // MCP23008 Output Latch Register
#define I2C_START 0
#define I2C_DATA 1
#define I2C_DATA_ACK 2
#define I2C_STOP 3
#define ACK 1
#define NACK 0

#define DATASIZE 32

#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))

//TWI Function Prototypes
unsigned char  i2c_transmit(unsigned char type);
char i2c_start(unsigned int dev_id, unsigned int dev_addr, unsigned char rw_type);
void i2c_stop(void);
char i2c_write(char data);
char i2c_read(char *data,char ack_type);
void Write_MCP23008(unsigned char reg_addr,unsigned char data);
unsigned char Read_MCP23008(unsigned char reg_addr);
void i2c_init(void);

//////////////////////////////////
/* SETUP FOR LCD */
//////////////////////////////////
#define SetBit(a, b) (a) |= (1 << (b))
#define ClearBit(a, b) (a) &= ~(1 << (b))

#define LCD_CLR             0x01	// clear display
#define LCD_HOME			0x02	// return to home position
#define LCD_CURSOR_RIGHT	0x14	// moves cursor right
#define LCD_CURSOR_LEFT		0x10	// moves cursor left
#define LCD_SHIFT_RIGHT		0x1C	// shifts display right
#define LCD_SHIFT_LEFT		0x18	// shifts display left

#define LCD_CGRAM           6      // set CG RAM address
#define LCD_DDRAM           7      // set DD RAM address

// DDRAM mapping
#define LCD_LINE0_DDRAMADDR		0x00	// on 2x16 0x00-0x0F
#define LCD_LINE1_DDRAMADDR		0x40	// on 2x16 0x40-0x4F

#define LCD_CTRL_RS		1	// bit 1 of I2C data word
#define LCD_CTRL_E		2	// bit 2 of I2C data word


// Function prototypes
//Low Level Commands
void LcdInit(void);
void LcdPortInit(void);
void LcdDataWrite(uint8_t);
void LcdCommandWrite(uint8_t);

//High Level Data Writting Functions
//These functions are identical to that of the parallel lcd communication
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
void LcdPrintf(char *fmt, ...);
void LcdGoToXY(uint8_t, uint8_t);


int main (void)
{
	LcdInit();
	LcdClear();
	LcdHome();
  
	// LcdDataWrite(0x48); //0x48=Ascii letter "H"
	LcdDataWrite(0x49); //0x48=Ascii letter "I"

	// LcdPrintChar('H');
	// LcdPrintChar('I');

	// LcdPrintString("Questing");

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

	// float fNum = 3.14159;
	// LcdPrintFloat(fNum);

	// LcdPrintf("Testing");

	// int16_t testInteger = -15;
	// LcdPrintf("Number = %d", testInteger);
	// LcdPrintf("Number = %i", testInteger);

	// char testChar = 'A';
	// LcdPrintf("Char = %c", testChar);

	// float testFloat = 3.14159;
	// LcdPrintf("FLT = %f", testFloat);

	// char testString[] = "testing";
	// LcdPrintf("String = %s", testString);

	// uint16_t testOctal = 012; //Note that Octal is preceded with a 0
	// LcdPrintf("Number = %o", testOctal);

	// uint8_t testHex = 0xFF;
	// LcdPrintf("Number = %x", testHex);

	// uint16_t testUint = 65;
	// LcdPrintf("Number = %u", testUint);

	// LcdPrintf("Line 0");
	// LcdGoToXY(0,1);
	// LcdPrintf("Line 1");

	while(1)
	{
	//Do nothing
	}

	return 0;
}

/////////////////////////////////////
/* Two Wire Interface (TWI) : I2C  */
/////////////////////////////////////
void i2c_init(void)
{
  // Initial ATMega328P TWI/I2C Peripheral
  TWSR = 0x00; // TWI Status Register: Select Prescaler of 1 (TWPS1=0, TWPS0=0)
               // Also holds status codes and we are clearing all

  // Setting SCL (Serial clock) which is controlled by setting TWI Bit Rate Register
  // SCL frequency = (Cpu Clock Frequency) / ( 16 + 2(TWBR) * Prescaler Value)
  // SCL frequency = 16000000 / (16 + 2 * 12 * 1) = 400 kHz
  // LCD Backpack MCP23008 Datasheet asks for 100 khz, 400 khz or 1.7 Mhz: we choose to use 400 khz
  TWBR = 0x0C; // 12 Decimal
}

/* START I2C Routine */
unsigned char i2c_transmit(unsigned char type) 
{
	switch(type) {
		//Each of the following cases are clearly outlined in the datasheet
		//Datasheet outlines what bits to set for specific cases
		case I2C_START:    // Send Start Condition
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); //TWCR: TWI Control Register
			break;
		case I2C_DATA:     // Send Data with No-Acknowledge
			TWCR = (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_DATA_ACK: // Send Data with Acknowledge
			TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_STOP:     // Send Stop Condition
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

		return 0;
  }

  // Wait for TWINT flag set on Register TWCR
  while (!(TWCR & (1 << TWINT)));

  // Return TWI Status Register, mask off the prescaler bits (TWPS1,TWPS0)
  return (TWSR & 0xF8);
}

// Function that sends start condition and slave address
// Function will return 0 if executed properly and -1 if it fails
char i2c_start(unsigned int dev_id, unsigned int dev_addr, unsigned char rw_type)
{
	//This function uses labels and goto statements
	unsigned char n = 0;
	unsigned char twi_status;
	char r_val = -1;

	i2c_retry:
		if (n++ >= MAX_TRIES) return r_val;

		// Transmit Start Condition
		twi_status = i2c_transmit(I2C_START);

		// Check the TWI Status
		if (twi_status == TW_MT_ARB_LOST) goto i2c_retry;
		if ((twi_status != TW_START) && (twi_status != TW_REP_START)) goto i2c_quit; //bad status recieved

		// Load slave address (SLA_W)
		TWDR = (dev_id & 0xF0) | (dev_addr & 0x0E) | rw_type; //Filling TWI Data Register

		// Transmit I2C Data
		twi_status = i2c_transmit(I2C_DATA);

		// Check the TWSR status
		if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) goto i2c_retry;
		if (twi_status != TW_MT_SLA_ACK) goto i2c_quit; //bad status recieved

		r_val = 0;

	i2c_quit:
		return r_val;
}

// Function will send stop condition
void i2c_stop(void)
{
  unsigned char twi_status;

  // Transmit I2C Data
  twi_status = i2c_transmit(I2C_STOP);
}

// Function will fill TWI data register and then send the data
// Function will return 0 if executed properly or -1 if it fails
char i2c_write(char data)
{
  unsigned char twi_status;
  char r_val = -1;

  // Send the Data to I2C Bus
  TWDR = data;

  // Transmit I2C Data
  twi_status = i2c_transmit(I2C_DATA);

  // Check the TWSR status
  if (twi_status != TW_MT_DATA_ACK) goto i2c_quit; //bad status recieved

  r_val = 0;

	i2c_quit:
  return r_val;
}

// Function will fill "data" varriable with the TWI Data register data and Send Acknowledge or No Acknowledge based on input
// Function will return 0 if executed properly or -1 if it fails
char i2c_read(char *data,char ack_type)
{
  unsigned char twi_status;
  char r_val = -1;

  if (ack_type) 
  {
    // Read I2C Data and Send Acknowledge
    twi_status = i2c_transmit(I2C_DATA_ACK);

    if (twi_status != TW_MR_DATA_ACK) goto i2c_quit; //bad status recieved
  } 
  else 
  {
    // Read I2C Data and Send No Acknowledge
    twi_status = i2c_transmit(I2C_DATA);

    if (twi_status != TW_MR_DATA_NACK) goto i2c_quit; //bad status recieved
  }

  // Get the Data
  *data = TWDR;

  r_val = 0;

  i2c_quit:
  return r_val;
}

// Write data to MCP23008 SPI/I2C LCD backpack driver
void Write_MCP23008(unsigned char reg_addr,unsigned char data)
{
   // Start the I2C Write Transmission
   i2c_start(MCP23008_ID,MCP23008_ADDR,TW_WRITE);

   // Sending the Register Address
   i2c_write(reg_addr);

   // Write data to MCP23008 Register
   i2c_write(data);

   // Stop I2C Transmission
   i2c_stop();
}

// Read data from MCP23008 SPI/I2C LCD backpack driver
unsigned char Read_MCP23008(unsigned char reg_addr)
{
   char data;

   // Start the I2C Write Transmission
   i2c_start(MCP23008_ID,MCP23008_ADDR,TW_WRITE);

   // Read data from MCP23008 Register Address
   i2c_write(reg_addr);

   // Stop I2C Transmission
   i2c_stop();

   // Re-Start the I2C Read Transmission
   i2c_start(MCP23008_ID,MCP23008_ADDR,TW_READ);

   i2c_read(&data,NACK);

   // Stop I2C Transmission
   i2c_stop();

   return data;
}



//////////////////////////////////
/* LCD Lower Level Functions */
/* Specific to TWI/I2C communication */
//////////////////////////////////
void LcdInit(void)
{
	delay_ms(100);             // wait 100ms for the LCD to come out of reset
	LcdPortInit();             // initalize the port pins and init i2c/twi subsystem
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
	i2c_init();                       //init i2c/twi subsystem
	Write_MCP23008(IODIR,0b00000000); //Writing to LCD over I2C to clear out all IO Directory Address to 0s
	Write_MCP23008(GPIO,0b00000000);  //Writing to LCD over I2C to clear out all GPIO Address to 0s
}

void LcdCommandWrite(uint8_t cmd)
{
	char temp_reg;
	/* Read MCP23008 Output Latch Register. The OLAT register provides access to the output latches. 
	A read from this register results in a read of the OLAT and not the port itself. 
	A write to this register modifies the output latches that modifies the pins configured as outputs. */
	temp_reg = Read_MCP23008(OLAT);        				//Read MCP23008 Output Latch Register

	/* Lower RS to send commands */
	temp_reg &= ~(1 << (LCD_CTRL_RS));					//Raise RS bit for Data
	Write_MCP23008(GPIO,temp_reg);
	delay_ms(1);

	/* Write Enable High, Write Data first half of 8bit data, Write Enable Low */
	temp_reg |= (1 << (LCD_CTRL_E));					//Raise Enable bit
	Write_MCP23008(GPIO,temp_reg);

	temp_reg=(temp_reg&0x87)|((cmd>>1)&0x78);		 	//Mask for the Data bits and set to upper nibble (4bits)
	Write_MCP23008(GPIO,temp_reg);

	temp_reg &= ~(1 << (LCD_CTRL_E));					//Lower Enable bit
	Write_MCP23008(GPIO,temp_reg);
	delay_ms(2);										//Wait 2ms so display uploads data

	/* Write Enable High, Write Data second half of 8bit data, Write Enable Low */
	temp_reg |= (1 << (LCD_CTRL_E));					//Raise Enable bit
	Write_MCP23008(GPIO,temp_reg);

	temp_reg=(temp_reg & 0x87) | ((cmd<<3) & 0x78);		//Mask for the Data bits and set to lower nibble (4bits)
	Write_MCP23008(GPIO,temp_reg);

	temp_reg &= ~(1 << (LCD_CTRL_E));					//Lower Enable bit
	Write_MCP23008(GPIO,temp_reg);
	delay_ms(2);										//Wait 2ms so display uploads data
}

void LcdDataWrite(uint8_t data)
{
	char temp_reg;

	temp_reg=Read_MCP23008(OLAT);						//Read MCP23008 Output Latch Register

	/* Raise RS bit to send Data */
	temp_reg |= (1 << (LCD_CTRL_RS));					//Raise RS bit for Data
	Write_MCP23008(GPIO,temp_reg);
	delay_ms(1);

	/* Write Enable High, Write Data first half of 8bit data, Write Enable Low */
	temp_reg |= (1 << (LCD_CTRL_E));					//Raise Enable bit
	Write_MCP23008(GPIO,temp_reg);
	temp_reg=(temp_reg&0x87)|((data>>1)&0x78);		 	//Mask for the Data bits and set to upper nibble (4bits)
	Write_MCP23008(GPIO,temp_reg);
	temp_reg &= ~(1 << (LCD_CTRL_E));					//Lower Enable bit
	Write_MCP23008(GPIO,temp_reg);
	delay_ms(1);										//Wait 1ms so display uploads data

	/* Write Enable High, Write Data second half of 8bit data, Write Enable Low */
	temp_reg |= (1 << (LCD_CTRL_E));					//Raise Enable bit
	Write_MCP23008(GPIO,temp_reg);
	temp_reg=(temp_reg & 0x87) | ((data<<3) & 0x78);	// send lower nibble
	Write_MCP23008(GPIO,temp_reg);
	temp_reg &= ~(1 << (LCD_CTRL_E));					//Lower Enable bit
	Write_MCP23008(GPIO,temp_reg);
	delay_ms(1);										//Wait 1ms so display uploads data
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/* LCD High Level Functions */
/* The following functions are exactly identical to that of the LCD Parallel Communication Project */
///////////////////////////////////////////////////////////////////////////////////////////////////////

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