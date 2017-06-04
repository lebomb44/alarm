/* Envoi d'un caractere 8 bits sur liaison série */

#include "global.h"
#include "uart1.h"
#include "fifo.h"
#include "gm862.h"

FIFO_T uart1_rx_fifo;

void uart1_init(void)
{
  //cbi(UCSR1C,7);  // Reserved Bit
  cbi(UCSR1C,6);  // USART Mode Select : Asynchronous Operation
  cbi(UCSR1C,5);  // Parity Mode : 00 Disabled
  cbi(UCSR1C,4);  // Parity Mode : 00 Disabled
  cbi(UCSR1C,3);  // Stop Bit : 1
  sbi(UCSR1C,2);  // Character Size : 8-bit
  sbi(UCSR1C,1);  // Character Size : 8-bit
  cbi(UCSR1C,0);  // Clock Polarity : Falling Edge

  UBRR1H=0x00;     // Baud Rate
  UBRR1L=0x0C;     // Baud Rate 38400 bps

  cbi(UCSR1B,0);  // Transmit Data Bit 8 : 0
  cbi(UCSR1B,1);  // Receive Data Bit 8 : 0
  cbi(UCSR1B,2);  // Character Size : Disabled
  sbi(UCSR1B,3);  // Transmitter Enable : Enabled
  sbi(UCSR1B,4);  // Receiver Enable : Enabled
  cbi(UCSR1B,5);  // USART Data Register Empty Interrupt Enable : Disabled
  cbi(UCSR1B,6);  // TX Complete Interrupt Enable : Disabled
  sbi(UCSR1B,7);  // RX Complete Interrupt Enable : Enabled

  fifo_init(&uart1_rx_fifo);

  sbi(UART1_RX_PORT, UART1_RX_BIT);
  cbi(UART1_RX_DDR , UART1_RX_BIT);

  cbi(UART1_TX_PORT, UART1_TX_BIT);
  sbi(UART1_TX_DDR , UART1_TX_BIT);
}

ISR(USART1_RX_vect)
{
  u08 data=0;
  data=UDR1;
  fifo_put(&uart1_rx_fifo, data);
  /* Give SEM to receive cycle only if receive is activated */
  if(gm862_receive_state ==  GM862_RECEIVE_STATE_ON)
  {
    gm862_receive_sem = GM862_RECEIVE_SEM_GET;
  }
}

void uart1_putc(u08 data)
{
  while(!bit_is_set(UCSR1A,5));
  UDR1=data;
}


void uart1_puts(u08 *s)
{
  u08 c;
  /* Put until end of string */
  while ( (c = *s++) ) 
    {
      uart1_putc(c);
    }
}

void uart1_puts_P(const u08* texte)
{
  while(pgm_read_byte(texte))
  { uart1_putc(pgm_read_byte(texte)); texte++; }
}

void uart1_putd(u08 type, u32 data)
{
  u08 i=8;
  u32 div_val=0;
  static const unsigned char hex[] = "0123456789ABCDEF";
  switch (type)
  {
    case 'b':
      i=8;
      do { i--; if(bit_is_set(data,i)) {uart1_putc('1');} else {uart1_putc('0');} } while(i);
      break;
    case 'c':
      uart1_putc(data);
      break;
    case 'd':
      div_val = 10000;
      while (div_val > 1 && div_val > data) { div_val /= DEC; }
      do { uart1_putc(hex[data / div_val]); data %= div_val; div_val /= DEC; }
      while (div_val);
      break;
    case 'x':
      div_val = 0x00000010;
      do { uart1_putc(hex[data / div_val]); data %= div_val; div_val /= HEX; }
      while (div_val);
      break;
    case 'm':
      div_val = 0x10000000;
      do { uart1_putc(hex[data / div_val]); data %= div_val; div_val /= HEX; }
      while (div_val);
      break;
    default:
      uart1_putc(type);
      break;
  }
}
