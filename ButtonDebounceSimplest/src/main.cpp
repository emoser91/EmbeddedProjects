/* The simplest debounce function

Only Works for a Single Button

When the button is pressed, the button pull the GPIO pin to ground and produce a 0. The value is added into the state through OR | operator, 
and shift up through shift << operator each time the button pin is read, the 0xfe00 is a bit mask that mask-out the higher byte, and sort of 
means that we only care if the lower byte produce a consecutive stream of 0.

When you think of the bouncing problem, the button is in an unstable mode for the initial a few microseconds when it is pressed, and could 
produce a stream of either 0 or 1 during that few microseconds. If our little function could detect a pattern consists of a stream of 0 after 
8 times of reading the button pin, the state | 0xff00 would produce a true, in another word, the button has reached to a stable state and no 
longer bouncing around.

*/

#include <Arduino.h>

#define btn 2  //assuming we use D2 on Arduino

//Returns TRUE once when the debounced leading edge of the switch closure is encountered.
//It offers protection from both bounce and EMI.
bool debounce()
{
  static uint16_t state = 0;
  state = (state << 1) | digitalRead(btn) | 0xfe00;
  return (state == 0xff00);
}

void setup()//Basically int main (void)
{
  pinMode(btn, INPUT_PULLUP); //Set Button Input and Pullup Resistor
  pinMode(LED_BUILTIN, OUTPUT); //Set LED Output
}

void loop()//Basically while(1)
{
  if (debounce())
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); //Light Up LED
  }
}
