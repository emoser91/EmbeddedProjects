/* 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	THIS PROJECT DOES NOT WORK AS OF YET!!!!
	I guess the Adafruit I2C/SPI Backpack doesnt use SPI
	The Adafruit LiquidCrystal Library doesnt use SPI Hardware either
	The Library does work to output a Hello World and can be found in this folder

	The Adafruit uses a 74HC595 serial to parallel shift register chip to perform
	what is calls "SPI"

	The Adafruit Library just shift out bits directly.
	The 74HC595 uses a 3 wire shift register interface, NOT SPI

	There might be a way to modify SPI to work as a 3 wire but I 
	am not sure. 

	The following code is SPI fully setup to transmit but it doesnt seem to work
	The LCD just acts as if it is getting junk data.

	This code MIGHT work for a different SPI LCD Backpack if it uses true SPI
	
	Google atmega328p to 74HC595 shift register to figure out how to fix this project
	Also look up shiftout functions because the Adafruit example uses it

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  This project is to get a LCD 16 by 2 working in SPI communication
  The LCD Backpack just has a spot you can solder over to enable SPI mode
  This will use the 4bit mode where we send half of a 8bit command, wait a 
  specific amount of time then send the other half of the 4 bits

  This project will try to reuse the LCD Functions from the I2C project and
  just redo the communication functions for SPI

  This project uses an Adafruit I2C/SPI LCD Backpack to take in I2C data and relay it to the LCD
  The LCD is still techically operating in parallel communication, we are just using the Backpack to let us
  send the data to it with I2C and save a lot of wiring

  Make sure to attach the LCD Backpack in the correct orintation, Pins all aligned and board tucked behind LCD screen
  Interface the LCD panel 16x2 with I2C Backpack attached to the Arduino Uno
  Goal: To print out Hello World on the LCD screen.

  LCD 16x2 with LCD Backpack Wiring:
  LAT(Latch)/SS/SC to PB2
  DAT(Data)/MOSI   to PB3
  CLK(Clock)/SCK   to PB5
  5v to 5v Pin
  GND to GND Pin

  The MCP23S08 is a slave SPI device. The slave
  address contains five fixed bits and two user-defined
  hardware address bits (pins A1 and A0), with the read/
  write bit filling out the control byte. 0 1 0 0 0 A1 A0 R/W, Write=0, Read=1

  SPI Order of Operations per Datasheet
  This function sends data to the chip
  It's a 5 step process
  1. Pull the Slave/Chip select LOW
  2. Send the chip's address to the chip *(Most SPI devices dont need this but the LCD Backpack does)
  3. Send the register to the chip
  4. Send the value to the chip
  5. Pull the Slave/Chip select HIGH

  Order of Operations:
  (Instead of setting outputs like in lcd parallel example, we will set bits in a temp register then send the data via SPI)
  1. Set RS bit (first bit) (low for lcd commands and high for data)
  2. Raise Enable bit to 1 (second bit)
  3. Set Data bits to the first upper 4bit half of the 8bit command (4 bits)
  4. Lower Enable bit to 0
  5. Wait at least minimum time for lcd to recieve the data (used 2ms)
  6. Repeat process 2 through 5 to send the last lower 4bit half of the 8bit command (4 bits)
  7. Repeat the entire process to send a new command/data

  8bit operation would just send all the Data at once but because we are using 4bit, we have to send in two sections

  Youtube video great for explaining how the display works https://www.youtube.com/watch?v=cXpeTxC3_A4

*/

#include <math.h> //Required for float Math
#include <avr/io.h>
#include <inttypes.h>
#include "MSOE_I2C/delay.h"
#include <stdlib.h>
// #include <Arduino.h> //Probably dont need, just for VS Code/Plateform IO

///////////////////////////////////////////////////
/* SETUP FOR SPI */
//////////////////////////////////////////////////
#define IODIR 0x00           // MCP23008 I/O Direction Register
#define GPIO  0x09           // MCP23008 General Purpose I/O Register
#define OLAT  0x0A           // MCP23008 Output Latch Register

//SPI Function Prototypes
void SPIMasterInit(void);
void SPIMasterSend_Start(void);
void SPIMasterSend_Address(uint8_t dev_id, uint8_t rw_type);
void SPIMasterSend_Data(uint8_t);
void SPIMasterSend_Stop(void);
void Write_MCP23008(unsigned char reg_addr,unsigned char data);
unsigned char Read_MCP23008(unsigned char reg_addr);

//////////////////////////////////
/* SETUP FOR LCD */
//////////////////////////////////
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

#define LCD_CTRL_RS		1	// bit 1 of data word
#define LCD_CTRL_E		2	// bit 2 of data word


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

//////////////////////////////////
/* SPI Master */
//////////////////////////////////
#include <avr/io.h>
#include <util/delay.h>

