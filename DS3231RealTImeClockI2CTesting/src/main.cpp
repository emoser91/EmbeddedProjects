// #include <Arduino.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <util/delay.h>
#include <util/twi.h>

#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

// #include <Arduino.h> //Probably dont need, just for VS Code/Plateform IO
#include <compat/twi.h> //WinAVR Library in avr/include/util/twi.h

//https://www.avrfreaks.net/forum/ds3231-interfacing-atmega328p#comment-3046406

/*All Macro Definitions*/
#define FOSC 16000000
#define FREQ_SCL 400000
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
	timeObj.ss=10;
	timeObj.mm=11;
	timeObj.hh=12;
	RTC_Time_Set(timeObj.hh,timeObj.mm,timeObj.ss);
	RTC_Time_Get(&timeObj);

  lcd_printf("h:%d",timeObj.hh);
  lcd_printf("m:%d",timeObj.mm);
  lcd_printf("s:%d",timeObj.ss);

  while(1)
  {
    RTC_Time_Get(&timeObj);

    lcd_home();
    lcd_printf("h:%d",timeObj.hh);
    lcd_printf("m:%d",timeObj.mm);
    lcd_printf("s:%d",timeObj.ss);

    // delay_ms(1000);
  }
}
void TWI_Init(void)
{
  TWSR = 0;
  TWBR = ((FOSC / FREQ_SCL) - 16) / 2;
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
}

unsigned char TWI_Start(unsigned char addr)
{
  unsigned char twst;
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  twst = TW_STATUS & 0xF8; // Return TWI Status Register, mask off the prescaler bits (TWPS1,TWPS0)
  if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;
  TWDR = addr;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  twst = TW_STATUS & 0xF8; // Return TWI Status Register, mask off the prescaler bits (TWPS1,TWPS0)
  if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
  return 0;
}

void TWI_Stop(void)
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while (TWCR & (1 << TWSTO));
}

unsigned char TWI_Write(unsigned char byte_data)
{
  unsigned char twst;
  TWDR = byte_data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  twst = TW_STATUS & 0xF8; // Return TWI Status Register, mask off the prescaler bits (TWPS1,TWPS0)
  if ( twst != TW_MT_DATA_ACK) return 1;
  return 0;
}

unsigned char TWI_Read_Ack(void)
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  while (!(TWCR & (1 << TWINT)));

  return TWDR;
}

unsigned char TWI_Read_Nack(void)
{
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));

  return TWDR;
}

/*RTC Function Definitions*/
void RTC_Time_Set(unsigned char hh, unsigned char mm, unsigned char ss)
{
  unsigned char memoryStartAddr = 0;
  TWI_Start(RTCADDR_W);
  TWI_Write(memoryStartAddr);
  TWI_Write(ss);
  TWI_Write(mm);
  TWI_Write(hh);
  TWI_Stop();
}

void RTC_Time_Get(struct TIME * timePtr)
{
  unsigned char memoryStartAddr = 0;
  TWI_Start(RTCADDR_W);
  TWI_Write(memoryStartAddr);
  TWI_Stop(); //Timing diagram doesnt show a stop
  TWI_Start(RTCADDR_R);
  timePtr->ss=TWI_Read_Ack();
  timePtr->mm=TWI_Read_Ack();
  timePtr->hh=TWI_Read_Nack(); //Maybe Nack?
  TWI_Stop();
}