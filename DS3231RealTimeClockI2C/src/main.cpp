/* This project will interface a DS3231 Real Time Clock Module with the Arduino Uno

  DS3231 Wiring

  Note, my first I2C Backpack fried itself so be careful wiring up

  Order of Operations:



  A wiring diagram is included in the file structure.
*/
#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>

#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

// #include <Arduino.h> //Probably dont need, just for VS Code/Plateform IO
#include <compat/twi.h> //WinAVR Library in avr/include/util/twi.h


//TWI Function Prototypes
unsigned char  i2c_transmit(unsigned char type);
char i2c_start(unsigned int dev_id, unsigned int dev_addr, unsigned char rw_type);
void i2c_stop(void);
char i2c_write(char data);
char i2c_read(char *data,char ack_type);
void Write_DS3231(unsigned char reg_addr,unsigned char data);
unsigned char Read_DS3231(unsigned char reg_addr);
void i2c_init(void);

// Function prototypes for data processing
void Set_Time_DS3231(uint8_t seconds, uint8_t minutes, uint8_t hours);
void Set_Date_DS3231(uint8_t day, uint8_t month, uint8_t year);

void Read_Time_DS3231(uint8_t *seconds, uint8_t *minutes, uint8_t *hours);
void Read_Date_DS3231(uint8_t *day, uint8_t *month, uint8_t *year);

//////////////////////////////////
/* SETUP FOR DS3231 */
//////////////////////////////////
#define SetBit(a, b) (a) |= (1 << (b))
#define ClearBit(a, b) (a) &= ~(1 << (b))

//DS3231 SPI/I2C LCD backpack driver
#define DS3231_ID    0x68  // DS3231 Device Identifier
#define DS3231_ADDR  0x00  // DS3231 Device Address

//DS3231 Register Addresses
#define DS3231_SECONDS  0x00
#define DS3231_MINUTES  0x01
#define DS3231_HOURS    0x02
#define DS3231_DAY      0x03
#define DS3231_DATE     0x04
#define DS3231_MONTH    0x05
#define DS3231_YEAR     0x06

//I2C Status
#define I2C_START 0
#define I2C_DATA 1
#define I2C_DATA_ACK 2
#define I2C_STOP 3
#define ACK 1
#define NACK 0

#define MAX_TRIES 50
#define DATASIZE 32 //CHECK THIS


int main (void)
{
	//Initialize LCD Screen
	lcd_init();
	lcd_clear();
	lcd_home();
  
  /* Write Current Time */
  /* You only need to set the time once, it will stay set even after resets */
  /* You can run the code once to set and then reprogram Arduino with this section commented out */
  uint8_t setSeconds = 0x00;
  uint8_t setMinutes = 0x00;
  uint8_t setHours = 0x00;

  Set_Time_DS3231(setSeconds, setMinutes, setHours);

  /* Read Current Time */
  uint8_t readSeconds;
  uint8_t readMinutes;
  uint8_t readHours;

  // Read_Time_DS3231(&readSeconds, &readMinutes, &readHours);

  // //Print Seconds for Debugging
  // lcd_printf("%d",readSeconds);

	while(1)
	{
    Read_Time_DS3231(&readSeconds, &readMinutes, &readHours);

    //Print Seconds for Debugging
    lcd_printf("%d",readSeconds);
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
unsigned char i2c_transmit(unsigned char type) {
	switch(type) 
  {
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

// Write data to DS3231
void Write_DS3231(unsigned char reg_addr,unsigned char data)
{
   // Start the I2C Write Transmission
   i2c_start(DS3231_ID,DS3231_ADDR,TW_WRITE);

   // Sending the Register Address
   i2c_write(reg_addr);

   // Write data to DS3231 Register
   i2c_write(data);

   // Stop I2C Transmission
   i2c_stop();
}

// Read data from DS3231
unsigned char Read_DS3231(unsigned char reg_addr)
{
   char data;

   // Start the I2C Write Transmission
   i2c_start(DS3231_ID,DS3231_ADDR,TW_WRITE);

   // Read data from DS3231 Register Address
   i2c_write(reg_addr);

   // Stop I2C Transmission
   i2c_stop();

   // Re-Start the I2C Read Transmission
   i2c_start(DS3231_ID,DS3231_ADDR,TW_READ);

   i2c_read(&data,NACK);

   // Stop I2C Transmission
   i2c_stop();

   return data;
}


///////////////////////////////////////
/* DS3221 Data Processing Functions  */
///////////////////////////////////////
void Set_Time_DS3231(uint8_t seconds, uint8_t minutes, uint8_t hours)
{
  //Write Seconds
	Write_DS3231(DS3231_SECONDS,seconds);
	delay_ms(1); //Wait for data to be processed

  //Write Minutes
	Write_DS3231(DS3231_MINUTES,minutes);
	delay_ms(1); //Wait for data to be processed

  //Write Hours
	Write_DS3231(DS3231_HOURS,hours);
	delay_ms(1); //Wait for data to be processed
}

void Set_Date_DS3231(uint8_t day, uint8_t month, uint8_t year)
{

}

void Read_Time_DS3231(uint8_t *seconds, uint8_t *minutes, uint8_t *hours)
{

  *seconds = Read_DS3231(DS3231_SECONDS);

  *minutes = Read_DS3231(DS3231_MINUTES);

  *hours = Read_DS3231(DS3231_HOURS);
}

void Read_Date_DS3231(uint8_t *day, uint8_t *month, uint8_t *year)
{

}
