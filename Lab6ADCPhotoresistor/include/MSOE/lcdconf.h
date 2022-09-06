/* 
   Copyright (c) 2003 Joerg Mossbrucker <mossbruc@msoe.edu>
   
   All Rights Reserved.
  
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
  
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
  
*/

/*
	lcdconf.h
	contains configuration information for the LCD display
	Copyright (c) 2003 Joerg Mossbrucker <mossbruc@msoe.edu>
*/

#ifndef _LCDCONF_H
#define _LCDCONF_H

// define port and bits the LCD control signals are connected
#define LCD_CTRL_PORT	PORTB
#define LCD_CTRL_DDR	DDRB

/*
 * 7/21/2011 (CJP): Modified for Arduino UNO
 *
 * 7/26/2011 (CJP): Further modification to free up control of
 * RW pin on LCD.  Instead, connect RW pin to GND.
 */
//#define LCD_CTRL_RS		2
//#define LCD_CTRL_RW		1
//#define LCD_CTRL_E		0
#define LCD_CTRL_RS		5	// Arduino UNO digital pin 13
//#define LCD_CTRL_RW		4	// Arduino UNO digital pin 12
//#define LCD_CTRL_E		3	// Arduino UNO digital pin 11
#define LCD_CTRL_E		4	// Arduino UNO digital pin 12

// define port the LCD data signals are connected
/*
 * 7/21/2011 (CJP): Modified for Arduino UNO
 */
//#define LCD_DATA_PORT	PORTB
//#define LCD_DATA_DDR	DDRB
#define LCD_DATA_PORT	PORTD
#define LCD_DATA_DDR	DDRD

// defines LCD data interface (currently only 4 bit interface is implemented)
// almost all low-level functions need to be changed if 8 bit interface is also implemented
#define LCD_INTERFACE	4

#define LCD_LINES				2
#define LCD_LINE_LENGTH			16

// DDRAM mapping
#define LCD_LINE0_DDRAMADDR		0x00	// on 2x16 0x00-0x0F
#define LCD_LINE1_DDRAMADDR		0x40	// on 2x16 0x40-0x4F
#define LCD_LINE2_DDRAMADDR		0x14
#define LCD_LINE3_DDRAMADDR		0x54

#endif
