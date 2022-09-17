/*
*testPointerToGPIORegister.c
*Demostrates use of pointers to address memory mapped registers.
*
*Onboard LED ("L") should flash on and off
*
*MSOE supplied code
*/

#include <avr/io.h>
#include <inttypes.h>
#include <MSOE/delay.c>

#define MASK_BITS 0x20

int main(void)
{
  uint8_t *regDDRB;
  uint8_t *regPORTB;

  regDDRB = (uint8_t *) 0x24; //setup pointer to DDRB
  regPORTB = (uint8_t *) 0x25; //setup pointer to PORTB

  *regDDRB = (*regDDRB) | MASK_BITS; //set pin 5 to output

  while(1)
  {
    *regPORTB = (*regPORTB) ^ MASK_BITS;
    delay_ms(200);
  }

  return 0;
}