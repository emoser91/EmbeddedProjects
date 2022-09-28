/* This project is a test of the Watch Dog Timer
   In this project, we will blink the on board LED (PORTB5/Digital 13) at different rates
   depending on where we are in the program to help to visualize the steps in a watchdog timer. 
   The exact blink rates are descrribed in the section below.

   Watchdog Features:
   *Clocked from separate on-chip oscillator (128kHz)
   *Three operating modes Interrupt Mode, System Reset Mode, and Interrput and System Reset Mode
   *Selectable time-out period from 16ms to 8s
   *Possible hardware fuse watchdog always on (WDTON) for fail-safe mode

   This project we will try out the Interrupt and System Reset Mode with a 4s time-out and not touch the hardware fuse at all.
   
   Interrupt and System Reset Mode: 
   Watchdog Timer Setup Procedure:
   1. Disable Inturrupts
   2. Reset watchdog timer with WDR commands
   3. Write logic 1 to WDCE and WDE bits of WDTCSR simultaneously
   4. Then, in four cycles, write logic 1 to enable(or zero to disable) watchdog timer or change prescaller

   Program Description:
   We will blink LED at approximately 0.5Hz (Medium Rate), 
   and when watchdog interrupts occurs, it will give a burst of five 0.1Hz signals (Fast Rate).
   After reset, LED will light for a half-second (Hold Solid). These will give us a clue about ongoing processes.

   Keep in mind that after watchdog interrupt counter has to count up once more for a reset to occur.
   It looks like the first time the watchdog is triggered, it causes interrupt to fire and 
   then next time it will cause a reset to happen. (Interrupt and Reset Mode)

   We are using the _delay_ms() function from the util/delay.h include
   we are using wdt_reset() function from the avr/wdt.h include

   Be careful with Watchdog Timer systems as it is possible to get it stuck into an infinate resetting loop

   Take a look at the attached files in this same directory as well as the atmega328p datasheet for more info

   Wiring:
   None, just using the on-board LED digital 13

*/

// #include <Arduino.h> //Not really needed with the following includes

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

// Can either include avr/wdt.h or just uncomment this line bellow
// #define wdt_reset() __asm__ __volatile__ ("wdr") //Macro from avr/wdt.h

// Prepare LED pin
void PortInit(void)
{
    //PORTB5/Digital 13 on-board LED ON
    DDRB |= (1<<PORTB5);
    PORTB |= (1<<PORTB5);
}

// initialize watchdog
void WDT_Init(void)
{
  //disable interrupts
  cli();

  //reset watchdog (function from avr/wdt.h)
  wdt_reset(); //"Feeding the Dog"

  //set up WDT interrupt
  WDTCSR = (1<<WDCE) | (1<<WDE);

  //Start watchdog timer with 4s prescaller
  WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3);

  //Enable global interrupts
  sei();
}

// Watchdog timeout ISR
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
}

int main (void)
{
    //Initialize port
    PortInit();

    //initialize watchdog
    WDT_Init();

    //Long delay to detect reset where LED will be solid lit
    _delay_ms(500);

    while(1)
    {
      //Medium speed flashing of LED meaning it is in while loop
      PORTB |= (1<<PORTB5); //LED ON
      _delay_ms(100);
      PORTB &= ~(1<<PORTB5); //LED OFF
      _delay_ms(500); //~0.5s delay

      //  wdt_reset(); //Uncomment to keep "feeding the dog" so the watchdog timer event is not triggered
    }
}