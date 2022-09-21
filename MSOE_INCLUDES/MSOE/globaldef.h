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
	globaldef.h
	Global definitions for the MSOE development platform
	Copyright (c) 2004 Joerg Mossbrucker <mossbruc@msoe.edu>
*/


#ifndef _GLOBALDEF_H
#define _GLOBALDEF_H


// CPU clock speed

/*
 * 7/21/2011 (CJP):
 * resolves "F_CPU redefined" warning (defined during call to avr-gcc)
 * Arduino uses 16MHz crystal
 */
//#define F_CPU        14745600L              // 14.7456MHz processor
//#define CYCLES_PER_US 14.7456				// cpu cycles per microsecond
											// since its all in integer
											// this will lead to 15cycles/us

#define CYCLES_PER_US 16				// cpu cycles per microsecond


#endif
