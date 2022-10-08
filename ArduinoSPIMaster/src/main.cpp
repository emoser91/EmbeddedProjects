/* The Project will connect two Arduino Uno (atmega328p) over SPI Communication
  One of the Arduinos will be programmed in SPI Master Mode
  and the second Arduino will be programmed in SPI Slave Mode

  The following code is for the SPI Master Mode, check out the file
  labeled ArduinoSPISlave to see the SPI Slave side code. 

  Arduino Wiring:
                            Arduino1/Master   |   Arduino2 Slave
  Slave Select/Chip Select: SS/CS (PB2)           SS/SC (PB2)
  Master Out Slave In:      MOSI  (PB3)           MOSI  (PB3)
  Master In Slave Out:      MISO  (PB4)           MISO  (PB4)
  Source Clock:             SCK   (PB5)           SCK   (PB5)
  Tie GND together!         GND                   GND

  SPI Order of Operations:
  It's a 4 step process
  1. Pull the Slave/Chip select LOW
  2. Send the register to the chip
  3. Send the value to the chip
  4. Pull the Slave/Chip select HIGH

  TODO: Have a LED and a buton on both Arduinos. Have the button on master turn on the led on slave
  and have the reverse true as well. Can just turn the button on for a few seconds when pressed.

*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define SS      PORTB2
#define MOSI    PORTB3
#define MISO    PORTB4 
#define SCK     PORTB5

#define ACK     0x7E //Custom value that I have chosen to replay with on the slave side when data is recieved

//Initialize SPI Master Device
void Spi_MasterInit(void)
{
	//set MOSI, SCK and SS as output
	DDRB |= (1<<MOSI) | (1<<SCK) | (1<<SS);
	//set SS to high
	PORTB |= (1<<SS);
	//enable master SPI at clock rate Fck/16 = 1Mhz
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

//Function to send data
void SPIMasterSend(uint8_t data)
{
  //select slave and drive low
  PORTB &= ~(1<<SS);

  //send data
  SPDR = data;
  //wait for transmition complete
  while (!(SPSR &(1<<SPIF)));

  //SS to high
  PORTB |= (1<<SS);
}

//Function to send and receive data
unsigned char Spi_Tranceiver (unsigned char data)
{
  //select slave and drive low
  PORTB &= ~(1<<SS);

  //send data
  SPDR = data;
  //wait for transmition complete
  while (!(SPSR &(1<<SPIF)));

  //SS to high
  PORTB |= (1<<SS);

  return(SPDR); //Return received data
}

//Function to blink LED
void led_blink (uint16_t i)
{
  //Blink LED "i" number of times
  for (; i>0; --i)
  {
    PORTD |= (1<<PORTD0);
    _delay_ms(100);
  }
}

int main(void)
{
  Spi_MasterInit(); //Initialize SPI Maste
  DDRD |= (1<<PORTD0); //PD0 as Output
  unsigned char data; //Received data stored
  uint8_t x = 0; //Counter value which

  while(1)
  {
    data = 0x00; //Reset ACK in "data"
    data = Spi_Tranceiver(++x); //Send "x", receive ACK

    //We have the Slave sending and ACK back when it recieves data
    if(data == ACK)
    { 
      //If received data is the same as ACK, blink LED "
      led_blink(x);
    }
    else
    {
      //If received data is not ACK, then blink LED for
      led_blink(1000);
    }

    _delay_ms(500); //Wait
  }

  return 0;
}