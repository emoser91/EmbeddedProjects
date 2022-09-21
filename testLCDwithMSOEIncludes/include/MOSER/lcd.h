/*
	lcd.h
	Header file for the lcd interface
*/

#ifndef _LCD_H
#define _LCD_H

#include "globaldef.h"
#include "delay.c"
#include <inttypes.h>


// Hitachi HD44780 commands
// LCD interface is used in write-only mode, therefore no busy define

#define SetBit(a, b) (a) |= (1 << (b))
#define ClearBit(a, b) (a) &= ~(1 << (b))

#define LCD_CLR             0x01	// clear display
#define LCD_HOME			0x02	// return to home position
#define LCD_CURSOR_RIGHT	0x14	// moves cursor right
#define LCD_CURSOR_LEFT		0x10	// moves cursor left
#define LCD_SHIFT_RIGHT		0x1C	// shifts display right
#define LCD_SHIFT_LEFT		0x18	// shifts display left

#define LCD_CGRAM           6      // set CG RAM address
#define LCD_DDRAM           7      // set DD RAM address

// DDRAM mapping
#define LCD_LINE0_DDRAMADDR		0x00	// on 2x16 0x00-0x0F
#define LCD_LINE1_DDRAMADDR		0x40	// on 2x16 0x40-0x4F

// Function prototypes
void LcdInit(void);
void LcdPortInit(void);
void LcdDataWrite(uint8_t);
void LcdCommandWrite(uint8_t);
void LcdClear(void);
void LcdHome(void);
void LcdPrintChar(uint8_t);
void LcdPrintString(const char *);
void LcdPrintUint8(uint8_t);
void LcdPrintInt8(int8_t);
void LcdPrintUint16(uint16_t);
void LcdPrintInt16(int16_t);
void LcdPrintHex(uint8_t);
void LcdPrintFloat(float);
void LcdPrintf(char *fmt, ...);
void LcdGoToXY(uint8_t, uint8_t);

#endif
