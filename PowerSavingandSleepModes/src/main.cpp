/* This project is a test of the Power Saving and Sleep Modes on the Arduino Uno
   
   Arduino Power Savings
   1. Write all Digital Pins high or low when they are not used (We write all Low in this program)
   2. Turn off all subsystems in Power Down Register
   3. Disable Brownout Detection System
   4. Put Arduino into Power Down Mode (Deepest Sleep Mode)
   *Note: In Power Down Mode, Arduino will only wake to external interrupt, TWI(I2C) address match or watchdog interrupt
    can wake the Arduino back up. Reset button will reset Arduino and Brownout reset but we turn Brownout off.

   Additional Power Saving Options (NOT USED IN THIS PROJECT)
   1. Slow down Arduino Clock with Clock Prescale Register CLKPR (Setting CLKPS0 to 1 in CLKPR will slow down by factor of 2 16Mhz to 8Mhz clock rate)
   *Note: Any delay funcitons will need to be multiplied by the prescale value to be correct. IE for factor of 2, multiply delays by 2
   2. Remove the Power to the ON LED on the board by removing the resistor going to the ON LED
   3. Reduce the input power going to the Arduino down to 3v, Operating Voltage Range: 2.7v to 5.5v
   4. Standalone atmega328p, remove the chip from the Arduino board and wire it up to run on its own

   Description of Program
   1. Program will hold on-board LED lit for 3 seconds
   2. The LED will then turn off and we will enter Power Down Sleep Mode
   3. 8 seconds will then pass while the Arduino is in Sleep
   4. The Watchdog Timer will trigger its interrupt and the system will wake back up
   5. The on-board LED will flash quickly 5 times to show that the Arduino has woken up
   6. Previously turned off systems can be turned back on at this time in the Watchdog Interrupt
   *Note: The BOD: Brownout Detection will automatically turn back on when we come out of sleep mode

   Wiring:
   None, just using the on-board LED digital 13/PB5

*/

// #include <Arduino.h> //Not really needed with the following includes

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define POWER_OFF 0
#define POWER_ON 1

void PowerSavePortsLow(void)
{
  //Set all pins to output
  DDRB |= (1<<PORTB0) | (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4) | (1<<PORTB5); //PB6 and PB7 for Clock
  DDRC |= (1<<PORTC0) | (1<<PORTC1) | (1<<PORTC2) | (1<<PORTC3) | (1<<PORTC4) | (1<<PORTC5); //PC6 is RESET, no PC7
  DDRD |= (1<<PORTD0) | (1<<PORTD1) | (1<<PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7);

  //Write all pin output to 0
  PORTB &= ~(1<<PORTB0) & ~(1<<PORTB1) & ~(1<<PORTB2) & ~(1<<PORTB3) & ~(1<<PORTB4) & ~(1<<PORTB5);
  PORTC &= ~(1<<PORTC0) & ~(1<<PORTC1) & ~(1<<PORTC2) & ~(1<<PORTC3) & ~(1<<PORTC4) & ~(1<<PORTC5);
  PORTD &= ~(1<<PORTD0) & ~(1<<PORTD1) & ~(1<<PORTD2) & ~(1<<PORTD3) & ~(1<<PORTD4) & ~(1<<PORTD5) & ~(1<<PORTD6) & ~(1<<PORTD7);
}

// Prepare LED pin
void PortInit(void)
{
    //PORTB5/Digital 13 on-board LED ON
    DDRB |= (1<<PORTB5);
    PORTB |= (1<<PORTB5);
}

//Power Down Register
void Toggle_Subsystem_Power(uint8_t request)
{
  if(request == POWER_OFF)
  {
    //Power down all subsystems in Power Down Register, writing a 1 powers down
    //PRTWI=TWI, PRTTIM2=Timer/Counter2, PRTIM0=Timer/Counter0, PRTIM1=Timer/Counter1, PRSPI=SPI, PRUSART0=USART, PRADC=ADC
    PRR |= (1<<PRTWI) | (1<<PRTIM2) | (1<<PRTIM0) | (1<<PRTIM1) | (1<<PRSPI) | (1<<PRUSART0) | (1<<PRADC);
  }
  else if(request == POWER_ON)
  {
    //Power on all subsystems in Power Down Register
    PRR &= ~(1<<PRTWI) & ~(1<<PRTIM2) & ~(1<<PRTIM0) & ~(1<<PRTIM1) & ~(1<<PRSPI) & ~(1<<PRUSART0) & ~(1<<PRADC);
  }
}

//Brownout Detection Disable
void Brownout_Detect_Disable(void)
{
  MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
}

//Setup Power Down Sleep Mode
void Setup_Sleep_Mode(void)
{
  //Setup Deep Sleep Mode Power Down
  SMCR |= (1 << SM1); //power down mode
  SMCR |= (1 << SE);//enable sleep
}

// initialize watchdog with interrupt mode only and 8s prescale
void WDT_Init(void)
{
  //disable interrupts
  cli();

  //reset watchdog (function from avr/wdt.h)
  wdt_reset(); //"Feeding the Dog"

  //set up WDT interrupt
  WDTCSR = (1<<WDCE) | (1<<WDE);

  //Start watchdog timer 8s prescaller and clear out WDE and WDCE bits
  WDTCSR = 0b00100001; //Setting WDCE=0, WDE=0, WDIE=1, WDP0=1, WDP3=1

  //Turn on watchdog interrupt
  WDTCSR = (1<<WDIE);

  //Enable global interrupts
  sei();
}

// Watchdog timeout ISR
// Will wake up the Arduino from its Sleep Mode
ISR(WDT_vect)
{
    //Short Burst of five 0.1Hz pulses where LED will be flashing fast
    for (uint8_t i=0; i<=4; i++)
    {
        PORTB |= (1<<PORTB5); //LED ON
        _delay_ms(20); //~0.1s delay
        PORTB &= ~(1<<PORTB5); //LED OFF
        _delay_ms(80);
    }

    //Turn back on subsystems
    Toggle_Subsystem_Power(POWER_ON);

    //BOD: Brownout Detection will automatically enable when we come out of sleep mode
}

int main (void)
{
  //Set all pins to a known state to save power
  //Set all pins low
  PowerSavePortsLow();

  //Set desired Input/Output
  PortInit();

  //initialize watchdog
  WDT_Init();

  //Power Down all subsystems in Power Down Register
  Toggle_Subsystem_Power(POWER_OFF);

  //Enable Sleep - this enables the sleep mode
  Setup_Sleep_Mode();

  while(1)
  {
    //Turn on LED for 3 seconds
    PORTB |= (1<<PORTB5);
    _delay_ms(3000);
    PORTB &= ~(1<<PORTB5); //LED off

    //BOD DISABLE - this must be called right before the __asm__ sleep instruction
    Brownout_Detect_Disable();

    //Call Sleep Command
    __asm__  __volatile__("sleep");//in line assembler to go to sleep
  }

  return 0;
}