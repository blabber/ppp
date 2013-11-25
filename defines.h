/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 */


/* UART baud rate */
#define UART_BAUD  9600

/* HD44780 LCD port connections */
#define HD44780_RS D, 5
#define HD44780_RW D, 6
#define HD44780_E  D, 7
/* The data bits have to be not only in ascending order but also consecutive. */
#define HD44780_D4 B, 0

/* Whether to read the busy flag, or fall back to worst-time delays. */
#define USE_BUSY_BIT 1
