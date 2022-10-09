/* The Project will connect two Arduino Uno (atmega328p) over SPI Communication
  One of the Arduinos will be programmed in SPI Master Mode
  and the second Arduino will be programmed in SPI Slave Mode

  The following code is for the SPI Slave Mode, check out the file
  labeled ArduinoSPIMaster to see the SPI Master side code. 

  Arduino Wiring:
                            Arduino1/Master   |   Arduino2 Slave
  Slave Select/Chip Select: SS/CS (PB2)           SS/SC (PB2)
  Master Out Slave In:      MOSI  (PB3)           MOSI  (PB3)
  Master In Slave Out:      MISO  (PB4)           MISO  (PB4)
  Source Clock:             SCK   (PB5)           SCK   (PB5)
  Tie GND together!         GND                   GND

  LED Output:   PC1

  SPI Order of Operations:
  It's a 4 step process
  1. Wait for Pull the Slave/Chip select LOW from SPI Master
  2. Receive the register to the chip
  3. Receive the value to the chip
  4. Once Pull the Slave/Chip select HIGH then no more data to receive

  MY OWN SPI Between Two Arduinos (Slave Write/Read):
  *I do not look for Slave/Chip select to go low/high or receive a register value and I send an ACK to the SPI Master
  *I am not entirely sure if I would even look for Slave/Chip select to go high/low or if all of that is handled in the
   microcontroller SPI system.
  1. Wait to receive data
  2. Receive the value to the chip
  3. Send ACK to the Arduino SPI Master

  The Arduino SPI Master will send SPI data to the Arduino SPI Slave once when the button is pressed. 
  The Arduino SPI Master will blink an LED fast if the data ACK is recieved from the Arduino SPI Slave
  and blink once very slowly if the ACK recieved is bad.

  The Arduino SPI Slave will recieve the data sent from the Arduino SPI Master and if it is equal to the
  expected value then the LED will blink fast. If the data is not correct then the LED will blink slow.
  Extra: The Arduino SPI Slave will watch the SS line and wait for it to go low then it will look for data
  to be sent from the Arduino SPI Master.

  If a SPI data transmission happens properly, you will see both LEDs (Master LED and Slave LED)
  both blink fast 5 times at the same time.

  This project DOES NOT USE the SPI Interrupt
  If one is implemented then the following needs to be kept in mind:
  1. #include <avr/interrupt.h>
  2. SPCR |= (1<<SPIE) //to enable interrupt
  3. sei() //to enable global interrupts
  4. //SPI Transmission/reception complete ISR
      ISR(SPI_STC_vect)
      {
        //read/set SPDR value
      }
*/

////////////////////////////////////////////////
// SPI Slave Polling Method
////////////////////////////////////////////////
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define SS      PORTB2
#define MOSI    PORTB3
#define MISO    PORTB4 
#define SCK     PORTB5

#define ACK     0x7E //Custom value we replay with on the slave side when data is recieved
#define DATA    0xFF //Custom value we are expecting to recieve from the Master

void Led_Init(void)
{
  DDRC |= (1<<PORTC1);
}

void Led_Blink_Slow(void)
{
  PORTC |= (1<<PORTC1);
  _delay_ms(500);
  PORTC &= ~(1<<PORTC1);
  _delay_ms(500);
}

void Led_Blink_Fast(void)
{
  for (uint8_t i=0; i<=4; i++)
    {
        PORTC |= (1<<PORTC1);
        _delay_ms(20); //~0.1s delay
        PORTC &= ~(1<<PORTC1);
        _delay_ms(80);
    }
}

//Initialize SPI Slave Device
void SPISlaveInit(void)
{
	//set MISO as output, the rest default to input
	DDRB |= (1<<MISO);

	//enable SPI in slave mode
	SPCR = (1<<SPE);
}

//Function to send and receive data
unsigned char Spi_Tranceiver (unsigned char data)
{
  //send data
  SPDR = data;
  //wait for transmition complete
  while (!(SPSR &(1<<SPIF)));

  return(SPDR); //Return received data
}

int main(void)
{
  Led_Init();
  SPISlaveInit(); //Initialize SPI Slave
  
  unsigned char dataRec = 0; //Received data stored

  while(1)
  {
    dataRec = Spi_Tranceiver(ACK);
    // dataRec = Spi_Tranceiver(0x00);//Uncomment and comment out the above line to try sending ACK the master isnt looking for

    //Check the data we recieved
    if(dataRec == 0xFF)
    {
      Led_Blink_Fast();
    }
    else
    {
      Led_Blink_Slow();
    }
  }

  return 0;
}