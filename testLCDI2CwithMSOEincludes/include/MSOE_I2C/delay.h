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
	delay.h
	Header file for the delay function
	Copyright (c) 2004 Joerg Mossbrucker <mossbruc@msoe.edu>
*/

#ifndef _DELAY_H
#define _DELAY_H

#include "globaldef.h"
#include <inttypes.h>
void delay_us(uint16_t us);
// delay for ms milli-seconds
// delay times are only half-way accurate if optimization is turned on!
void delay_ms(uint16_t ms)
{
	volatile uint16_t i;

	for(i=ms;i>0;i--)
	{
		delay_us(1000);
	}
}

// delay for us micro-seconds
// delay times are only half-way accurate if optimization is turned on to level 3!
// max value for us is 65535/4*CYCLES_PER_US
// which is app. 17777 for 14.7456MHZ
void delay_us(uint16_t us)
{
	uint16_t _count;
	_count=us/4*CYCLES_PER_US;

	asm volatile (
		"1: sbiw %0,1" "\n\t"
		"brne 1b"
		: "=w" (_count)
		: "0" (_count)
	);
}

#endif
