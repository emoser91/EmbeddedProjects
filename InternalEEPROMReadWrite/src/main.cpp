/* This project is a test of the Internal EEPROM on the Arduino Uno

  In this project, we will set a value in EEPROM and check to see that
  it has stayed after a RESET.
  
  Arduino Internal EEPROM
  *Atmega328p contains 1k byte of data EEPROM memory
  *EEPROM has an endurance of at least 100,000 write/erase cycles
  *EEPROM can corrupt if low power event happens but Brownout Detection will keep it safe
  *Make sure to turn off interrupts during the EEPROM write

  Description of Program
  1. Program will Read the value of EEPROM at a specific address and print it to LCD
  2. Program will add 1 to the current value
  3. Program will update the EEPROM to the new value and print it to LCD
  4. Program goes into infinite while loop doing nothing
  5. RESET the Arduino to see the EEPROM address value increment by 1 again

  LCD 16x2 Wiring:
  RS to PB5
  R/w to GND, as we are just doing writes
  E to PB4,
  DB 4:7 to PD4:7
  Vss/GND to GND
  Vee/CV to 1k potentiometer wired to 5v, GND (LCD contrast)
  Vdd/VCC to 5v
  LCD Backlight (optional depending on display): LED+ through resistor (Used 1k but anything is fine) to 5v and LED- to GND

  *Note: See attached docs in same folder for more information on Arduino Internal EEPROM
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>
#include <MSOE/lcd.c>
// #include <Arduino.h> //not really needed

//////////////////////////////////////////////
// EEPROM Functions: Polling Method
//////////////////////////////////////////////
#define EEPROM_SIZE 1023 //Max size of atmega328p EEPROM

enum{
	EEPROM_OK,
	EEPROM_WRITE_FAIL,
	EEPROM_INVALID_ADDR	
};

//EEPROM Read Function
uint8_t EEPROM_read(uint16_t uiAddress, uint8_t *data)
{
  /* Check that access a valid address */
	if(uiAddress > EEPROM_SIZE)
  {
		return EEPROM_INVALID_ADDR;
	}
	
  /* EECR bits EEPM1=0, EEPM0=0 for Erase and write in one operation mode*/

	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE)); //This is the polling method, could use interupt method instead

	/* Set up address register */
	EEARH = (uiAddress & 0xFF00) >> 8;
	EEARL = (uiAddress & 0x00FF);

	/* Start eeprom read by writing EERE: EEPROM Read Enable */
	EECR |= (1<<EERE);

	/* Return data from Data Register */
	*data = EEDR;

	return EEPROM_OK;
}

//EEPROM Write Function
//Can remove interrupt disable/enable lines from function if you are not using them
uint8_t EEPROM_write(uint16_t uiAddress, uint8_t ucData)
{
  //A interrupt during the write process can cause it to fail to write
  cli(); //disable interrupts

  /* Check that access a valid address */
	if(uiAddress > EEPROM_SIZE)
  {
		return EEPROM_INVALID_ADDR;
	}

  /* EECR bits EEPM1=0, EEPM0=0 for Erase and write in one operation mode*/

	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE)); //This is the polling method, could use interupt method instead

	/* Set up address and Data Registers */
	EEARH = (uiAddress & 0xFF00) >> 8;
	EEARL = (uiAddress & 0x00FF);

  /* Fill EEPROM Data Register with data */
	EEDR = ucData;

	/* Write logical one to EEMPE: EEPROM Master Write Enable*/
	EECR |= (1<<EEMPE);

	/* Start eeprom write by setting EEPE: EEPROM Write Enable */
	EECR |= (1<<EEPE);

  sei(); //Enable global interrupts

	return EEPROM_OK;
}

/* Wrapper function that only writes new data to the EEPROM if it is a new value
   and checks to make sure the data was properly written. 
*/
uint8_t EEPROM_update(uint16_t uiAddress, uint8_t ucData)
{
	uint8_t err = EEPROM_OK;

  /* Check that access a valid address */
	if(uiAddress > EEPROM_SIZE)
  {
		return EEPROM_INVALID_ADDR;
	}

  /* EEPROM has a set number of read/writes so we check to see
  if the address we are writing to already contains the value we want or not */
	uint8_t value = 0;
	err = EEPROM_read(uiAddress,&value);
	if(err != EEPROM_OK)
  {
		return err;
	}
	if(value == ucData)
  {
		return EEPROM_OK; //We are done, wanted value already exists
	}
	
  //Write the EEPROM data
	err = EEPROM_write(uiAddress,ucData);
	if(err != EEPROM_OK)
  {
		return err;
	}
	
  //Read the EEROM and check to make sure our value was properly set
	err = EEPROM_read(uiAddress,&value);
	if(err != EEPROM_OK)
  {
		return err;
	}
	if(value != ucData)
  {
		return EEPROM_WRITE_FAIL;
	}
	
	return EEPROM_OK;
	
}

/* Writting to a block of data of lenth to EEPROM
   Takes in any form of data and assumes its a uint8_t because that is the smallest we can work with
   Can handle arrays, strings, floats, doubles, chars, basically anything
*/
uint8_t EEPROM_update_batch(uint16_t uiAddress, void *data, uint16_t len)
{
	uint16_t i = 0;
	uint8_t err = EEPROM_OK;
	uint8_t *data_cast = (uint8_t *)data; //cast unknown data type to the smallest type
	
	for(i = 0; i < len; i++)
  {
		err = EEPROM_update(uiAddress + i,data_cast[i]);
		if(err != EEPROM_OK)
    {
			return err;
		}
	}

	return EEPROM_OK;
}

//Reading a block of data of lenth from EEPROM
uint8_t EEPROM_read_batch(uint16_t uiAddress, void *data, uint16_t len)
{
	uint16_t i = 0;
	uint8_t err = EEPROM_OK;
	uint8_t *data_cast = (uint8_t *)data; //cast unknown data type to the smallest type
	
	for(i = 0; i < len; i++)
  {
		err = EEPROM_read(uiAddress + i,&data_cast[i]);
		if(err != EEPROM_OK)
    {
			return err;
		}
	}

	return EEPROM_OK;
}


//////////////////////////////////////////////
// main function
//////////////////////////////////////////////
int main()
{
  //Initialize LCD Screen
	lcd_init();
	lcd_clear();
	lcd_home();
	
  uint8_t run = 0;
  uint8_t err = 0; //holds return status after EEPROM calls

  //Read current value
  err = EEPROM_read(96, &run); //writting address 96(just picked random) to the value of varriable run

  //Print current value to LCD
  lcd_printf("Current: %d",run);

  //Add 1 to the current value
  uint8_t count = run + 1;
  // uint8_t count = 0; //uncomment and comment above line to reset the EEPROM address value back down to 0

  //Update the EEPROM to the new value
  err = EEPROM_update(96, count);

  //Print new value to LCD
  lcd_goto_xy(0,1);
  lcd_printf("Updated: %d",count);

	while (1)
	{
			//do nothing
	}
	
	return 0;
}