#ifndef _UART0_
#define _UART0_

#include "global.h"
#include "fifo.h"
#include "config.h"

#define UART0_RX_DDR  DDRE
#define UART0_RX_PORT PORTE
#define UART0_RX_PIN  PINE
#define UART0_RX_BIT  0

#define UART0_TX_DDR  DDRE
#define UART0_TX_PORT PORTE
#define UART0_TX_PIN  PINE
#define UART0_TX_BIT  1

extern FIFO_T uart0_rx_fifo;

void uart0_init(void);
void uart0_putc(u08 data);
void uart0_puts(u08 *s);
void uart0_puts_P(const u08* texte);
void uart0_putd(u08 type, u32 data);

#define DEBUG0_PUTS(t)   { if(config_keyboard.debug0) { uart0_puts(t); } }
#define DEBUG0_PUTS_P(t) { if(config_keyboard.debug0) { uart0_puts_P(t); } }
#define DEBUG0_PUTD(t,d) { if(config_keyboard.debug0) { uart0_putd(t,d); } }

#endif
