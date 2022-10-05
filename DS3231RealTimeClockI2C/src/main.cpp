/* This project will interface a DS3231 Real Time Clock Module with the Arduino Uno

  The I2C in this project was implemented a little different from the LCD Backpack project.
  The I2C communications code works the same as LCD Backpack just less abstract functions.
  This project could be updated to use the base I2C functions as LCD Backpack project.

  Read through the Datasheet attached in the folder for more information

  DS3231 Wiring:
  DAT(Data) to SDA/PC4/A4 Pin
  CLK(Clock) to SCL/PB5/A5 Pin
  5v to 5v Pin
  GND to GND Pin

  Order of Operations:
  There is no fancy bit setting required for this project like LCD backpack needed.
  Below is the exact operation we are doing to write/read to/from the DS3231
  The following operations will be the same for most I2C systems
  We can get away with continued writes and reads because the data we want is next to
  eachother in the DS3231 addresses.
  The Write/Read timing diagram in the datasheet shows exactly how to commuicate with the DS3231

  Write:
  1. Send Start bit
     Check return status
  2. Send DS3231 Address containing 0 for write bit
     Check return status
  3. Send DS3231 Memory address we want to write to
     Check return status
  4. Send Seconds Data we want to write
     Check return status
  5. Send Minutes Data we want to write
     Check return status
  6. Send Hours data we want to write
     Check return status
  7. Send Stop bit

  Read:  
  1. Send Start bit
     Check return status
  2. Send DS3231 Address containing 1 for read bit
     Check return status
  3. Send DS3231 Memory address we want to read from
     Check return status
  4. Send Stop bit
  5. Send Start bit
     Check return status
  6. Send DS3231 Address containing 1 for read bit
     Check return status
  7. Read Data buffer for Seconds
     Send Ack
  8. Read Data buffer for Minutes
     Send Ack
  9. Read Data buffer for Hours
     Send Nack
  10. Send Stop bit
*/
#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <util/delay.h>
#include <util/twi.h>

#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

// #include <Arduino.h> //Probably dont need, just for VS Code/Plateform IO

//https://www.avrfreaks.net/forum/ds3231-interfacing-atmega328p#comment-3046406

/*All Macro Definitions*/
#define FOSC 16000000
#define FREQ_SCL 400000
//Arduino IDE Example I2C Scanner says address is 0x68 because that is the 7bits and 
//does not take into account the read/write bit.
#define RTCADDR 208 //Datasheet shows address is 1101000X where X is 0 for write and 1 for read
#define RTCADDR_W 208
#define RTCADDR_R 209

struct TIME
{
  unsigned char hh;
  unsigned char mm;
  unsigned char ss;
};

/*TWI Function Prototypes*/
void TWI_Init(void);
void TWI_Stop(void);
unsigned char TWI_Start(unsigned char);
unsigned char TWI_Write(unsigned char);
unsigned char TWI_Read_Ack(void);
unsigned char TWI_Read_Nack(void);

/*RTC Function Prototypes*/
void RTC_Time_Set(unsigned char, unsigned char, unsigned char);
void RTC_Time_Get(struct TIME*);

