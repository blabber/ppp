/*-
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tobias.rehbein@web.de> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.
 *                                                             Tobias Rehbein
 */

#include <stdint.h>

void lcd_init(void);
void lcd_clr(void);
void lcd_putc(char c);
void lcd_puts(char *s);
void lcd_select_line(uint8_t l);
