/*
*testFiveHundredHertzOnOff.c
*Sample program to demostrate Timer/Counter waveform generation and interrupts
*
*Waveform generated on OCOA (PD6) pin.
*
*MSOE supplied code
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

int8_t burst;

int main(void)
{
  PORTD |= (1<<PORTD6); //Waveform output

  /* Configure T/C0:
  *  CTC Mode, Toggle OC0A, N=64 (prescale)
  */
  TCCR0A = (1<<COM0A0) | (1<<WGM01);
  TCCR0B = (1<<CS01) | (1<<CS00);
  OCR0A = 249; //500Hz

/* Configure T/C1:
  *  CTC Mode, Output Compare Interrupt, N=1024 (prescale)
  */
  //TCCR1A left at all defaults
  TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
  OCR1A = 7812; //500Hz
  TIMSK1 = (1<<OCIE1A); //Timer/Counter 1, Output Compare A Match Interrupt Enable

  burst = 1;
  sei(); //Global interrupts on

  while(1)
  {
    //left empty
  }

  return 0;
}

//External Interrupt for INT0
ISR(TIMER1_COMPA_vect)
{
  if(burst ==1)
  {
    TCCR0B = 0x00;
    burst = 0;
    PORTD &= ~(1<<PORTD6);
  }
  else
  {
    TCCR0B = (1<<CS01) | (1<<CS00);
    burst = 1;
  }
}