/* This project is a review of the Embedded lab #9 Sony IRC Remote Control Decoder.

  Project uses two Timer/Counters one Fast PWM and one CTC setup as an input capture
  
  *Learn and understand the Sony Infared Remote Control (SIRC) Protocol
  *Recieve bits from a Sony IR remote control using a Sharp GP1uXS1QS
  *First wire and then manually decode the 12 data bits with an oscilloscope
  *Write a program that displays the on the LCD the binary value of the 12 bit word recieved from the remote controller
  *Expand the program to display on the LCD bottom line which button was pressed
  *Show Keys: 0-9, channel up/down, volume up/down
  *The last part of the lab, I choose to add motors and try to drive my arduino with the remote
  *I used a motor driver chip L293D
  *Use Atmega328p Datasheet for Timer/Counter Register Setup
  *Use an Oscilloscope to confirm waveforms

  *!!! There is a lot of wiring in this lab so check out the wiring diagram in the folder !!!
  *!!! No clue what the R5,R6,R7 LEDs are for, they are not used in the oringal code !!!
  *A wiring diagram is included in the file structure.

  *Note, I cleaned up the code and worked to understand everything. I did not rewire and setup to test. 
  *I could not test so I did not refactor the whole project. The data type conversions can be improved. 

  LCD Output is the same as in Lab3
  Outputs: 
  LCD 16x2 Wiring:
  rs-PB5
  R/w-GND
  E-PB4
  DB 4:7-PD4:7
  Vss-GND
  Vee-pot-(5v,GND)
  Vdd-5v

  CTC Timer/Counter for Input Capture Register Setup:
  Resister Setup
  TCCR1A
  (COM1A1)(COM1A0)(COM1B1)(COM1B0)(-)(-)(WGM11)(WGM10)
     1        0       0      0     0  0    1      0

  TCCR1B
  (ICNC1)(ICES1)(-)(WGM13)(WGM12)(CS12)(CS11)(CS10)
     0     0     0    0      1      0     0     1

  TIMSK1: Timer/Counter Interrupt Mask Register
  (-)(-)(ICIE1)(-)(-)(OCIE1B)(OCIE1A)(TOIE1)
   0  0    1    0  0    0       0       0

  Input:
  *Digital 8/PB0/ICP1 input from the IR Reciever

  Fast PWM Timer/Counter for Motors Register Setup
  TCCR2A
  (COM2A1)(COM2A0)(COM2B1)(COM2B0)(-)(-)(WGM21)(WGM20)
     0        1       1      0     0  0    1      1

  TCCR2B
  (FOC2A)(FOC2B)(-)(-)(WGM22)(CS22)(CS21)(CS20)
     0     0     0  0    1      1     1     1

  Outputs:
  *digital 3 PWM/OC2B output to motor driver chip
  *PC0:PC3 outputs for the motor driver chip directional bits

  *CHECK THE WIRING DIAGRAM FOR THE REST OF THE WIRING
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <MSOE/delay.c>
#include <MSOE/bit.c>
#include <MSOE/lcd.c>

//Sony Remote Keys and values
#define KEY1 256
#define KEY2 258
#define KEY3 260
#define KEY4 262
#define KEY5 264
#define KEY6 266
#define KEY7 268
#define KEY8 270
#define KEY9 272
#define KEY0 274

#define KEYVOLUP    292
#define KEYVOLDOWN  294
#define KEYCHANUP   288
#define KEYCHANDOWN 290
#define KEYUP       488
#define KEYDOWN     490
#define KEYLEFT     360
#define KEYRIGHT    358
#define KEYSTOP     458

//SONY Protocol is 1 start bit, 7 Data bits, 5 Address bits
#define SONYBITLENGTH 13
#define STARTBITLENGTH 38400

//GLOBAL VARRIBLES
int edgeTrigger = 0;
uint16_t bits[SONYBITLENGTH];
int bitposition = 0;
int allBitsCaptured = 0;

int main (void)
{
	//SETTING UP T/C 2 FOR MOTORS 500Hz
	DDRC |= (1<<PORTC0) | (1<<PORTC1);//motor control bit
	DDRC |= (1<<PORTC2) | (1<<PORTC3);//motor control bit
	DDRD |= (1<<PORTD3);//pwm output digital 3 / OC2B
	TCCR2A = (1<<COM2B1) | (1<<COM2A1) | (1<<WGM21) | (1<<WGM20);//fast pwm mode and clear on compare
	TCCR2B = (1<<CS22) | (1<<CS20);//prescale 128 had cs21 and wgm22

	//SETTING UP T/C 1 FOR INPUT CAPTURE
	DDRB |= (~(1<<PORTB0));//input channel
	TCCR1A = (1<<COM1A1);//ctc mode and clear on compare (normally we go toggle)
	TCCR1B = (1<<WGM12)|(1<<CS10);//Prescale 1
	OCR1A = 65535;//top set at max to provide highest input capture resolution with the prescale of 1
	//ICES1 is 0 by default for falling edge trigger
	TIMSK1 |= (1<<ICIE1);//input compare interrupt enable

	//ALTERNATIVE MOTOR SETUP
	//I set up the motors with phase correct pwm in EE2030 for motor control
	//TCCR0A=(1<<COM0B1)|(1<<COM0A1)|(1<<WGM00);//Phase correct PWM mode top=0xFF(255)
	//TCCR0B=(1<<CS01)|(1<<CS00);//N=64 =490.2Hz   

	sei();//turning on global interrupts
	lcd_init();lcd_clear();lcd_home();//LCD enable

	int startbit=0;
    int number=0;
	uint16_t result[SONYBITLENGTH];

	//function prototypes
	int allstop(void);
	int motorsfwd(void);
	int motorsback(void);
	int leftturn (void);
	int rightturn (void);

	while(1)
	{
		lcd_home();

		//START BIT CHECK
		if(bits[0] > STARTBITLENGTH)
		{
			startbit = 1; //startbit good
			result[0] = 0;//setting start bit
		}

		//TRANSFER SYSTEM TO BINARRY
		if(startbit == 1)//is start bit is good
		{
			for(int b = 1; b < SONYBITLENGTH; b++)
			{
				if(bits[b] > 19000) //Sony length of 19000 means a 1 bit, Math says 19200 is bit length
				{
					result[b] = 1;
				}
				if(bits[b] < 12000) //Sony length of 12000 means a 0 bit, Math says 9600 is bit length
				{
					result[b] = 0;
				}
			}

			//PRINTING BINARY TO LCD
			for(int c = 1; c < SONYBITLENGTH; c++)
			{
				lcd_goto_xy(c-1,0);//shifting bits
				lcd_print_uint16(result[c]);//printing each bit
			}
		}

		//DEFAULT CASE
		else
		{
			lcd_printf("PRESS ME");
		}

		//CONVERTING ARRAY TO DECIMAL
		if(allBitsCaptured == 1)
		{
			number = 0;
			if(result[0] == 1){number = number + 1;}
			if(result[1] == 1){number = number + 2;}
			if(result[2] == 1){number = number + 4;}
			if(result[3] == 1){number = number + 8;}
			if(result[4] == 1){number = number + 16;}
			if(result[5] == 1){number = number + 32;}
			if(result[6] == 1){number = number + 64;}
			if(result[7] == 1){number = number + 128;}
			if(result[8] == 1){number = number + 256;}
			if(result[9] == 1){number = number + 512;}
			if(result[10] == 1){number = number + 1024;}
			if(result[11] == 1){number = number + 2048;}
			if(result[12] == 1){number = number + 4096;}

			allBitsCaptured = 0;
		}

		//PRINTING DECIMAL NUMBER TO LCD//
		lcd_goto_xy(0,1);
		lcd_print_uint16(number);

		//PRINT BUTTON PRESSES TO LCD
		lcd_goto_xy(10,1);
		if(number==KEY1){lcd_printf("1    ");}
		if(number==KEY2){lcd_printf("2    ");}
		if(number==KEY3){lcd_printf("3    ");}
		if(number==KEY4){lcd_printf("4    ");}
		if(number==KEY5){lcd_printf("5    ");}
		if(number==KEY6){lcd_printf("6    ");}
		if(number==KEY7){lcd_printf("7    ");}
		if(number==KEY8){lcd_printf("8    ");}
		if(number==KEY9){lcd_printf("9    ");}
		if(number==KEY0){lcd_printf("0    ");}
		if(number==KEYVOLUP){lcd_printf("vol+ ");}
		if(number==KEYVOLDOWN){lcd_printf("vol-");}
		if(number==KEYCHANUP){lcd_printf("Chan+");}
		if(number==KEYCHANDOWN){lcd_printf("Chan-");}
		if(number==KEYUP){lcd_printf("up   ");}
		if(number==KEYDOWN){lcd_printf("down ");}
		if(number==KEYLEFT){lcd_printf("left ");}
		if(number==KEYRIGHT){lcd_printf("right");}
		if(number==KEYSTOP){lcd_printf("STOP ");}

		//RUNNING THE MOTOR
		//Originally I was told to ramp speeds but this code just sends it
		if(number == 488)//forward
		{
			motorsfwd();
		}
		if(number == 490)//backwards
		{
			motorsback();
		}
		if(number == 458)//all stop
		{
			allstop();
		}
		if(number == 358)//right
		{
			rightturn();
		}
		if(number == 360)//left
		{
			leftturn();
		}
	}

  return 0;
}

//INPUT CAPTURE INTERRUPT//
ISR(TIMER1_CAPT_vect)
{
	//Looking for the falling edge for the start of bits
	if(edgeTrigger == 0)
	{
		TCNT1 = 0;//setting timer counter count back to 0
		TCCR1B |= (1<<ICES1);//raising edge trigger to look for the end of the bit
		edgeTrigger = 1;
	}

	//Looking for the raising edge for the end of bit
	else if(edgeTrigger == 1)
	{
		bits[bitposition] = ICR1;//storing in array
		TCCR1B &= (~(1<<ICES1));//falling edge trigger to look for the start of the next bit
		edgeTrigger = 0;
		bitposition++; //varrible for array
	}

  	//Array reset if max length of bits is captured
	if(bitposition == SONYBITLENGTH)//refresh system
	{
		bitposition = 0;
		allBitsCaptured = 1;
	}
}

int motorsfwd(void)
{
	PORTC |= (1<<PORTC1) | (1<<PORTC2);
	PORTC &= (~(1<<PORTC0)) | (~(1<<PORTC3));
	OCR2B = 255;
	OCR2A = 255;
	return 0;
}
int motorsback(void)
{
	PORTC |= (1<<PORTC0) | (1<<PORTC3);
	PORTC &= (~(1<<PORTC1)) | (~(1<<PORTC2));
	OCR2B = 255;
	OCR2A = 255;
	return 0;
}
int leftturn(void)
{
	PORTC |= (1<<PORTC1) | (1<<PORTC2);
	PORTC &= (~(1<<PORTC0)) | (~(1<<PORTC3));
	OCR2B = 255;
	OCR2A = 100;
	return 0;
}
int rightturn(void)
{
	PORTC |= (1<<PORTC1) | (1<<PORTC2);
	PORTC &= (~(1<<PORTC0)) | (~(1<<PORTC3));
	OCR2B = 100;
	OCR2A = 255;
	return 0;
}
int allstop(void)
{
	PORTC &= (~(1<<PORTC0)) & (~(1<<PORTC1)) & (~(1<<PORTC2)) & (~(1<<PORTC3));
	return 0;
}