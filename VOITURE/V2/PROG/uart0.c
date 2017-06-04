/* Envoi d'un caractere 8 bits sur liaison série */

#include "global.h"
#include "uart0.h"
#include "fifo.h"
#include "config.h"

FIFO_T uart0_rx_fifo;

void uart0_init(void)
{
  //cbi(UCSR0C,7);  // Reserved Bit
  cbi(UCSR0C,6);  // USART Mode Select : Asynchronous Operation
  cbi(UCSR0C,5);  // Parity Mode : 00 Disabled
  cbi(UCSR0C,4);  // Parity Mode : 00 Disabled
  cbi(UCSR0C,3);  // Stop Bit : 1
  sbi(UCSR0C,2);  // Character Size : 8-bit
  sbi(UCSR0C,1);  // Character Size : 8-bit
  cbi(UCSR0C,0);  // Clock Polarity : Falling Edge

  UBRR0H=0x00;     // Baud Rate
  UBRR0L=0x0C;     // Baud Rate 38400 bps

  cbi(UCSR0B,0);  // Transmit Data Bit 8 : 0
  cbi(UCSR0B,1);  // Receive Data Bit 8 : 0
  cbi(UCSR0B,2);  // Character Size : Disabled
  sbi(UCSR0B,3);  // Transmitter Enable : Enabled
  sbi(UCSR0B,4);  // Receiver Enable : Enabled
  cbi(UCSR0B,5);  // USART Data Register Empty Interrupt Enable : Disabled
  cbi(UCSR0B,6);  // TX Complete Interrupt Enable : Disabled
  sbi(UCSR0B,7);  // RX Complete Interrupt Enable : Enabled

  fifo_init(&uart0_rx_fifo);

  sbi(UART0_RX_PORT, UART0_RX_BIT);
  cbi(UART0_RX_DDR , UART0_RX_BIT);

  cbi(UART0_TX_PORT, UART0_TX_BIT);
  sbi(UART0_TX_DDR , UART0_TX_BIT);
}

ISR(USART0_RX_vect)
{
  u08 data=0;
  data=UDR0;
  fifo_put(&uart0_rx_fifo, data);
  /* Give SEM to config cycle because uart0 received data are analysed by config cycle */
  config_sem = CONFIG_SEM_GET;
}

void uart0_putc(u08 data)
{
  while(!bit_is_set(UCSR0A,5));
  UDR0=data;
}


void uart0_puts(u08 *s)
{
  u08 c;
  /* Put until end of string */
  while ( (c = *s++) ) 
    {
      uart0_putc(c);
    }
}

void uart0_puts_P(const u08* texte)
{
  while(pgm_read_byte(texte))
  { uart0_putc(pgm_read_byte(texte)); texte++; }
}

void uart0_putd(u08 type, u32 data)
{
  u08 i=8;
  u32 div_val=0;
  static const unsigned char hex[] = "0123456789ABCDEF";
  switch (type)
  {
    case 'b':
      i=8;
      do { i--; if(bit_is_set(data,i)) {uart0_putc('1');} else {uart0_putc('0');} } while(i);
      break;
    case 'c':
      uart0_putc(data);
      break;
    case 'd':
      div_val = 10000;
      while (div_val > 1 && div_val > data) { div_val /= DEC; }
      do { uart0_putc(hex[data / div_val]); data %= div_val; div_val /= DEC; }
      while (div_val);
      break;
    case 'x':
      div_val = 0x00000010;
      do { uart0_putc(hex[data / div_val]); data %= div_val; div_val /= HEX; }
      while (div_val);
      break;
    case 'm':
      div_val = 0x10000000;
      do { uart0_putc(hex[data / div_val]); data %= div_val; div_val /= HEX; }
      while (div_val);
      break;
    default:
      uart0_putc(type);
      break;
  }
}
