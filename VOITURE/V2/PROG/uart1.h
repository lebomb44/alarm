#ifndef _UART1_
#define _UART1_

#include "global.h"
#include "fifo.h"

#define UART1_RX_DDR  DDRD
#define UART1_RX_PORT PORTD
#define UART1_RX_PIN  PIND
#define UART1_RX_BIT  2

#define UART1_TX_DDR  DDRD
#define UART1_TX_PORT PORTD
#define UART1_TX_PIN  PIND
#define UART1_TX_BIT  3

extern FIFO_T uart1_rx_fifo;

void uart1_init(void);
void uart1_putc(u08 data);
void uart1_puts(u08 *s);
void uart1_puts_P(const u08* texte);
void uart1_putd(u08 type, u32 data);

#endif

