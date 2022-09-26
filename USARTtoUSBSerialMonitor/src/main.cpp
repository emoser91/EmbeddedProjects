/* This project is a test of Serial Monitoring
   USART to USB

   Baud rate: 9600
   Mode: Asynchronous, normal speed
   Parity: Disabled
   Start bits: 1
   Data bits: 8

   Please see project named testSerialMonitor to simply test out serial monitoring
   This project will be getting my own UART setup and communicating

   There are multiple examples of USART use cases below. All cases use the same #includes
   and all you need to do is to uncomment the desired section to test it out.

   We are using the Arduino Sketch style code for an inital test
   This same code can be copied and pasted into Arduino IDE and will work

   Once code is loaded, you can click on the PlatformIO: Serial Monitor button (plug button)
   to open a serial communication to the COM PORT associated with the Arduino Uno connected.

   Note: If the Arduino is power cycled or communication inturrupted in any way, a new Serial Terminal will need to be opened. 

   You can also open Putty to open your own Serial monitoring to the Arduino associated COM PORT
   Keep in mind that only one program can be connected to the COM PORT for communication at a time
   ie. Only the PlatformIO or the Putty Terminal can be open, not both.

   If you want to send characters to the Arduino through Putty, you will need to select the Terminal Settings
   and set both discipline options to "Force on" and you will be able to input characters

   You can also use the Arduino IDE Serial Monitor the same as the other two above mentioned options.

   Wiring:
   We only need the USB to program and power the Arduino connected to the PC
   Check to see what COM PORT the Arduino is conencted to under Device Manager in Windows

   USING AN EXERNAL TTL(USART) to USB DEVICE
   You can also use an external ttl to usb device to communicate with the arduino.
   The Arduino just has one USART subsystem so when you are watching the USB line powering the Arduino, 
   you will recieve the same data as using the TX/RX lines with an external device.

   Wiring:
   *Make sure to cross TX/RX lines
   USB to TLL   |   Arduino Uno
        5v              5v
        GND             GND
        TX              RX
        RX              TX
   
   See attached documents in the documents in the same folder as this file to see more information on USART
   See LCD Project for information on sending different types of data
   The resouce attached named "Datatype Examples" might also be useful for sending different types of data

   ISSUE: I ran into an issue where none of the example code I would find would work, even the examples in the
   datasheet. I figured out that U2X0 was set to 1 by default putting the USART in double speed mode which would
   change the calculation needed for UBRR. I simply forced U2X0 to 0 and all examples would then work.
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <MSOE/delay.c>
#include <MSOE/bit.c>
// #include <MSOE/lcd.c>
// #include <MSOE/uart.c> //Has the above mentioned issue.
#include <Arduino.h>

//////////////////////////////////////////////////////////////////////////
// Starting USART and transmitting using the Arduino IDE defined functions
//
/////////////////////////////////////////////////////////////////////////
// int main()
// {
//   Serial.begin(9600);

// 	// USART_Init();
// 	while (1)
// 	{
//     Serial.println("\nStarting Serial Monitor");
// 	   delay_ms(1000);
// 	}
// 	return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////
// Polling Transmition Method (easiest method)
//
// Polling transmission is the simplest method of transmission where the application 
// software keeps monitoring the status of the USART transmitter hardware and loads a 
// byte of data into the USART buffer UDR0 only when the hardware is ready for transmission. 
// This wastes CPU time in constantly monitoring the status of UDRE0 bit of UCSR0A register.
////////////////////////////////////////////////////////////////////////////////////////////////
// #define USART_BAUDRATE 9600 // Desired Baud Rate

// #define NORMAL_SPEED 16UL //Asynchronous normal speed mode when U2X0=0
// #define DOUBLE_SPEED 8UL //Asynchronous double speed mode when U2X0=1
// #define SPEED_MODE NORMAL_SPEED

// #define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * SPEED_MODE))) - 1)

// #define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection UMSEL01=0, UMSEL00=0

// #define DISABLED    (0<<UPM00) // UPM01=0, UPM00=0
// #define EVEN_PARITY (2<<UPM00) // UPM01=1, UPM00=0
// #define ODD_PARITY  (3<<UPM00) // UPM01=1, UPM00=1
// #define PARITY_MODE  DISABLED // USART Parity Bit Selection

// #define ONE_BIT (0<<USBS0)
// #define TWO_BIT (1<<USBS0)
// #define STOP_BIT ONE_BIT      // USART Stop Bit Selection

// #define FIVE_BIT  (0<<UCSZ00) // UCSZ01=0, UCSZ00=0 (UCSZ02=0 by default)
// #define SIX_BIT   (1<<UCSZ00) // UCSZ01=0, UCSZ00=1
// #define SEVEN_BIT (2<<UCSZ00) // UCSZ01=1, UCSZ00=0
// #define EIGHT_BIT (3<<UCSZ00) // UCSZ01=1, UCSZ00=1
// #define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

// // Initialize the USART
// void USART_Init(void)
// {
//     // Set the baud prescaler
//     UBRR0H = (unsigned char) (USART_BAUD_PRESCALER >> 8);
//     UBRR0L = (unsigned char) USART_BAUD_PRESCALER;

//     // Set the frame size of the receiver and transmitter to 8-bits
//     UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;

//     // Set U2X0 for double speed operation
//     // Arduino defalut U2X0=1 so we will change it
//     UCSR0A = ~(1<<U2X0); //U2X0=0 for Asynchronous normal speed mode

//     // Enable the transmitter and the rceiver
//     UCSR0B = ((1 << TXEN0) | (1 << RXEN0));
// }

// // Transmit data over USART
// void USART_Transmit(unsigned char transmit_data)
// {
//     // Wait for empty transmit buffer then transmit data
//     while (( UCSR0A & (1<<UDRE0)) == 0) {};
//     // while (!(UCSR0A & (1 << UDRE0)));
//     UDR0 = transmit_data;
// }

// int main()
// {
//     USART_Init();
//     while (1)
//     {
//         USART_Transmit('a');
//         delay_ms(1000);
//     }
// }


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Polling Reception
//
// Polling reception is the simplest method of reception where the application software keeps 
// monitoring the status of the USART receiver hardware and reads the data from the USART buffer 
// UDR0 only when the hardware has received a byte of data. This wastes CPU time in constantly 
// monitoring the status of RXC0 bit of UCSR0A register.
//
// The below code waits for the user input. If the serial input is ‘a’ it glows the D13 LED on 
// the Arduino UNO board.
//
// If using Putty, set to serial, pick port, baud at 9600, under Terminal settings select
// both line discipline options to "Force on" and you will be able to input characters
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define USART_BAUDRATE 9600 // Desired Baud Rate

// #define NORMAL_SPEED 16UL //Asynchronous normal speed mode when U2X0=0
// #define DOUBLE_SPEED 8UL //Asynchronous double speed mode when U2X0=1
// #define SPEED_MODE NORMAL_SPEED

// #define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * SPEED_MODE))) - 1)

// #define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection UMSEL01=0, UMSEL00=0

// #define DISABLED    (0<<UPM00) // UPM01=0, UPM00=0
// #define EVEN_PARITY (2<<UPM00) // UPM01=1, UPM00=0
// #define ODD_PARITY  (3<<UPM00) // UPM01=1, UPM00=1
// #define PARITY_MODE  DISABLED // USART Parity Bit Selection

// #define ONE_BIT (0<<USBS0)
// #define TWO_BIT (1<<USBS0)
// #define STOP_BIT ONE_BIT      // USART Stop Bit Selection

// #define FIVE_BIT  (0<<UCSZ00) // UCSZ01=0, UCSZ00=0 (UCSZ02=0 by default)
// #define SIX_BIT   (1<<UCSZ00) // UCSZ01=0, UCSZ00=1
// #define SEVEN_BIT (2<<UCSZ00) // UCSZ01=1, UCSZ00=0
// #define EIGHT_BIT (3<<UCSZ00) // UCSZ01=1, UCSZ00=1
// #define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

// void USART_Init()
// {
// 	// Set Baud Rate
// 	UBRR0H = USART_BAUD_PRESCALER >> 8;
// 	UBRR0L = USART_BAUD_PRESCALER;
	
// 	// Set Frame Format
// 	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
	
//     // Set U2X0 for double speed operation
//     // Arduino defalut U2X0=1 so we will change it
//     UCSR0A = ~(1<<U2X0); //U2X0=0 for Asynchronous normal speed mode

// 	// Enable Receiver and Transmitter
// 	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
// }

// uint8_t USART_ReceivePolling()
// {
// 	uint8_t DataByte;
// 	while (( UCSR0A & (1<<RXC0)) == 0) {}; // Do nothing until data have been received
// 	DataByte = UDR0 ;
// 	return DataByte;
// }

// int main()
// {
// 	DDRB |= (1 << PORTB5); // Configuring PB5 / D13 as Output
// 	USART_Init();
// 	char LocalData;
// 	while (1)
// 	{
// 		LocalData = USART_ReceivePolling();
// 		if (LocalData == 'a')
// 		{
// 			PORTB |= (1 << PORTB5); // Writing HIGH to glow LED
// 		}
// 		else
// 		{
// 			PORTB &= ~(1 << PORTB5); // Writing LOW
// 		}
// 		delay_ms(1000);
// 	}
// 	return 0;
// }


////////////////////////////////////////////////////////////////////////////////////////////////
// Polling Loopback
//
// The loopback test is a great way to verify any communication channel. A loopback test of 
// USART will verify both the reception and transmission side of the code. A loopback test sends 
// back the same data that is received. The below code will echo back the same character 
// that is sent from the serial terminal.
//
// If using Putty, set to serial, pick port, baud at 9600, under Terminal settings select
// both line discipline options to "Force on" and you will be able to input characters
////////////////////////////////////////////////////////////////////////////////////////////////
// #define USART_BAUDRATE 9600 // Desired Baud Rate

// #define NORMAL_SPEED 16UL //Asynchronous normal speed mode when U2X0=0
// #define DOUBLE_SPEED 8UL //Asynchronous double speed mode when U2X0=1
// #define SPEED_MODE NORMAL_SPEED

// #define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * SPEED_MODE))) - 1)

// #define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection UMSEL01=0, UMSEL00=0

// #define DISABLED    (0<<UPM00) // UPM01=0, UPM00=0
// #define EVEN_PARITY (2<<UPM00) // UPM01=1, UPM00=0
// #define ODD_PARITY  (3<<UPM00) // UPM01=1, UPM00=1
// #define PARITY_MODE  DISABLED // USART Parity Bit Selection

// #define ONE_BIT (0<<USBS0)
// #define TWO_BIT (1<<USBS0)
// #define STOP_BIT ONE_BIT      // USART Stop Bit Selection

// #define FIVE_BIT  (0<<UCSZ00) // UCSZ01=0, UCSZ00=0 (UCSZ02=0 by default)
// #define SIX_BIT   (1<<UCSZ00) // UCSZ01=0, UCSZ00=1
// #define SEVEN_BIT (2<<UCSZ00) // UCSZ01=1, UCSZ00=0
// #define EIGHT_BIT (3<<UCSZ00) // UCSZ01=1, UCSZ00=1
// #define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

// void USART_Init()
// {
// 	// Set Baud Rate
// 	UBRR0H = (unsigned char) (USART_BAUD_PRESCALER >> 8);
//     UBRR0L = (unsigned char) USART_BAUD_PRESCALER;
	
// 	// Set Frame Format
// 	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;

//     // Set U2X0 for double speed operation
//     // Arduino defalut U2X0=1 so we will change it
//     UCSR0A = ~(1<<U2X0); //U2X0=0 for Asynchronous normal speed mode
	
// 	// Enable Receiver and Transmitter
// 	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
// }

// uint8_t USART_ReceivePolling()
// {
// 	uint8_t DataByte;
//     while (!(UCSR0A & (1 << UDRE0)));
// 	while (( UCSR0A & (1<<RXC0)) == 0) {}; // Do nothing until data have been received
// 	DataByte = UDR0 ;
// 	return DataByte;
// }

// void USART_TransmitPolling(uint8_t DataByte)
// {
// 	while (( UCSR0A & (1<<UDRE0)) == 0) {};
// 	UDR0 = DataByte;
// }

// int main()
// {
// 	USART_Init();
// 	char LocalData;
// 	while (1)
// 	{
// 		LocalData = USART_ReceivePolling();
// 		USART_TransmitPolling(LocalData);
// 	}
// 	return 0;
// }


////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt Transmission
//
// In polling the CPU waste valuable time monitoring the USART registers. This valuable time 
// could be used in the execution of other instructions. This problem is solved by interrupt 
// based transmission. Below code transmits using interrupts. The code transmits the character 
// ‘a’ endlessly while the D13 LED on Arduino UNO keeps blinking. The CPU keeps performing the 
// LED blinking in an infinite loop and every time the transmission finishes an interrupt is 
// generated to state that the UDR0 buffer is ready to receive new data. The CPU pauses the 
// LED blinking and serves the ISR.
////////////////////////////////////////////////////////////////////////////////////////////////
// #define USART_BAUDRATE 9600 // Desired Baud Rate

// #define NORMAL_SPEED 16UL //Asynchronous normal speed mode when U2X0=0
// #define DOUBLE_SPEED 8UL //Asynchronous double speed mode when U2X0=1
// #define SPEED_MODE NORMAL_SPEED

// #define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * SPEED_MODE))) - 1)

// #define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection UMSEL01=0, UMSEL00=0

// #define DISABLED    (0<<UPM00) // UPM01=0, UPM00=0
// #define EVEN_PARITY (2<<UPM00) // UPM01=1, UPM00=0
// #define ODD_PARITY  (3<<UPM00) // UPM01=1, UPM00=1
// #define PARITY_MODE  DISABLED // USART Parity Bit Selection

// #define ONE_BIT (0<<USBS0)
// #define TWO_BIT (1<<USBS0)
// #define STOP_BIT ONE_BIT      // USART Stop Bit Selection

// #define FIVE_BIT  (0<<UCSZ00) // UCSZ01=0, UCSZ00=0 (UCSZ02=0 by default)
// #define SIX_BIT   (1<<UCSZ00) // UCSZ01=0, UCSZ00=1
// #define SEVEN_BIT (2<<UCSZ00) // UCSZ01=1, UCSZ00=0
// #define EIGHT_BIT (3<<UCSZ00) // UCSZ01=1, UCSZ00=1
// #define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

// #define RX_COMPLETE_INTERRUPT         (1<<RXCIE0)
// #define DATA_REGISTER_EMPTY_INTERRUPT (1<<UDRIE0)

// volatile uint8_t USART_TransmitBuffer; // Global Buffer

// void USART_Init()
// {
// 	// Set Baud Rate
// 	UBRR0H = USART_BAUD_PRESCALER >> 8;
// 	UBRR0L = USART_BAUD_PRESCALER;
	
// 	// Set Frame Format
// 	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;

//     // Set U2X0 for double speed operation
//     // Arduino defalut U2X0=1 so we will change it
//     UCSR0A = ~(1<<U2X0); //U2X0=0 for Asynchronous normal speed mode
	
// 	// Enable Receiver and Transmitter
// 	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
// 	//Enable Global Interrupts
// 	sei();
// }

// // Fill transmit buffer and enable interrupt
// void USART_TransmitInterrupt(uint8_t Buffer)
// {
// 	USART_TransmitBuffer = Buffer;
// 	UCSR0B |= DATA_REGISTER_EMPTY_INTERRUPT; // Enables the Interrupt, could be moved to USART_Init() if we never lower
// }

// int main()
// {
// 	DDRB |= (1 << PORTB5); // Configuring PB5 / D13 as Output
// 	uint8_t LocalData = 'a';
// 	USART_Init();
// 	USART_TransmitInterrupt(LocalData);
	
// 	while (1)
// 	{
// 		PORTB |= (1 << PORTB5); // Writing HIGH to glow LED
// 		delay_ms(500);
// 		PORTB &= ~(1 << PORTB5); // Writing LOW
// 		delay_ms(500);		
// 	}
	
// 	return 0;
// }

// // Called when Data Register is Empty
// ISR(USART_UDRE_vect)
// {
// 	UDR0 = USART_TransmitBuffer;
// 	//UCSR0B &= ~DATA_REGISTER_EMPTY_INTERRUPT; // Disables the Interrupt, uncomment for one time transmission of data
// }


////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt Reception
//
// Interrupt reception behaves exactly the same as polling reception but in the case of 
// interrupt reception. The CPU is busy looping an infinite loop and whenever data is received 
// in USART Buffer an interrupt is thrown and the CPU serves it and toggles the LED accordingly. 
// The CPU doesn’t have to monitor the USART register bits to check the status of the reception.
//
// If using Putty, set to serial, pick port, baud at 9600, under Terminal settings select
// both line discipline options to "Force on" and you will be able to input characters
////////////////////////////////////////////////////////////////////////////////////////////////
// #define USART_BAUDRATE 9600 // Desired Baud Rate

// #define NORMAL_SPEED 16UL //Asynchronous normal speed mode when U2X0=0
// #define DOUBLE_SPEED 8UL //Asynchronous double speed mode when U2X0=1
// #define SPEED_MODE NORMAL_SPEED

// #define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * SPEED_MODE))) - 1)

// #define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection UMSEL01=0, UMSEL00=0

// #define DISABLED    (0<<UPM00) // UPM01=0, UPM00=0
// #define EVEN_PARITY (2<<UPM00) // UPM01=1, UPM00=0
// #define ODD_PARITY  (3<<UPM00) // UPM01=1, UPM00=1
// #define PARITY_MODE  DISABLED // USART Parity Bit Selection

// #define ONE_BIT (0<<USBS0)
// #define TWO_BIT (1<<USBS0)
// #define STOP_BIT ONE_BIT      // USART Stop Bit Selection

// #define FIVE_BIT  (0<<UCSZ00) // UCSZ01=0, UCSZ00=0 (UCSZ02=0 by default)
// #define SIX_BIT   (1<<UCSZ00) // UCSZ01=0, UCSZ00=1
// #define SEVEN_BIT (2<<UCSZ00) // UCSZ01=1, UCSZ00=0
// #define EIGHT_BIT (3<<UCSZ00) // UCSZ01=1, UCSZ00=1
// #define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

// #define RX_COMPLETE_INTERRUPT         (1<<RXCIE0)
// #define DATA_REGISTER_EMPTY_INTERRUPT (1<<UDRIE0)

// volatile uint8_t USART_ReceiveBuffer; // Global Buffer

// void USART_Init()
// {
// 	// Set Baud Rate
// 	UBRR0H = USART_BAUD_PRESCALER >> 8;
// 	UBRR0L = USART_BAUD_PRESCALER;
	
// 	// Set Frame Format
// 	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;

//     // Set U2X0 for double speed operation
//     // Arduino defalut U2X0=1 so we will change it
//     UCSR0A = ~(1<<U2X0); //U2X0=0 for Asynchronous normal speed mode
	
// 	// Enable Receiver and Transmitter
// 	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
// 	//Enable Global Interrupts
// 	sei();
// }

// int main()
// {
// 	DDRB |= (1 << PORTB5); // Configuring PB5 / D13 as Output
// 	USART_Init();
// 	UCSR0B |= RX_COMPLETE_INTERRUPT; // Enables the Recieve Interrupt, We never lower so we could move this line to USART_Init()
// 	while (1)
// 	{
// 	}
// 	return 0;
// }

// // Called when USART data is recieved
// ISR(USART_RX_vect)
// {
// 	USART_ReceiveBuffer = UDR0;
//     // If 'a' character is recieved then turn on on-board LED
// 	if (USART_ReceiveBuffer == 'a')
// 	{
// 		PORTB |= (1 << PORTB5);    // Writing HIGH to glow LED
// 	}
// 	else
// 	{
// 		PORTB &= ~(1 << PORTB5); // Writing LOW
// 	}
// }


////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt Loopback
//
// The below example works exactly like polling loopback but here the CPU doesn’t waste 
// time in checking the status of the USART registers. The loopback test sends back the same 
// data that is received. The below code will echo back the same character that is sent 
// from the serial terminal.
//
// If using Putty, set to serial, pick port, baud at 9600, under Terminal settings select
// both line discipline options to "Force on" and you will be able to input characters
////////////////////////////////////////////////////////////////////////////////////////////////
#define USART_BAUDRATE 9600 // Desired Baud Rate

#define NORMAL_SPEED 16UL //Asynchronous normal speed mode when U2X0=0
#define DOUBLE_SPEED 8UL //Asynchronous double speed mode when U2X0=1
#define SPEED_MODE NORMAL_SPEED

#define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * SPEED_MODE))) - 1)

#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection UMSEL01=0, UMSEL00=0

#define DISABLED    (0<<UPM00) // UPM01=0, UPM00=0
#define EVEN_PARITY (2<<UPM00) // UPM01=1, UPM00=0
#define ODD_PARITY  (3<<UPM00) // UPM01=1, UPM00=1
#define PARITY_MODE  DISABLED // USART Parity Bit Selection

#define ONE_BIT (0<<USBS0)
#define TWO_BIT (1<<USBS0)
#define STOP_BIT ONE_BIT      // USART Stop Bit Selection

#define FIVE_BIT  (0<<UCSZ00) // UCSZ01=0, UCSZ00=0 (UCSZ02=0 by default)
#define SIX_BIT   (1<<UCSZ00) // UCSZ01=0, UCSZ00=1
#define SEVEN_BIT (2<<UCSZ00) // UCSZ01=1, UCSZ00=0
#define EIGHT_BIT (3<<UCSZ00) // UCSZ01=1, UCSZ00=1
#define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

#define RX_COMPLETE_INTERRUPT         (1<<RXCIE0)
#define DATA_REGISTER_EMPTY_INTERRUPT (1<<UDRIE0)

volatile uint8_t USART_ReceiveBuffer; // Global Buffer

void USART_Init()
{
	// Set Baud Rate
	UBRR0H = USART_BAUD_PRESCALER >> 8;
	UBRR0L = USART_BAUD_PRESCALER;
	
	// Set Frame Format
	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;

    // Set U2X0 for double speed operation
    // Arduino defalut U2X0=1 so we will change it
    UCSR0A = ~(1<<U2X0); //U2X0=0 for Asynchronous normal speed mode
	
	// Enable Receiver and Transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
	//Enable Global Interrupts
	sei();
}

int main()
{
	USART_Init();
	UCSR0B |= RX_COMPLETE_INTERRUPT; //We never lower so we could move this line to USART_Init()
	while (1)
	{
	}
	return 0;
}

ISR(USART_RX_vect)
{
	USART_ReceiveBuffer = UDR0;
	UDR0 = USART_ReceiveBuffer;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupt Transmission with transmission completion notification
//
// The code transmits the character ‘a’ once and lights up the D13 on-board LED once the 
// transmission is completed. This example is similar to the Interrupt Transmission example
// plus the use of the TX Complete Interrupt.
////////////////////////////////////////////////////////////////////////////////////////////////
// #define USART_BAUDRATE 9600 // Desired Baud Rate

// #define NORMAL_SPEED 16UL //Asynchronous normal speed mode when U2X0=0
// #define DOUBLE_SPEED 8UL //Asynchronous double speed mode when U2X0=1
// #define SPEED_MODE NORMAL_SPEED

// #define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * SPEED_MODE))) - 1)

// #define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection UMSEL01=0, UMSEL00=0

// #define DISABLED    (0<<UPM00) // UPM01=0, UPM00=0
// #define EVEN_PARITY (2<<UPM00) // UPM01=1, UPM00=0
// #define ODD_PARITY  (3<<UPM00) // UPM01=1, UPM00=1
// #define PARITY_MODE  DISABLED // USART Parity Bit Selection

// #define ONE_BIT (0<<USBS0)
// #define TWO_BIT (1<<USBS0)
// #define STOP_BIT ONE_BIT      // USART Stop Bit Selection

// #define FIVE_BIT  (0<<UCSZ00) // UCSZ01=0, UCSZ00=0 (UCSZ02=0 by default)
// #define SIX_BIT   (1<<UCSZ00) // UCSZ01=0, UCSZ00=1
// #define SEVEN_BIT (2<<UCSZ00) // UCSZ01=1, UCSZ00=0
// #define EIGHT_BIT (3<<UCSZ00) // UCSZ01=1, UCSZ00=1
// #define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

// #define TX_COMPLETE_INTERRUPT         (1<<TXCIE0)
// #define RX_COMPLETE_INTERRUPT         (1<<RXCIE0)
// #define DATA_REGISTER_EMPTY_INTERRUPT (1<<UDRIE0)

// volatile uint8_t USART_ReceiveBuffer; // Global Buffer
// volatile uint8_t USART_TransmitBuffer; // Global Buffer
// volatile uint8_t USART_TransmitComplete;

// void USART_Init()
// {
// 	// Set Baud Rate
// 	UBRR0H = USART_BAUD_PRESCALER >> 8;
// 	UBRR0L = USART_BAUD_PRESCALER;
	
// 	// Set Frame Format
// 	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;

//     // Set U2X0 for double speed operation
//     // Arduino defalut U2X0=1 so we will change it
//     UCSR0A = ~(1<<U2X0); //U2X0=0 for Asynchronous normal speed mode
	
// 	// Enable Receiver and Transmitter
// 	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
// 	//Enable Global Interrupts
// 	sei();
// }

// // Fill transmit buffer and enable interrupt
// void USART_TransmitInterrupt(uint8_t Buffer)
// {
// 	USART_TransmitBuffer = Buffer;
// 	UCSR0B |= DATA_REGISTER_EMPTY_INTERRUPT; // Enables the Interrupt, could be moved to USART_Init() if we never lower
// }

// int main()
// {
// 	DDRB |= (1 << PORTB5); // Configuring PB5 / D13 as Output
// 	uint8_t LocalData = 'a';
// 	USART_Init();
// 	USART_TransmitInterrupt(LocalData);
	
// 	while (1)
// 	{
// 		if (USART_TransmitComplete)
//         {
//             PORTB |= (1 << PORTB5);    // Writing HIGH to glow LED
//             delay_ms(1000);
//             USART_TransmitComplete = 0;
//         }
//         else
//         {
//             PORTB &= ~(1 << PORTB5); // Writing LOW
//         }		
// 	}
	
// 	return 0;
// }

// // Called when Data Register is Empty
// ISR(USART_UDRE_vect)
// {
// 	UDR0 = USART_TransmitBuffer;
// 	UCSR0B &= ~DATA_REGISTER_EMPTY_INTERRUPT; // Disables the Interrupt, uncomment for one time transmission of data

//     UCSR0B |= TX_COMPLETE_INTERRUPT;
// }

// // Call on transmit completion
// ISR(USART_TX_vect)
// {
// 	USART_TransmitComplete = 1;
//     UCSR0B &= ~TX_COMPLETE_INTERRUPT;
// }