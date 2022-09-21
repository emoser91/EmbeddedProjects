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
	uart.c
	contains functions to control the uart
	Copyright (c) 2004 Joerg Mossbrucker <mossbruc@msoe.edu>
*/

#ifndef _UART_C
#define _UART_C

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include <MSOE/uart.h>


//uart globals
static volatile uint8_t *uart_tx_data_ptr;			// pointer to data to be sent
static volatile uint8_t uart_tx_counter;			// counter of bytes sent

static volatile uint8_t	uart_rx_buffer[BUFFERSIZE];	// rx ring buffer
static volatile uint8_t	uart_rx_buffer_wr_ptr;		// rx ring buffer write pointer
static volatile uint8_t uart_rx_buffer_rd_ptr;		// rx ring buffer read pointer	

// uart tx ready interrupt handler
ISR(USART_TXC_vect)      
{
    uart_tx_data_ptr++;

    if (--uart_tx_counter)
        UDR= *uart_tx_data_ptr;       // write byte to data buffer
}


// uart rx complete interrupt handler
ISR(USART_RXC_vect)      
{
	uart_rx_buffer[uart_rx_buffer_wr_ptr]=UDR; // save byte in ring buffer
	uart_rx_buffer_wr_ptr++;						// increment write pointer
	if(uart_rx_buffer_wr_ptr>=BUFFERSIZE)			// if write pointer at end
		uart_rx_buffer_wr_ptr=0;					// 		set to beginning
}

// receive a byte from uart
uint8_t uart_receive_byte(void)
{
	uint8_t	tmp;
    while(uart_rx_buffer_wr_ptr==uart_rx_buffer_rd_ptr)	// wait until byte received
    ;													// 	do nothing
    tmp=uart_rx_buffer_rd_ptr;							// save current read pointer
    uart_rx_buffer_rd_ptr++;							// increment read pointer
	if(uart_rx_buffer_rd_ptr>=BUFFERSIZE)				// if read pointer at end
		uart_rx_buffer_rd_ptr=0;						//		set to beginning
    	
    return uart_rx_buffer[tmp];    						// read byte from ring buffer
}

// send a byte to uart
void uart_send_byte(uint8_t tx_char)
{
	uart_send(&tx_char,1);								// send one byte
}

// send buffer *buf to uart
void uart_send(uint8_t *buf, uint8_t no_of_bytes)
{   
    if (!uart_tx_counter)
    {
        uart_tx_data_ptr  = buf;      		// set global buffer ptr to buffer
        uart_tx_counter   = no_of_bytes;	// set no. of characters to be sent
        UDR= *buf;					// send first byte to UDR
    }
}

// initialize uart
void uart_init(void)
{
    UCSRB=(1<<RXCIE)|(1<<TXCIE)|(1<<RXEN)|(1<<TXEN);	// enable RxD/TxD and interrupts
    UBRRL=(uint8_t)UART_BAUD_SELECT; 				// set baud rate
    uart_rx_buffer_wr_ptr=0;								// write pointer of rx ring buffer at start
    uart_rx_buffer_rd_ptr=0;								// read pointer of rx ring buffer at start
}

#endif
