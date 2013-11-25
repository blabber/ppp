/*-
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tobias.rehbein@web.de> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.
 *                                                             Tobias Rehbein
 */

/*
 * Pingie Pie Portable
 */

#include <string.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "lcd.h"

#define LED		PB5
#define BUTTON		PD2
#define BACKLIGHT	PB4

#define PERM_BACKLIGHT	(1<<0)

#define LINELEN		16
#define BACKLIGHT_TIMES	2

void wrap_out(char *s);

char line1[LINELEN + 1] = {0};
char line2[LINELEN + 1] = {0};

char input[UART_BUFFLEN];

volatile int8_t status = 0;

int
main(void)
{
	lcd_init();
	uart_init();

	DDRB |= (1<<LED);
	DDRB |= (1<<BACKLIGHT);

	/* setup timer used to blink notification LED (clk/256 prescaler) */
	TCCR1B |= (1<<WGM12);		/* CTC mode */
	TIMSK1 |= (1<<OCIE1A);		/* enable output compare A interrupt */
	OCR1A = (F_CPU / 256 / 2) - 1;	/* half second output compare A */

	/*
	 * Setup  external interrupt and prepare TIMER0 as debounce timer. The
	 * debounce timer will use a (clk/1024) prescaler and use the overflow
	 * interrupt, yielding a debounce time of approximately 16 ms.
	 */
	EICRA |= (1<<ISC01) | (1<<ISC00);	/* INT0 on rising edge */
	EIMSK |= (1<<INT0);
	TIMSK0 |= (1<<TOIE0);

	lcd_puts("*** Ready ***");
	PORTB |= (1<<BACKLIGHT);
	_delay_ms(1000);
	PORTB &= ~(1<<BACKLIGHT);
	lcd_clr();

	sei();

	uint8_t backlight = 0;
	for (;;) {
		if (uart_receive_done != 0) {
			uart_gets(input, UART_BUFFLEN);
			TCCR1B |= (1<<CS12);	/* start LED timer */
			if (!(status & PERM_BACKLIGHT)) {
				PORTB |= (1<<BACKLIGHT);
				backlight = BACKLIGHT_TIMES;
			}
		}

		wrap_out(input);

		/*
		 * If LED timer is running, stop it and make sure that the LED
		 * is turned off.
		 */
		if (TCCR1B & (1<<CS12)) {
			TCCR1B &= ~(1<<CS12);
			PORTB &= ~(1<<LED);
		}

		if ((!(status & PERM_BACKLIGHT)) && (--backlight == 0))
			PORTB &= ~(1<<BACKLIGHT);
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

ISR(TIMER1_COMPA_vect)
{
	PORTB ^= (1<<LED);
}

ISR(TIMER0_OVF_vect)
{
	if (PIND & (1<<BUTTON)) {
		status ^= PERM_BACKLIGHT;

		if (status & PERM_BACKLIGHT)
			PORTB |= (1<<BACKLIGHT);
		else
			PORTB &= ~(1<<BACKLIGHT);
	}

	/*
	 * Stop debounce timer, reset timer counter and clear overflow interrupt
	 * flag.
	 */
	TCCR0B &= ~((1<<CS02) | (1<<CS00));
	TCNT0 = 0;
	TIFR0 |= (1<<TOV0);

	/*
	 * Clear external interrupt INT0 flag and reenable external interrupt
	 * INT0.
	 */
	EIFR |= (1<<INTF0);
	EIMSK |= (1<<INT0);
}

ISR(INT0_vect)
{
	/*
	 * Disable external interrupt INT0 and enable debounce timer (clk/1024)
	 * prescaler.
	 */
	EIMSK &= ~(1<<INT0);
	TCCR0B |= (1<<CS02) | (1<<CS00);
}