#define SPI_DDR DDRB
#define SS      PORTB2 //Latch on LCD Backpack
#define MOSI    PORTB3 //DAT on LCD Backpack
#define MISO    PORTB4 
#define SCK     PORTB5 //CLK on LCD Backpack

//MCP23008 SPI/I2C LCD backpack driver
#define MCP23008_ID    0x40  // MCP23008 Device Identifier
#define MCP23008_ADDR  0x00  // MCP23008 Device Address

#define WRITE 0
#define READ 1

//SPI init
void SPIMasterInit(void)
{
	//set MOSI, SCK and SS as output
	DDRB |= (1<<MOSI) | (1<<SCK) | (1<<SS);
	//set SS to high
	PORTB |= (1<<SS);
	//enable master SPI at clock rate Fck/16 = 1Mhz
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void SPIMasterSend_Start(void)
{
  //Pull the Slave/Chip select LOW
	PORTB &= ~(1<<SS);
}

void SPIMasterSend_Address(uint8_t dev_id, uint8_t rw_type)
{
  //Send the chip's address to the chip
  //SPI normally doesnt need this but datasheet wants it
  /*The slave Address: 0 1 0 0 0 A1 A0 R/W, Write=0, Read=1*/
  SPDR = 0b01000000;

	//wait for transmition complete
	while (!(SPSR & (1<<SPIF)));
}

void SPIMasterSend_Data(uint8_t data)
{
	//send data
	SPDR = data;
	//wait for transmition complete
	while (!(SPSR & (1<<SPIF)));
}

void SPIMasterSend_Stop(void)
{
  //Pull the Slave/Chip select HIGH
	PORTB |= (1<<SS);
}

// Write data to MCP23008 SPI/I2C LCD backpack driver
void Write_MCP23008(unsigned char reg_addr,unsigned char data)
{
  // Pull the Slave/Chip select LOW
  SPIMasterSend_Start();
  delay_us(1);
  //Send the chip's address to the chip
  SPIMasterSend_Address(MCP23008_ID,WRITE);
// delay_us(1);
  // Send the register to the chip
  SPIMasterSend_Data(reg_addr);
// delay_us(1);
  // Send the value to the chip
  SPIMasterSend_Data(data);
// delay_us(1);
  // Pull the Slave/Chip select HIGH
  SPIMasterSend_Stop();
//   delay_us(1);
}

// Read data from MCP23008 SPI/I2C LCD backpack driver
// unsigned char Read_MCP23008(unsigned char reg_addr)
// {
//    char data;

//    // Start the I2C Write Transmission
//    i2c_start(MCP23008_ID,MCP23008_ADDR,TW_WRITE);

//    // Read data from MCP23008 Register Address
//    i2c_write(reg_addr);

//    // Stop I2C Transmission
//    i2c_stop();

//    // Re-Start the I2C Read Transmission
//    i2c_start(MCP23008_ID,MCP23008_ADDR,TW_READ);

//    i2c_read(&data,NACK);

//    // Stop I2C Transmission
//    i2c_stop();

//    return data;
// }

//////////////////////////////////
/* LCD Lower Level Functions */
/* Specific to TWI/I2C communication */
//////////////////////////////////
void LcdInit(void)
{
	delay_ms(100);             // wait 100ms for the LCD to come out of reset
	LcdPortInit();             // initalize the port pins and init SPI subsystem
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
	SPIMasterInit();                       //init SPI subsystem
	Write_MCP23008(IODIR,0b00000000); //Writing to LCD over I2C to set all IO to output
	Write_MCP23008(GPIO,0b00000000);  //Writing to LCD over I2C to clear out all GPIO Address to 0s
}

void LcdCommandWrite(uint8_t cmd)
{
	char temp_reg = 0x00;
	/* Read MCP23008 Output Latch Register. The OLAT register provides access to the output latches. 
	A read from this register results in a read of the OLAT and not the port itself. 
	A write to this register modifies the output latches that modifies the pins configured as outputs. */
	// temp_reg = Read_MCP23008(OLAT);        				//Read MCP23008 Output Latch Register

	/* Lower RS to send commands */
	temp_reg &= ~(1 << (LCD_CTRL_RS));					//Lower RS bit for Data
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
	char temp_reg = 0x00;

	// temp_reg=Read_MCP23008(OLAT);						//Read MCP23008 Output Latch Register

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

int main (void)
{
	LcdInit();
	LcdClear();
	LcdHome();

	LcdDataWrite(0x48); //0x48=Ascii letter "H"
	LcdDataWrite(0x49); //0x48=Ascii letter "I"/

	while(1)
	{
		LcdDataWrite(0x48);
		delay_ms(100);
	  //Do nothing
	}

	return 0;
}
