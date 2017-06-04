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

u16 captureADC(u08 voie)
{
  //u16 temp_ADC=0;
  //ADMUX=(0x40+voie);
  //ADCSRA=0xD7; //0b110100111
  //while(!bit_is_set(ADCSRA,4)); //0b00010000
  //temp_ADC = (u16) ADCL;
  //temp_ADC += (((u16) ADCH) << 8);

  // Left Adjusted & 8 bits resolution
  ADMUX=(0x60+voie);
  ADCSRA=0xF7; //0b11110111
  while(!bit_is_set(ADCSRA,4)); //0b00010000

  return ADCH;
}

//	|	Synchro		|	Adr Recepteur	|	No Commande	|	Nbre d'octets	|	Octet1		|	Octet2		|	Ch Sum	|
void envoi_uart(u08 adr_recepteur, u08 no_command, u08 nbr_data, u08 *data, u16 nb_em)
{
  u08 h=0;
  u08 i=0;
  u08 j=0;
  u08 temp_UDR=0;
  //LED1ON();
  cbi(UCSR0B,4); // Desactive reception
  sbi(VCC_ampli_PORT,VCC_ampli_BIT);
  sbi(TX_enable_PORT,TX_enable_BIT);
  sbi(TX0PORT,TX0BIT); // On Tx
  sbi(UCSR0B,3);  // Autorise transmition
  for(h=0;h<nb_em;h++)
	{
	  temp_UDR=0;
	  for(i=0;i<(nbr_data+5);i++)
		{
		while(!bit_is_set(UCSR0A,5));
		if(i==0)
		  { cbi(UCSR0B,0); } // 9° bit a 0
		else
		  { sbi(UCSR0B,0); } // 9° bit a 1

		if(i==0) // Envoi de la synchronisation 10101010 pour minimiser le taux d'erreur binaire et synchroniser l'uart du recepteur
		  { UDR0=0xAA; }
		else if(i==1)                // Envoi de l'adresse du recepteur concerné
		  { UDR0=adr_recepteur; }
		else if(i==2)                // Envoi du numero de commande
		  { UDR0=no_command; }
		else if(i==3)                // Envoi du nombre de data utiles dans la trame
		  { UDR0=nbr_data; }
		else if(i>3 && i<nbr_data+4) // Envoi data
		  { UDR0=*(data+i-4); }
		else if(i==nbr_data+4)     // Envoi du check sum
		  {
		  for(j=0;j<nbr_data;j++) { temp_UDR= (temp_UDR+*(data+j)); }
		  temp_UDR = adr_recepteur + no_command + nbr_data + temp_UDR;
		  UDR0 = temp_UDR;
		  }
		}
    }
  while(!bit_is_set(UCSR0A,5));
  cbi(UCSR0B,3);  // Desactive transmition
  cbi(TX0PORT,TX0BIT); // RAZ Tx
  cbi(TX_enable_PORT,TX_enable_BIT);
  cbi(VCC_ampli_PORT,VCC_ampli_BIT);
  sbi(UCSR0B,4); // Active reception
  //LED1OFF();
}

