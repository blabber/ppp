/*-
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tobias.rehbein@web.de> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.
 *                                                             Tobias Rehbein
 */

/**
 * Hardware USART
 *
 * 8N1 @ 9600bd
 */

#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "defines.h"
#include "uart.h"

#define BAUD	UART_BAUD
#include <util/setbaud.h>

#define MIN(A,B)	((A) < (B) ? (A) : (B))

volatile uint8_t uart_receive_done = 0;

char in[UART_BUFFLEN];
char *inp = in;

void
uart_init(void)
{
	/* Configure UART */
	UCSR0B |= (1<<RXEN0) |			/* Enable RX */
	    (1<<RXCIE0);			/* Enable RX complete IRQ */
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);	/* Set transmit mode to 8N1 */

	/* Set baud rate */
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
}

void
uart_gets(char *buffer, uint8_t bufflen)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		uart_receive_done = 0;
		*inp = *in;

		strncpy(buffer, in, MIN(UART_BUFFLEN, bufflen));
		buffer[bufflen - 1] = '\0';
	}
}

ISR(USART_RX_vect)
{
	uart_receive_done = 0;
	*inp = UDR0;

	if (*inp == '\r' || ++inp == (in + UART_BUFFLEN -1)) {
		*inp = '\0';
		inp = in;
		uart_receive_done = 1;
		return;
	}
}
