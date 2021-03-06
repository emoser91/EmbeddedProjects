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
	lcd.h
	Header file for the lcd interface
	Copyright (c) 2004 Joerg Mossbrucker <mossbruc@msoe.edu>
*/

#ifndef _LCD_H
#define _LCD_H

#include "globaldef.h"
#include "lcdconf.h"
#include "delay.c"
#include <inttypes.h>


// Hitachi HD44780 commands
// LCD interface is used in write-only mode, therefore no busy define

#define LCD_CLR             0x01	// clear display
#define LCD_HOME			0x02	// return to home position
#define LCD_CURSOR_RIGHT	0x14	// moves cursor right
#define LCD_CURSOR_LEFT		0x10	// moves cursor left
#define LCD_SHIFT_RIGHT		0x1C	// shifts display right
#define LCD_SHIFT_LEFT		0x18	// shifts display left




#define LCD_ENTRY_MODE      2      // set entry mode
#define LCD_ENTRY_INC       1      // increment
#define LCD_ENTRY_SHIFT     0      // shift
#define LCD_ON_CTRL         3      // turn on lcd/cursor
#define LCD_ON_DISPLAY      2      // turn on display
#define LCD_ON_CURSOR       1      // turn on cursor
#define LCD_ON_BLINK        0      // blinking cursor
#define LCD_MOVE            4      // move cursor/display
#define LCD_MOVE_DISP       3      // move display (0-> move cursor)
#define LCD_MOVE_RIGHT      2      // move right (0-> left)
#define LCD_FUNCTION        5      // function set
#define LCD_FUNCTION_8BIT   4      // set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 3      // two lines (0->one line)
#define LCD_FUNCTION_10DOTS 2      // 5x10 font (0->5x7 font)

#define LCD_CGRAM           6      // set CG RAM address
#define LCD_DDRAM           7      // set DD RAM address

// high level functions

// initilaize LCD
void lcd_init(void);
// set cursor to home
void lcd_home(void);
// clear display
void lcd_clear(void);
// set cursor position
void lcd_goto_xy(uint8_t x,uint8_t y);
// print character
void lcd_print_char(uint8_t symbol);
// print string at current position
void lcd_print_string(char *string);
// print hex number on LCD
void lcd_print_hex(uint8_t hex);
// print uint8 on LCD
void lcd_print_uint8(uint8_t no);
// print int8 on LCD
void lcd_print_int8(int8_t no);
// print uint16 on LCD
void lcd_print_uint16(uint16_t no);
// print int16 on LCD
void lcd_print_int16(int16_t no);
// print float on LCD
void lcd_print_float(float no);
// lcdprintf
void lcd_printf(char *fmt, ...);



// low level functions

// initialize port connected to LCD
void lcd_port_init(void);
// send a data byte to LCD
void lcd_data_write(uint8_t data);
// send a command byte to LCD
void lcd_cmd_write(uint8_t cmd);

#endif