ISR(USART0_RX_vect)
{
  u08 i=0;
  if(bit_is_clear(UCSR0B,1) && bit_is_clear(UCSR0A,4))
    {
      buf_uart[0]=UDR0;
	  curseur=0;buf_uart[3]=0xFC; buf_uart[2]==0x00; buf_uart[1]==0x00;
    }
  else
    {
	  if(bit_is_set(UCSR0B,1) && bit_is_clear(UCSR0A,4) && bit_is_clear(UCSR0A,3) && (buf_uart[0]==0xAA))
        {
		  curseur++;
		  buf_uart[curseur]=UDR0;
		  //LED1ON();
		}
	  else
	    {
		  TRASH=UDR0; curseur=0;
		  buf_uart[3]=0xFC; buf_uart[2]==0x00; buf_uart[1]==0x00; buf_uart[0]==0x00;
		  //LED1OFF();
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
		      if(buf_uart[2]==commande_on_avec_US)
			    {
				  sleep_mode();
				  mode_alarme=commande_on_avec_US;
				  envoi_uart(adr_telecommande, commande_on_avec_US, 0, (u08*) &commande_on_avec_US, NBRE_EMISSIONS);
				  Sirene_ON(3);
				  SCRUTATION_ON;USEMON();EIMSK=0x20;
				  //LED2ON();
				//uart_printf("COMMANDE ON AVEC US !\n");
				}
			  else if(buf_uart[2]==commande_on_sans_US)
			    {
				  sleep_mode();
				  mode_alarme=commande_on_sans_US;
				  envoi_uart(adr_telecommande, commande_on_sans_US, 0, (u08*) &commande_on_sans_US, NBRE_EMISSIONS);
				  //EIMSK = 0x10; // Choc
				  Sirene_ON(2);
				  SCRUTATION_ON;
				  //LED2ON();
				//uart_printf("COMMANDE ON SANS US !\n");
				}
			  else if(buf_uart[2]==commande_off)
			    {
				  sleep_mode();
				  envoi_uart(adr_telecommande, commande_off, 0, (u08*) &commande_off, NBRE_EMISSIONS);
				  Sirene_ON(1);
				  //LED2OFF();
				//uart_printf("COMMANDE OFF !\n");
				}
			  else if(buf_uart[2]==commande_etat)
			    {
				  //ARRET_US;
				  if((TCCR3B!=0x00) && (bit_is_set(USEMPORT,USEMBIT)))
					envoi_uart(adr_telecommande, commande_etat, 1, (u08*) &commande_on_avec_US, NBRE_EMISSIONS);
				  else if((TCCR3B!=0x00) && (bit_is_clear(USEMPORT,USEMBIT)))
					envoi_uart(adr_telecommande, commande_etat, 1, (u08*) &commande_on_sans_US, NBRE_EMISSIONS);
				  else
					envoi_uart(adr_telecommande, commande_etat, 1, (u08*) &commande_off, NBRE_EMISSIONS);
				//uart_printf("COMMANDE ETAT : %d\n",commande_alerte);
				}
			  else if(buf_uart[2]==commande_batt_voiture)
			    {
				  //ARRET_US;
				  niveau_batterie=captureADC(BATTERIEBIT0);
				  envoi_uart(adr_telecommande, commande_batt_voiture, 1, (u08*) &niveau_batterie, 10);
				//uart_printf("COMMANDE BATTERIE : %d\n",niveau_batterie);
				}
		    }
	    }
	  //LED1OFF();
	  curseur=0;
	  buf_uart[3]=0xFC; buf_uart[2]=0x00; buf_uart[1]=0x00; buf_uart[0]=0x00;
	}
}

void uart_putc(u08 data)
{
  PORTG=GSM_enable;
  sbi(UCSR1B,0);
  while(!bit_is_set(UCSR1A,5));
  UDR1=data;
  while(!bit_is_set(UCSR1A,5));
  //PORTG=GSMGPS_disable;
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
	  { while(!bit_is_set(UCSR1A,5)); uart_putc (*format); }
	else
	  {
		temp_arg = va_arg(ap,unsigned int);
		format++;
        switch (*format)
	      {
			case 'b':
			  i=8;
			  do { i--; while(!bit_is_set(UCSR1A,5)); if(bit_is_set(temp_arg,i)) {uart_putc('1');} else {uart_putc('0');} } while(i);
			  break;
			case 'c':
			  while(!bit_is_set(UCSR1A,5)); uart_putc(temp_arg);
			  break;
			case 'd':
			  div_val = 10000;
			  if (temp_arg < 0) {temp_arg = - temp_arg; while(!bit_is_set(UCSR1A,5)); uart_putc('-');}
			  while (div_val > 1 && div_val > temp_arg) { div_val /= DEC; }
			  do { while(!bit_is_set(UCSR1A,5)); uart_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= DEC; }
			  while (div_val);
			  break;
			case 'x':
			  div_val = 0x10;
			  do { while(!bit_is_set(UCSR1A,5)); uart_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= HEX; }
			  while (div_val);
			  break;
			case 'm':
			  div_val = 0x1000;
			  do { while(!bit_is_set(UCSR1A,5)); uart_putc(hex[temp_arg / div_val]); temp_arg %= div_val; div_val /= HEX; }
			  while (div_val);
			  break;
			default:
			  while(!bit_is_set(UCSR1A,5)); uart_putc(*format);
			  break;
		  }
	  }
	format++;
  }
  va_end (ap);
}
