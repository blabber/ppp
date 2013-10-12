/*-
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tobias.rehbein@web.de> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.
 *                                                             Tobias Rehbein
 */

/*
 * Portable Pingie Pie
 */

#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "lcd.h"

#define LINELEN 16

void wrap_out(char *s);

char line1[LINELEN + 1] = {0};
char line2[LINELEN + 1] = {0};

char input[UART_BUFFLEN];

int
main(void)
{
	lcd_init();
	uart_init();

	sei();

	for (;;) {
		if (uart_receive_done != 0)
			uart_gets(input, UART_BUFFLEN);

		wrap_out(input);
	}

	return (0);
}

void
wrap_out(char *s)
{
	*line1 = *line2 = '\0';

	while (*s != '\0') {
		strncpy(line1, line2, LINELEN);

		char *l = line2, *l0 = NULL;
		char *s0 = s;
		bool loop = true;
		while (loop) {
			if (*s == ' ') {
				s0 = s;
				l0 = l;
			}
			*l++ = *s++;

			if (*s == '\0') {
				*l = '\0';
				loop = false;
			}

			if (l == line2 + LINELEN) {
				if (l0 != NULL) {
					*l0 = '\0';
					s = ++s0;
				}
				loop = false;
			}
		}
		
		lcd_clr();
		lcd_puts(line1);
		lcd_select_line(2);
		lcd_puts(line2);
		_delay_ms(1000);
	}

	strncpy(line1, line2, LINELEN);
	*line2 = '\0';
	lcd_clr();
	lcd_puts(line1);
	lcd_select_line(2);
	lcd_puts(line2);
	_delay_ms(1000);
}
