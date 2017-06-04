#ifndef _UART_
#define _UART_

#define BIN 2
#define CHAR 8
#define DEC 10
#define HEX 16
#define MOT 32

u16 captureADC(u08 voie);
void envoi_uart(u08 adr_device, u08 no_command, u08 nbr_data, u08 *data, u16);
void uart_putc(u08 data);
void uart_puts(u08 *s);
void uart_printf(const unsigned char *format, ...);

#endif

