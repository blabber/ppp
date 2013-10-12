/*-
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tobias.rehbein@web.de> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.
 *                                                             Tobias Rehbein
 */

#define UART_BUFFLEN	255

extern volatile uint8_t uart_receive_done;

void uart_init(void);
void uart_gets(char *buffer, uint8_t bufflen);
