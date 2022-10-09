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

  Button Input: PC0
  LED Output:   PC1

  Normal SPI Order of Operations:
  It's a 4 step process
  1. Pull the Slave/Chip select LOW
  2. Send the register to the chip
  3. Send the value to the chip
  4. Pull the Slave/Chip select HIGH

  MY OWN SPI Between Two Arduinos (Master Write/Read):
  *I do not send a register value and I added an ACK from the Slave
  1. Pull the Slave/Chip select LOW
  2. Send the value to the chip
  3. Pull the Slave/Chip select HIGH
  4. Check ACK recieved from Arduino SPI Slave

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
// SPI Master Polling Method
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

void Button_Init(void)
{
  DDRC &= (~(1<<PORTC0));
  PORTC |= (1<<PORTC0); //setting pull up resistors
}

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

//Initialize SPI Master Device
void Spi_MasterInit(void)
{
	//set MOSI, SCK and SS as output, the rest default to input
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

int main(void)
{
  Button_Init();
  Led_Init();
  Spi_MasterInit(); //Initialize SPI Master
  
  unsigned char dataRec; //Received data stored

  while(1)
  {
    uint8_t buttonStatus = PINC & (1<<PORTC0);
    uint8_t buttonReset; //Make only a single transmission per button click

    if((buttonStatus == 0 ) & (buttonReset == 0)) //Button pressed
    {
      buttonReset = 1;
      dataRec = 0x00; //Reset ACK in "data"

      dataRec = Spi_Tranceiver(DATA); //Send data, receive ACK
      // dataRec = Spi_Tranceiver(0x00); //Uncomment and comment out the above line to try sending data the slave isnt looking for

      //We have the Slave sending an ACK back when it recieves data
      if(dataRec == ACK)
      { 
        //If received data is the same as ACK, blink LED Fast
        Led_Blink_Fast();
      }
      else
      {
        //If received data is not ACK, then blink LED Slow
        //This will happen if you send data and the slave doesnt recieve it
        //This can also happen if the slow delays on the slave led blink are in process and you try to send more data (interrupts would solve this)
        Led_Blink_Slow();
      }
    }

    if(buttonStatus == 1 ) //Button not pressed
    {
      buttonReset = 0;
    }
  }

  return 0;
}