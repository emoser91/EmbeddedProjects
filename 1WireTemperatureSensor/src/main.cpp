/* This project will read from a DS18B20 Temperature sensor with 1-Wire Serial Communication Protocol

  This project will print out temperature readings in degrees C.

  I found an amazing example and used the code from it to build this project. The full code is in this folder.
  This code just calls a simple read function that ignores the device address.
  Normally you would have to do a complete search for your 1wire device as each device has its own unique 64bit ID
  I also stripped out any other unused functions to help keep the file more readable.

  What is Maxim 1-wire technology:
  The basis of 1-Wire technology is a serial protocol using a single data line plus ground reference for 
  communication. A 1-Wire master initiates and controls the communication with one or more 1-Wire slave 
  devices on the 1-Wire bus. Each 1-Wire slave device has a unique, unalterable, factory-programmed, 
  64-bit ID (identification number), which serves as device address on the 1-Wire bus. The 8-bit family 
  code, a subset of the 64-bit ID, identifies the device type and functionality. Typically, 1-Wire slave 
  devices operate over the voltage range of 2.8V (min) to 5.25V (max). Most 1-Wire devices have no pin 
  for power supply; they take their energy from the 1-Wire bus (parasitic supply).

  1-Wire Standard Operations:
  * Write 1 bit: Drive bus low, delay 6us, release bus, delay 64us
  * Write 0 bit: Drive bus low, delay 60us, release bus, delay 10us
  * Read bit: Drive bus low, delay 6us, release bus, delay 9us, sample bus to read bit from slave, delay 55us
  * Reset: Reset the 1Wire bus slave and ready for a command: delay 0us, drive bus low, delay 480us, release bus,
    delay 70us, sample bus, 0=device(s) present, 1= no device present, delay 410us
  Note: All delay values are taken from 1Wire Timing spec in folder
  Note: There is also an Overdrive Mode that has different delay times and bit rates

  Process to reading temp data from DS28B20:
  Initalization:
  1. Send a Reset Pulse (Saying is anyone out there?)
  2. Check for Presence Pulse: device drove line low after reset pulse to say they are there and ready for a command
  ROM Commands:
  3. Issue a Skip ROM command 0xCC to allow access to the 1wire device without providing 64bit ROM ID
     *To read ROM ID of one device on the line, send Read ROM command 0x33 and 1Wire device will respond with its 
       8bit family code, 48bit serial number and 8bit crc
     *To talk to one device when many are on the line, send a Match ROM command 0x55 followed by 64bit ROM ID
     *To find ROM IDs you can use the Search ROM command 0xF0
  DS18B20 Function Commands:
  4. Send Convert command 0x44 to initate a single temperature conversion
  4. Send Read Scratchpad command 0xBE
  5. Start reading 1Wire line and fill our temperature buffer with data
     * You have to watch the line and decide if 1 or 0 being recieved then store it
  6. Process collected data so we can print to USART for USB serial monitor
  Note: My version just uses the Skip ROM function and the full example does the entire Search ROM system.

  DS18B20 Wiring (breakout board):
  VCC to 5v
  DAT to Arduino Pin PD3/Digital3
  GND to GND
*/

// AVR
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// C
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////////////////////
//  Contents used from pindef.h
//
/////////////////////////////////////////////////////////////////////////////////////////////

typedef struct gpin_t {
    // Pointers to PORT and PIN and DDR registers
    volatile uint8_t *port;
    volatile uint8_t *pin;
    volatile uint8_t *ddr;

    // Bit number in PORT
    uint8_t bit;
} gpin_t;

void gset_input_pullup(const gpin_t* pin);
void gset_input_hiz(const gpin_t* pin);
void gset_output(const gpin_t* pin);
void gset_output_high(const gpin_t* pin);
void gset_output_low(const gpin_t* pin);
void gset_bit(const gpin_t* pin);
void gclear_bit(const gpin_t* pin);
uint8_t gread_bit(const gpin_t* pin);

/////////////////////////////////////////////////////////////////////////////////////////////
//  Contents used from pindef.c
//  Just a clean way to set bits high or low (g to global)
/////////////////////////////////////////////////////////////////////////////////////////////

void gset_output(const gpin_t* pin) {
    *(pin->ddr) |= (1 << (pin->bit));
}

void gset_output_high(const gpin_t* pin) {
    *(pin->port) |= (1 << (pin->bit));
}

void gset_output_low(const gpin_t* pin) {
    *(pin->port) &= ~(1 << (pin->bit));
}

void gset_bit(const gpin_t* pin) {
    *(pin->port) |= (1 << (pin->bit));
}

void gclear_bit(const gpin_t* pin) {
    *(pin->port) &= ~(1 << (pin->bit));
}

uint8_t gread_bit(const gpin_t* pin) {
    return *(pin->pin) & (1 << (pin->bit));
}

void gset_input_pullup(const gpin_t* pin) {
    *(pin->ddr) &= ~(1 << (pin->bit));
    gset_output_high(pin);
}

