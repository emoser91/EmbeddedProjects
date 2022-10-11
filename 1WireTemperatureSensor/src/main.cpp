// #include <ONEWIRE/ds18b20.h>
// #include <ONEWIRE/onewire.h>
// #include <ONEWIRE/pindef.h>

// AVR
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// C
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  pindef Functions
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Generic AVR port pin
 */
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

void gset_output(const gpin_t* pin) {
    *(pin->ddr) |= _BV(pin->bit);
}

void gset_output_high(const gpin_t* pin) {
    *(pin->port) |= _BV(pin->bit);
}

void gset_output_low(const gpin_t* pin) {
    *(pin->port) &= ~_BV(pin->bit);
}

void gset_bit(const gpin_t* pin) {
    *(pin->port) |= _BV(pin->bit);
}

void gclear_bit(const gpin_t* pin) {
    *(pin->port) &= ~_BV(pin->bit);
}

uint8_t gread_bit(const gpin_t* pin) {
    return *(pin->pin) & _BV(pin->bit);
}

void gset_input_pullup(const gpin_t* pin) {
    *(pin->ddr) &= ~_BV(pin->bit);
    gset_output_high(pin);
}

void gset_input_hiz(const gpin_t* pin) {
    *(pin->ddr) &= ~_BV(pin->bit);
    gset_output_low(pin);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  onewire Functions
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * State for the onewire_search function
 * This must be initialised with onewire_search_init() before use.
 */
typedef struct onewire_search_state {

    // The highest bit position where a bit was ambiguous and a zero was written
    int8_t lastZeroBranch;

    // Internal flag to indicate if the search is complete
    // This flag is set once there are no more branches to search
    bool done;

    // Discovered 64-bit device address (LSB first)
    // After a successful search, this contains the found device address.
    // During a search this is overwritten LSB-first with a new address.
    uint8_t address[8];

} onewire_search_state;

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

/**
 * Address a specific device
 */
void onewire_match_rom(const gpin_t* io, uint8_t* address);

/**
 * Reset a search state for use in a search
 */
inline void onewire_search_init(onewire_search_state* state)
{
    state->lastZeroBranch = -1;
    state->done = false;

    // Zero-fill the address
    memset(state->address, 0, sizeof(state->address));
}

/**
 * Look for the next slave address on the bus
 *
 * Before the first search call, the state parameter must be initialised using
 * onewire_init_search(state). The same state must be passed to subsequent calls
 * to discover all available devices.
 *
 * The caller is responsible for performing a CRC check on the result if desired.
 *
 * @returns true if a new address was found
 */
bool onewire_search(const gpin_t* io, onewire_search_state* state);

/**
 * Look for the next slave address on the bus with an alarm condition
 * @see onewire_search()
 */
bool onewire_alarm_search(const gpin_t* io, onewire_search_state* state);

/**
 * Return true if the CRC byte in a ROM address validates
 */
bool onewire_check_rom_crc(onewire_search_state* state);

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

/**
 * Read the last temperature conversion from a specific probe
 * Address must be a an array of 8 bytes (uint8_t[8])
 */
uint16_t ds18b20_read_slave(const gpin_t* io, uint8_t* address);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CRC Functions
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <util/crc16.h>

uint8_t crc8(uint8_t* data, uint8_t len)
{
    uint8_t crc = 0;

    for (uint8_t i = 0; i < len; ++i) {
        crc = _crc_ibutton_update(crc, data[i]);
    }

    return crc;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TODO: Check order of things!!
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

void onewire_match_rom(const gpin_t* io, uint8_t* address)
{
    // Write Match Rom command on bus
    onewire_write(io, 0x55);

    // Send the passed address
    for (uint8_t i = 0; i < 8; ++i) {
        onewire_write(io, address[i]);
    }
}

void onewire_skiprom(const gpin_t* io)
{
    onewire_write(io, 0xCC);
}

/**
 * Search procedure for the next ROM addresses
 *
 * This algorithm is bit difficult to understand from the diagrams in Maxim's
 * datasheets and app notes, though its reasonably straight forward once
 * understood.  I've used the name "last zero branch" instead of Maxim's name
 * "last discrepancy", since it describes how this variable is used.
 *
 * A device address has 64 bits. With multiple devices on the bus, some bits
 * are ambiguous.  Each time an ambiguous bit is encountered, a zero is written
 * and the position is marked.  In subsequent searches at ambiguous bits, a one
 * is written at this mark, zeros are written after the mark, and the bit in
 * the previous address is copied before the mark. This effectively steps
 * through all addresses present on the bus.
 *
 * For reference, see either of these documents:
 *
 *  - Maxim application note 187: 1-Wire Search Algorithm
 *    https://www.maximintegrated.com/en/app-notes/index.mvp/id/187
 *
 *  - Maxim application note 937: Book of iButton® Standards (pages 51-54)
 *    https://www.maximintegrated.com/en/app-notes/index.mvp/id/937
 *
 * @see onewire_search()
 * @returns true if a new address was found
 */
static bool _search_next(const gpin_t* io, onewire_search_state* state)
{
    // States of ROM search reads
    enum {
        kConflict = 0b00,
        kZero = 0b10,
        kOne = 0b01,
    };

    // Value to write to the current position
    uint8_t bitValue = 0;

    // Keep track of the last zero branch within this search
    // If this value is not updated, the search is complete
    int8_t localLastZeroBranch = -1;

    for (int8_t bitPosition = 0; bitPosition < 64; ++bitPosition) {

        // Calculate bitPosition as an index in the address array
        // This is written as-is for readability. Compilers should reduce this to bit shifts and tests
        uint8_t byteIndex = bitPosition / 8;
        uint8_t bitIndex = bitPosition % 8;

        // Configure bus pin for reading
        gset_input_hiz(io);

        // Read the current bit and its complement from the bus
        uint8_t reading = 0;
        reading |= onewire_read_bit(io); // Bit
        reading |= onewire_read_bit(io) << 1; // Complement of bit (negated)

        switch (reading) {
            case kZero:
            case kOne:
                // Bit was the same on all responding devices: it is a known value
                // The first bit is the value we want to write (rather than its complement)
                bitValue = (reading & 0x1);
                break;

            case kConflict:
                // Both 0 and 1 were written to the bus
                // Use the search state to continue walking through devices
                if (bitPosition == state->lastZeroBranch) {
                    // Current bit is the last position the previous search chose a zero: send one
                    bitValue = 1;

                } else if (bitPosition < state->lastZeroBranch) {
                    // Before the lastZeroBranch position, repeat the same choices as the previous search
                    bitValue = state->address[byteIndex] & (1 << bitIndex);

                } else {
                    // Current bit is past the lastZeroBranch in the previous search: send zero
                    bitValue = 0;
                }

                // Remember the last branch where a zero was written for the next search
                if (bitValue == 0) {
                    localLastZeroBranch = bitPosition;
                }

                break;

            default:
                // If we see "11" there was a problem on the bus (no devices pulled it low)
                return false;
        }

        // Write bit into address
        if (bitValue == 0) {
            state->address[byteIndex] &= ~(1 << bitIndex);
        } else {
            state->address[byteIndex] |= (bitValue << bitIndex);
        }

        // Configure for output
        gset_output_high(io);
        gset_output(io);

        // Write bit to the bus to continue the search
        onewire_write_bit(io, bitValue);
    }

    // If the no branch points were found, mark the search as done.
    // Otherwise, mark the last zero branch we found for the next search
    if (localLastZeroBranch == -1) {
        state->done = true;
    } else {
        state->lastZeroBranch = localLastZeroBranch;
    }

    // Read a whole address - return success
    return true;
}

static inline bool _search_devices(uint8_t command, const gpin_t* io, onewire_search_state* state)
{
    // Bail out if the previous search was the end
    if (state->done) {
        return false;
    }

    if (!onewire_reset(io)) {
        // No devices present on the bus
        return false;
    }

    onewire_write(io, command);
    return _search_next(io, state);
}

bool onewire_search(const gpin_t* io, onewire_search_state* state)
{
    // Search with "Search ROM" command
    return _search_devices(0xF0, io, state);
}

bool onewire_alarm_search(const gpin_t* io, onewire_search_state* state)
{
    // Search with "Alarm Search" command
    return _search_devices(0xEC, io, state);
}

bool onewire_check_rom_crc(onewire_search_state* state)
{
    // Validate bits 0..56 (bytes 0 - 6) against the CRC in byte 7 (bits 57..63)
    return state->address[7] == crc8(state->address, 7);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ds18b20 Functions
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    onewire_skiprom(io);
    onewire_write(io, kReadScatchPad);

    // Read the data from the scratch pad
    return ds18b20_readScratchPad(io);
}

uint16_t ds18b20_read_slave(const gpin_t* io, uint8_t* address)
{
    // Confirm the device is still alive. Abort if no reply
    if (!onewire_reset(io)) {
        return kDS18B20_DeviceNotFound;
    }

    onewire_match_rom(io, address);
    onewire_write(io, kReadScatchPad);

    // Read the data from the scratch pad
    return ds18b20_readScratchPad(io);
}

void ds18b20_convert(const gpin_t* io)
{
    // Send convert command to all devices (this has no response)
    onewire_skiprom(io);
    onewire_write(io, kConvertCommand);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Int Main
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set up the ATmega328P's UART peripheral to transmit
 *
 * The contents of this may need to be changed if you're using a different chip
 * or a different CPU frequency.
 */
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

/**
 * Send a single byte out over serial (blocking)
 */
void usart_transmit(uint8_t data)
{
    // Wait for empty transmit buffer
    while ( !( UCSR0A & (1<<UDRE0)) );

    // Put data into buffer, sends the data
    UDR0 = data;
}

/**
 * Block until a single byte is received over serial
 */
uint8_t usart_receive(void)
{
    // Wait for data to be received
    while ( !(UCSR0A & (1<<RXC0)) );

    // Get and return received data from buffer
    return UDR0;
}

/**
 * Print a null-terminated string
 */
void print(char* string)
{
    uint8_t i = 0;
    while (string[i] != '\0') {
        usart_transmit(string[i]);
        ++i;
    }
}

/**
 * Print a null-terminated string followed by an automatic line break
 */
void println(char* string)
{
    print(string);
    usart_transmit('\n');
}

/**
 * Convert a fixed-point 12.4 number into a floating point string
 * This is creates a human-readable string for temperature values from a DS18B20
 *
 * The passed buffer must be able to accomodate up to 9 characters plus a null terminator.
 */
void fptoa(uint16_t value, char* buf, uint8_t bufsize)
{
    const uint16_t integer = (value >> 4); //original!

    // uint16_t integer = (value >> 4); //my convert to F degrees, need to convert with the decimal though!!!! Still the C decimal
    // integer = ( (9 * integer) + 160 ) / 5; //my convert to F degrees

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

const static gpin_t sensorPin = { &PORTD, &PIND, &DDRD, PD3 };

int main(void)
{
    init_usart();

    for (;;) {
        if (!onewire_reset(&sensorPin)) {
            println("Nothing on the bus?");
            _delay_ms(1000);
            continue;
        }

        // Intiate conversion and wait for it to finish
        ds18b20_convert(&sensorPin);
        _delay_ms(750);

        // Prepare a new device search
        onewire_search_state search;
        onewire_search_init(&search);

        // Search and dump temperatures until we stop finding devices
        while (onewire_search(&sensorPin, &search)) {
            if (!onewire_check_rom_crc(&search)) {
                print("Bad ROM CRC");
                continue;
            }

            uint16_t reading = ds18b20_read_slave(&sensorPin, search.address);

            if (reading != kDS18B20_CrcCheckFailed) {
                // You can get the temperature as floating point degrees if needed, though
                // working with floats on small AVRs eats up a lot of cycles and code space:
                
                  // float temperature = ((float) reading) / 16;
                

                // Convert fixed-point to a printable string
                char buf[10];
                fptoa(reading, buf, sizeof(buf));
                print(buf);

            } else {
                println("Bad temp CRC");
            }

            print("  ");
        }

        print("\r\n"); //For putty default settings. 
    }

    return 0;
}