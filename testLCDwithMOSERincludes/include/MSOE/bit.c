/* 
   Copyright (c) 2005 Joerg Mossbrucker <mossbruc@msoe.edu>
   
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
	bit.c
	contains only a couple of macros helpful for beginning programmers
	Copyright (c) 2005 Joerg Mossbrucker <mossbruc@msoe.edu>
*/

#ifndef _BIT_C
#define _BIT_C

#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))
#define tbi(a, b)  (((a) & (1<<(b)))>>(b))

#define bit_set(a,m)    ((a) |= (m))
#define bit_clear(a,m)  ((a) &= ~(m))
#define bit_toggle(a,m) ((a) ^= (m))
#define bit_test(a, m)  ((a) & (m))

#endif