void gset_input_hiz(const gpin_t* pin) {
    *(pin->ddr) &= ~(1 << (pin->bit));
    gset_output_low(pin);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  Contents used from crc.c
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include <util/crc16.h>

uint8_t crc8(uint8_t* data, uint8_t len) {
    uint8_t crc = 0;

    for (uint8_t i = 0; i < len; ++i) {
        crc = _crc_ibutton_update(crc, data[i]);
    }

    return crc;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  Contents used from onewire.h
//
/////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Send a reset pulse
 *
 * Returns true if One Wire devices were detected on the bus
 */
bool onewire_reset(const gpin_t* io);

/**
 * Write a byte as described in Maxim's One Wire protocol
 */
void onewire_write(const gpin_t* io, uint8_t byte);

/**
 * Read a byte as described in Maxim's One Wire protocol
 */
uint8_t onewire_read(const gpin_t* io);

/**
 * Skip sending a device address
 */
void onewire_skiprom(const gpin_t* io);

// Special return values
static const uint16_t kDS18B20_DeviceNotFound = 0xA800;
static const uint16_t kDS18B20_CrcCheckFailed = 0x5000;

/**
 * Trigger all devices on the bus to perform a temperature reading
 * This returns immedidately, but callers must wait for conversion on slaves (max 750ms)
 */
void ds18b20_convert(const gpin_t* io);

/**
 * Read the last temperature conversion from the only probe on the bus
 *
 * If there is a single slave on the one-wire bus the temperature data can be
 * retrieved without scanning for and targeting device addresses.
 *
 * Calling this with more than one slave on the bus will cause data collision.
 */
uint16_t ds18b20_read_single(const gpin_t* io);


/////////////////////////////////////////////////////////////////////////////////////////////
//  Contents used from onewire.c
//
/////////////////////////////////////////////////////////////////////////////////////////////

bool onewire_reset(const gpin_t* io)
{
    // Configure for output
    gset_output_high(io);
    gset_output(io);

    // Pull low for >480uS (master reset pulse)
    gset_output_low(io);
    _delay_us(480);

    // Configure for input
    gset_input_hiz(io);
    _delay_us(70);

    // Look for the line pulled low by a slave
    uint8_t result = gread_bit(io);

    // Wait for the presence pulse to finish
    // This should be less than 240uS, but the master is expected to stay
    // in Rx mode for a minimum of 480uS in total
    _delay_us(460);

    return result == 0;
}

/**
 * Output a Write-0 or Write-1 slot on the One Wire bus
 * A Write-1 slot is generated unless the passed value is zero
 */
static void onewire_write_bit(const gpin_t* io, uint8_t bit)
{
    if (bit != 0) { // Write high

        // Pull low for less than 15uS to write a high
        gset_output_low(io);
        _delay_us(5);
        gset_output_high(io);

        // Wait for the rest of the minimum slot time
        _delay_us(55);

    } else { // Write low

        // Pull low for 60 - 120uS to write a low
        gset_output_low(io);
        _delay_us(55);

        // Stop pulling down line
        gset_output_high(io);

        // Recovery time between slots
        _delay_us(5);
    }
}

// One Wire timing is based on this Maxim application note
// https://www.maximintegrated.com/en/app-notes/index.mvp/id/126
void onewire_write(const gpin_t* io, uint8_t byte)
{
    // Configure for output
    gset_output_high(io);
    gset_output(io);

    for (uint8_t i = 8; i != 0; --i) {

        onewire_write_bit(io, byte & 0x1);

        // Next bit (LSB first)
        byte >>= 1;
    }
}

/**
 * Generate a read slot on the One Wire bus and return the bit value
 * Return 0x0 or 0x1
 */
static uint8_t onewire_read_bit(const gpin_t* io)
{
    // Pull the 1-wire bus low for >1uS to generate a read slot
    gset_output_low(io);
    gset_output(io);
    _delay_us(1);

    // Configure for reading (releases the line)
    gset_input_hiz(io);

    // Wait for value to stabilise (bit must be read within 15uS of read slot)
    _delay_us(10);

    uint8_t result = gread_bit(io) != 0;

    // Wait for the end of the read slot
    _delay_us(50);

    return result;
}

uint8_t onewire_read(const gpin_t* io)
{
    uint8_t buffer = 0x0;

    // Configure for input
    gset_input_hiz(io);

    // Read 8 bits (LSB first)
    for (uint8_t bit = 0x01; bit; bit <<= 1) {

        // Copy read bit to least significant bit of buffer
        if (onewire_read_bit(io)) {
            buffer |= bit;
        }
    }

    return buffer;
}

void onewire_skiprom(const gpin_t* io)
{
    onewire_write(io, 0xCC);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  Contents used from ds18b20.c
//
/////////////////////////////////////////////////////////////////////////////////////////////

// Command bytes
static const uint8_t kConvertCommand = 0x44;
static const uint8_t kReadScatchPad = 0xBE;

// Scratch pad data indexes
static const uint8_t kScratchPad_tempLSB = 0;
static const uint8_t kScratchPad_tempMSB = 1;
static const uint8_t kScratchPad_crc = 8;

static uint16_t ds18b20_readScratchPad(const gpin_t* io)
{
    // Read scratchpad into buffer (LSB byte first)
    static const int8_t kScratchPadLength = 9;
    uint8_t buffer[kScratchPadLength];

    for (int8_t i = 0; i < kScratchPadLength; ++i) {
        buffer[i] = onewire_read(io);
    }

    // Check the CRC (9th byte) against the 8 bytes of data
    if (crc8(buffer, 8) != buffer[kScratchPad_crc]) {
        return kDS18B20_CrcCheckFailed;
    }

    // Return the raw 9 to 12-bit temperature value
    return (buffer[kScratchPad_tempMSB] << 8) | buffer[kScratchPad_tempLSB];
}

uint16_t ds18b20_read_single(const gpin_t* io)
{
    // Confirm the device is still alive. Abort if no reply
    if (!onewire_reset(io)) {
        return kDS18B20_DeviceNotFound;
    }

    // Reading a single device, so skip sending a device address
    onewire_skiprom(io); //Send SKIP ROM command 0xCC
    onewire_write(io, kReadScatchPad); //Send Read Scratchpad command 0xBE

    // Read the data from the scratch pad
    return ds18b20_readScratchPad(io);
}

void ds18b20_convert(const gpin_t* io)
{
    // Send convert command to all devices (this has no response)
    onewire_skiprom(io);
    onewire_write(io, kConvertCommand);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Int Main
//
// Program will read the DS18B20 Temperature sensor with 1-wire protocol and output the
// result through USART to USB that can then be read through a serial monitor.
// Serial Monitor Settings: baud: 9600, data bits: 8, stop bits: 1, COM: Arduino Com port
//
// I used my own USART init from the USART project I did because the example project
// USART wasnt working for me.
/////////////////////////////////////////////////////////////////////////////////////////////
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


void init_usart(void)
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

/* Send a single byte out over serial (blocking) */
void usart_transmit(uint8_t data)
{
    // Wait for empty transmit buffer
    while ( !( UCSR0A & (1<<UDRE0)) );

    // Put data into buffer, sends the data
    UDR0 = data;
}

/* Block until a single byte is received over serial */
uint8_t usart_receive(void)
{
    // Wait for data to be received
    while ( !(UCSR0A & (1<<RXC0)) );

    // Get and return received data from buffer
    return UDR0;
}

/* Print a null-terminated string */
void print(char* string)
{
    uint8_t i = 0;
    while (string[i] != '\0') {
        usart_transmit(string[i]);
        ++i;
    }
}

/* Print a null-terminated string followed by an automatic line break */
void println(char* string)
{
    print(string);
    usart_transmit('\n\r');
}

/**
 * Convert a fixed-point 12.4 number into a floating point string
 * This is creates a human-readable string for temperature values from a DS18B20
 *
 * The passed buffer must be able to accomodate up to 9 characters plus a null terminator.
 */
void fptoa(uint16_t value, char* buf, uint8_t bufsize)
{
    const uint16_t integer = (value >> 4);
    const uint16_t decimal = (value & 0x0F) * 625; // One 16th is 0.625

    memset(buf, '\0', bufsize);

    // Convert the integer portion of the number to a string
    itoa(integer, buf, 10);

    // Add decimal point
    buf += strlen((char*)buf);
    (*buf++) = '.';

    if (decimal == 0) {
      // Fill zeros to maintain fixed decimal places
      memset(buf, '0', 4);
      return;
    }

    if (decimal < 1000) {
      // Add a leading zero for .0625
      (*buf++) = '0';
    }

    // Convert the decimal portion of the number to a string
    itoa(decimal, buf, 10);
}

// Declare the Pin location of our Temperature Sensor: PD3
const static gpin_t sensorPin = { &PORTD, &PIND, &DDRD, PD3 };

int main(void)
{
    init_usart();

    while(1) 
    {
        // Send Reset command and check that our device is alive
        if (!onewire_reset(&sensorPin)) {
            println("Nothing on the bus?");
            _delay_ms(1000);
            continue;
        }

        // Send Skip ROM command and Send Convert command to intiate temperature conversion
        ds18b20_convert(&sensorPin);
        _delay_ms(750); // Wait for it to finish

        // Send Skip ROM command and Send Read Scratchpad command to read temperature value
        uint16_t reading = ds18b20_read_single(&sensorPin);

        // Check CRC on Data Reading
        if (reading != kDS18B20_CrcCheckFailed)
        {
            // Process collected data: Convert fixed-point to a printable string
            char buf[10];
            fptoa(reading, buf, sizeof(buf));
            // Print out temperature value to USART serial line
            print(buf);
        } 
        else 
        {
            println("Bad temp CRC");
        }

        // New line
        print("\r\n");
    }

    return 0;
}