int main()
{
  //Initialize LCD Screen
	lcd_init();
	lcd_clear();
	lcd_home();

	struct TIME timeObj;

	TWI_Init();

  /* Setting Time Values */
	timeObj.ss=0;  //8bit value: Upper 4bits are 10s place, lower 4bits are 1s place
	timeObj.mm=3;  //8bit value: Upper 4bits are 10s place, lower 4bits are 1s place
	timeObj.hh=22; //8bit value: bit7=0, bit6=0 for 24hour mode, bit5 is for 20, bit4 is for 10, lower 4 bits(bit3-bit0) are 1s place

  /* Command to set the time */
  /* !!! Only needs to happen once then you can comment out the following line and load the code again !!! */
	RTC_Time_Set(timeObj.hh,timeObj.mm,timeObj.ss);

  while(1)
  {
    RTC_Time_Get(&timeObj);

    lcd_home();

    if(timeObj.hh >= 10)
    {
      lcd_printf("%dh",timeObj.hh);
    }
    else
    {
      lcd_printf("0%dh",timeObj.hh);
    }

    if(timeObj.mm >= 10)
    {
      lcd_printf("%dm",timeObj.mm);
    }
    else
    {
      lcd_printf("0%dm",timeObj.mm);
    }

    if(timeObj.ss >= 10)
    {
      lcd_printf("%ds",timeObj.ss);
    }
    else
    {
      lcd_printf("0%ds",timeObj.ss);
    }
   
  }

  return 0;
}
void TWI_Init(void)
{
  // Initial ATMega328P TWI/I2C Peripheral
  // TWI Status Register: Select Prescaler of 1 (TWPS1=0, TWPS0=0)
  // Also holds status codes and we are clearing all
  TWSR = 0x00; 

  // Setting SCL (Serial clock) which is controlled by setting TWI Bit Rate Register
  // SCL frequency = (Cpu Clock Frequency) / ( 16 + 2(TWBR) * Prescaler Value)
  // SCL frequency = 16000000 / (16 + 2 * 12 * 1) = 400 kHz
  // DS3231 Datasheet asks for 400 khz
  TWBR = ((FOSC / FREQ_SCL) - 16) / 2;

  /* Send Start Condition */
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
}

unsigned char TWI_Start(unsigned char addr)
{
  unsigned char twst;

  /* Send Start Condition */
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  /* Check return status */
  twst = TW_STATUS & 0xF8; // Return TWI Status Register, mask off the prescaler bits (TWPS1,TWPS0)
  if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;

  /* Send Data Containing Device Address */
  TWDR = addr;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  /* Check return status */
  twst = TW_STATUS & 0xF8; // Return TWI Status Register, mask off the prescaler bits (TWPS1,TWPS0)
  if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;

  return 0;
}

void TWI_Stop(void)
{
  /* Send Stop Condition */
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while (TWCR & (1 << TWSTO));
}

unsigned char TWI_Write(unsigned char byte_data)
{
  unsigned char twst;

  /* Send Data */
  TWDR = byte_data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  /* Check return status */
  twst = TW_STATUS & 0xF8; // Return TWI Status Register, mask off the prescaler bits (TWPS1,TWPS0)
  if ( twst != TW_MT_DATA_ACK) return 1;

  return 0;
}

unsigned char TWI_Read_Ack(void)
{
  /* Send Read Ack */
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  while (!(TWCR & (1 << TWINT)));

  return TWDR;
}

unsigned char TWI_Read_Nack(void)
{
  /* Send Read Nack */
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));

  return TWDR;
}

/*RTC Function Definitions*/
void RTC_Time_Set(unsigned char hh, unsigned char mm, unsigned char ss)
{
  unsigned char memoryStartAddr = 0; //Start address for where seconds are held

  TWI_Start(RTCADDR_W);
  TWI_Write(memoryStartAddr);
  TWI_Write(ss);
  TWI_Write(mm);
  TWI_Write(hh);
  TWI_Stop();
}

void RTC_Time_Get(struct TIME * timePtr)
{
  unsigned char memoryStartAddr = 0; //Start address for where seconds are held

  TWI_Start(RTCADDR_W);
  TWI_Write(memoryStartAddr);
  TWI_Stop();                       //Timing diagram doesnt show a stop but all examples I see has it

  TWI_Start(RTCADDR_R);
  timePtr->ss=TWI_Read_Ack();
  timePtr->mm=TWI_Read_Ack();
  timePtr->hh=TWI_Read_Nack();      //Datasheet wants a Nack at the end
  TWI_Stop();

  /* Process Data */
  timePtr->ss= (((timePtr->ss & 0xF0) >> 4) * 10 ) + (timePtr->ss & 0x0F); //Upper 4bits are 10s place, lower 4bits are 1s place
  timePtr->mm= (((timePtr->mm & 0xF0) >> 4) * 10 ) + (timePtr->mm & 0x0F); //Upper 4bits are 10s place, lower 4bits are 1s place
  /* Using 24hour setting where hour bit6=0 */
  timePtr->hh= (((timePtr->hh & 0x20) >> 4) * 20 ) + (((timePtr->hh & 0x10) >> 4) * 10 ) + (timePtr->hh & 0x0F); //Bit 5 is for 20, bit 4 is for 10, lower 4 bits are 1s place
}