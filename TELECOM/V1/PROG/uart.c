/* Envoi d'un caractere 8 bits sur liaison série */
/* Possibilité d'envoyer un 9° bits */

#include "global.h"
#include "uart.h"
#include "timer.h"
#include <stdio.h>
#include <stdarg.h>

// Variables de reception
volatile u08 buf_uart[256]={0};
volatile u08 TRASH=0;
volatile u08 chk_sum=0;
volatile u08 curseur=0;

//	|	Synchro		|	Adr Recepteur	|	No Commande	|	Nbre d'octets	|	Octet1		|	Octet2		|	Ch Sum	|
void envoi_uart(u08 adr_recepteur, u08 no_command, u08 nbr_data, u08 *data, u16 nb_em)
{
  u08 h=0;
  u08 i=0;
  u08 j=0;
  u08 temp_UDR=0;
  cbi(UCSRB,4);  // Desactive reception
  sbi(TXPORT,TXBIT);
  sbi(UCSRB,3); // Autorise transmition
  for(h=0;h<nb_em;h++)
	{
	  temp_UDR=0;
	  for(i=0;i<(nbr_data+5);i++)
		{
		while(!bit_is_set(UCSRA,5));
		if(i==0)
		  { cbi(UCSRB,0); } // 9° bit a 0
		else
		  { sbi(UCSRB,0); } // 9° bit a 1

		if(i==0) // Envoi de la synchronisation 10101010 pour minimiser le taux d'erreur binaire et synchroniser l'uart du recepteur
		  { UDR=0xAA; }
		else if(i==1)                // Envoi de l'adresse du recepteur concerné
		  { UDR=adr_recepteur; }
		else if(i==2)                // Envoi du numero de commande
		  { UDR=no_command; }
		else if(i==3)                // Envoi du nombre de data utiles dans la trame
		  { UDR=nbr_data; }
		else if(i>3 && i<nbr_data+4) // Envoi data
		  { UDR=*(data+i-4); }
		else if(i==(nbr_data+4))     // Envoi du check sum
		  {
		  for(j=0;j<nbr_data;j++) { temp_UDR= (temp_UDR+*(data+j)); }
		  temp_UDR = adr_recepteur + no_command + nbr_data + temp_UDR;
		  UDR = temp_UDR;
		  }
		}
	}
  while(!bit_is_set(UCSRA,5));
  cbi(UCSRB,3); // Desactive transmition
  cbi(TXPORT,TXBIT);
  sbi(UCSRB,4);  // Autorise reception
}

