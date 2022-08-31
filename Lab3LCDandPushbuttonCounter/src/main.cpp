/* This project is a review of the Embedded lab #3 LCD and Pushbutton Counter
  Note: This project wasnt linking the MSOE header files for the longest time
  I finally got them to all link and not really sure what the issue was

  Interface the LCD panel 16x2 and a pair of pushbuttons to the Arduino Uno
  and display the number of button pressses on the LCD

  Requirments are as follows: 
  *Interface the LCD panel to the micro
  *Interface a pair of pushbuttons to the micro inputs
  *When button 1 is pressed, the count on the LCD increases by exactly 1
  *When button 2 is pressed, the count on the LCD decreases by exactly 1
  *Should display for example "count = 21"
  *The count should increment/decrement only once per press. Doesnt continue when held down
  *The count can be either positive or negative

  Inputs:
  *PC0 (button1), PC1(button2): Both buttons are then wired to ground

  Outputs: 
  LCD 16x2 Wiring:
	rs-PB5
  R/w-GND
  E-PB4
  DB 4:7-PD4:7
  Vss-GND
  Vee-pot-(5v,GND)
  Vdd-5v
  PC0-button1
  PC1-button2

  A wiring diagram is included in the file structure.
*/

#include <avr/io.h>
#include <inttypes.h>
#include "MSOE/delay.c"
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

#include <Arduino.h>

#define PUSHBUTTON_1 3
#define PUSHBUTTON_2 2
#define PUSHBUTTON_1_AND_2 1
#define PUSHBUTTON_RESET 4

int main (void)
{
  // Set up digital inputs
  DDRC &= (~(1<<PORTC0)) & (~(1<<PORTC1));
  PORTC |= (1<<PORTC0) | (1<<PORTC1); //setting pull up resistors

  int count = 0;
  int mastercount = 0;
  int button1resetflag = 0;
  int button2resetflag = 0;
	int buttonScan;

  // Function prototypes
	int pushbuttonScan(void);

  //Initialize LCD Screen
	lcd_init();
	lcd_clear();
	lcd_home();

  //Initial LCD Print
  lcd_printf("(ADD)      (SUB)");
  lcd_goto_xy(7,1);
  lcd_printf("%d",mastercount);

  while(1)
  {
    // Checking status of buttons
    buttonScan = pushbuttonScan();

    if(buttonScan == PUSHBUTTON_RESET)
    {
      //Could have individual resets instead but it shouldnt matter
      button1resetflag = 0;
      button2resetflag = 0;
    }

    if((buttonScan == PUSHBUTTON_1) & (!button1resetflag))
    {
      count = count + 1;
      button1resetflag = 1;
    }

    if((buttonScan == PUSHBUTTON_2) & (!button2resetflag))
    {
      count = count - 1;
      button2resetflag = 1;
    }

    if(buttonScan == PUSHBUTTON_1_AND_2)
    {
      count = 0;
    }

    //Run the printing of on the LCD
    //Only update LCD if the count changed
    if(mastercount != count)
    {
      mastercount = count;
      lcd_clear();
      lcd_home();
      lcd_printf("(ADD)      (SUB)");
      lcd_goto_xy(7,1);
      lcd_printf("%d", mastercount);
    }

  }

  return 0;
}

int pushbuttonScan(void)
{
  int buttonPressStatus = 0;

  //Bit Masking inputs for each button
  //Note that if a button is not pressed it reads high because of the pullup resistors
  uint8_t button1and2Check = PINC & 0b00000011;
  uint8_t button1Check = PINC & 0b00000001;
  uint8_t button2Check = PINC & 0b00000010;
  
  if(button1and2Check == 0b00000000)
  {
    buttonPressStatus = PUSHBUTTON_1_AND_2;
  }

  else if(button1Check == 0b00000000)
  {
    buttonPressStatus = PUSHBUTTON_1;
  }

  else if(button2Check == 0b00000000)
  {
    buttonPressStatus = PUSHBUTTON_2;
  }

  else if(button1and2Check == 0b00000011)
  {
      buttonPressStatus = PUSHBUTTON_RESET;
      delay_ms(50); //For Debouncing of button
  }

  return buttonPressStatus;

}