ISR(USART_RXC_vect)
{
  u08 i=0;
  if(bit_is_clear(UCSRB,1) && bit_is_clear(UCSRA,4))
    {
      buf_uart[0]=UDR;
	  curseur=0;buf_uart[3]=0xFC; buf_uart[2]==0x00; buf_uart[1]==0x00;
    }
  else 
    {
	  if(bit_is_set(UCSRB,1) && bit_is_clear(UCSRA,4) && bit_is_clear(UCSRA,3) && (buf_uart[0]==0xAA))
        {
		  curseur++;
		  buf_uart[curseur]=UDR;
		}
	  else
	    {
		  TRASH=UDR; curseur=0;
		  buf_uart[3]=0xFC; buf_uart[2]==0x00; buf_uart[1]==0x00; buf_uart[0]==0x00;
		}
	}

  if(curseur>(buf_uart[3]+3))
    {
	  if((buf_uart[0]==0xAA) && (buf_uart[1]==adr_device))
	    {
		  chk_sum=0;
	      for(i=1;i<(buf_uart[3]+4);i++)
		    { chk_sum=(u08) (chk_sum+buf_uart[i]); }
		  if(chk_sum==buf_uart[buf_uart[3]+4])
		    {		// liste des commandes
//		      if(buf_uart[2]==commande_on_avec_US)
//			    {
//				  LED2OFF();
//				}
//			  else if(buf_uart[2]==commande_on_sans_US)
//			    {
//				  LED3OFF();
//				}
//			  else if(buf_uart[2]==commande_off)
//			    {
//				  LED4OFF();
//				}
//			  else
			  if(buf_uart[2]==commande_etat)
			    {
				  //LED2OFF();
				  if(buf_uart[4]==commande_on_avec_US)
			    	{
					  LED2ON();
					}
				  else if(buf_uart[4]==commande_on_sans_US)
				    {
					  LED3ON();
					}
				  else if(buf_uart[4]==commande_off)
				    {
					  LED4ON();
					}
				}
			  else if(buf_uart[2]==commande_batt_voiture)
			    {
				  if(buf_uart[4]>200)
					{
					  LED1ON();LED2ON();LED3ON();LED4ON();
					}
				  else if((buf_uart[4]<=200) && (buf_uart[4]>180))
					{
					  LED1OFF();LED2ON();LED3ON();LED4ON();
					}
				  else if((buf_uart[4]<=180) && (buf_uart[4]>150))
					{
					  LED1OFF();LED2OFF();LED3ON();LED4ON();
					}
				  else if((buf_uart[4]<=150) && (buf_uart[4]>0))
					{
					  LED1OFF();LED2OFF();LED3OFF();LED4ON();
					}
				  else
					{
					  LED1OFF();LED2OFF();LED3OFF();LED4OFF();
					}
				}
			  else if(buf_uart[2]==commande_alerte_choc)
			    {
				  TCCR0=0x03;   // CLK/64 declenchement timer0
				  // TCCR2=0x03;   // CLK/64 declenchement timer2
				}
			  else if(buf_uart[2]==commande_alerte_US)
			    {
				  TCCR0=0x03;   // CLK/64 declenchement timer0
				  // TCCR2=0x03;   // CLK/64 declenchement timer2
				}
			  else if(buf_uart[2]==commande_alerte_batterie)
			    {
				  TCCR0=0x03;   // CLK/64 declenchement timer0
				  // TCCR2=0x03;   // CLK/64 declenchement timer2
				}
		    }
	    }
	  curseur=0;
	  buf_uart[3]=0xFC; buf_uart[2]=0x00; buf_uart[1]=0x00; buf_uart[0]=0x00;
	}
}

void uart_putc(u08 data)
{
  sbi(UCSRB,0);
  while(!bit_is_set(UCSRA,5));
  UDR=data;
}


void uart_puts(u08 *s)
{
  u08 c;
  while ( (c = *s++) ) 
    {
      uart_putc(c);
    }
}


void uart_printf(const unsigned char *format, ...)
{
  static const unsigned char hex[] = "0123456789ABCDEF";
  unsigned int div_val, temp_arg;
  u08 i=8;
  va_list ap;

  va_start (ap, format);
  while(*format != '\0')
  {
	if(*format != '%')
	  { while(!bit_is_set(UCSRA,5)); uart_putc (*format); }
	else
	  {
		temp_arg = va_arg(ap,unsigned int);
		format++;
        switch (*format)
	      {
			case 'b':
			  i=8;
			  do { i--; while(!bit_is_set(UCSRA,5)); if(bit_is_set(temp_arg,i)) {uart_putc('1');} else {uart_putc('0');} } while(i);
			  break;
			case 'c':
			  while(!bit_is_set(UCSRA,5)); uart_putc(temp_arg);
			  break;
			case 'd':
			  div_val = 10000;
			  if (temp_arg < 0) {temp_arg = - temp_arg; while(!bit_is_set(UCSRA,5)); uart_putc('-');}
			  while (div_val > 1 && div_val > temp_arg) { div_val /= DEC; }
			  do { while(!bit_is_set(UCSRA,5)); uart_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= DEC; }
			  while (div_val);
			  break;
			case 'x':
			  div_val = 0x10;
			  do { while(!bit_is_set(UCSRA,5)); uart_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= HEX; }
			  while (div_val);
			  break;
			case 'm':
			  div_val = 0x1000;
			  do { while(!bit_is_set(UCSRA,5)); uart_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= HEX; }
			  while (div_val);
			  break;
			default:
			  while(!bit_is_set(UCSRA,5)); uart_putc(*format);
			  break;
		  }
	  }
	format++;
  }
  va_end (ap);
}
