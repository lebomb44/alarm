#include "global.h"
#include "eeprom.h"
#include "MM53200.h"
#include "timer.h"
#include "uart.h"

u16 captureADC(u08 voie)
{
  ADMUX=(0x60+voie);
  ADCSRA=0xF7; //0b11110111
  while(!bit_is_set(ADCSRA,4)); //0b00010000

  return ADCH;
}

void init(void)
{
  // Initialisations des parametres
  adr_device = 110;//read_eeprom(0x00);
  adr_voiture = 100;//read_eeprom(0x01);
  commande_on_avec_US = 120;//read_eeprom(0x10);
  commande_on_sans_US = 121;//read_eeprom(0x11);
  commande_off = 122;//read_eeprom(0x12);
  commande_etat = 123;//read_eeprom(0x13);
  commande_batt_voiture = 124;//read_eeprom(0x14);
  commande_alerte_choc = 125;//read_eeprom(0x15);
  commande_alerte_US = 126;//read_eeprom(0x16);
  commande_alerte_batterie = 127; //read_eeprom(0x17);
  nbre_bips_sirene = 0;
  // Initialisations de Timer0
  sbi(TIMSK,0);    // Interruption Timer0 owerflow
  TCNT0=0x00;         // Initialisation timer0
  // Initialisations de Timer2
  // sbi(TIMSK,6);    // Interruption Timer2 owerflow
  // TCNT2=0x00;         // Initialisation timer2
  // Initialisation de la transmission
  UBRRH=0x00;    // Baud Rate
  UBRRL=0xCF;    // Baud Rate 2400 bps
  sbi(UCSRB,2);  // Mode X bits
//sbi(UCSRB,3);  // Autorise transmition
  UCSRC=0x86;    // Mode 9 bits
  sbi(UCSRB,7);  // Autorisation de l'Interruption Receiver Enable
}

int main(void)
{
  u32 tempo;
  u08 temp;
  // Initialisations des Ports
  PORTB=0x00;
  PORTC=0x00;
  PORTD=0x00;
  DDRB=VAL_DDRB;
  DDRC=VAL_DDRC;
  DDRD=VAL_DDRD;
  sbi(RXPORT,0);     // Resistance de tirage sur RXD
  sbi(SREG,7);


  if(bit_is_set(BOUTON_MAISON_PIN,BOUTON_MAISON_BIT))
    {
	  init_MM53200(maison);
	  while(bit_is_set(LED1PORT,LED1BIT));
	}
  else
    {
	  init();
	  if(bit_is_set(BOUTON_ON_AVEC_US_PIN,BOUTON_ON_AVEC_US_BIT))
		{
		  LED2ON();
		  envoi_uart(adr_voiture, commande_on_avec_US, 0, (u08*) &commande_on_avec_US, NBRE_EMISSIONS);
		  LED2OFF();
		}
	  else if(bit_is_set(BOUTON_ON_SANS_US_PIN,BOUTON_ON_SANS_US_BIT))
		{
		  LED3ON();
		  envoi_uart(adr_voiture, commande_on_sans_US, 0, (u08*) &commande_on_sans_US, NBRE_EMISSIONS);
		  LED3OFF();
		}
	  else if(bit_is_set(BOUTON_OFF_PIN,BOUTON_OFF_BIT))
		{
		  LED4ON();
		  envoi_uart(adr_voiture, commande_off, 0, (u08*) &commande_off, NBRE_EMISSIONS);
		  LED4OFF();
		}
	  else 
		{
		  tempo=0x0004FFFF;
		  while(tempo--)
			{
			  LED1ON();
			  if(bit_is_set(BOUTON_VIALA_PIN,BOUTON_VIALA_BIT))
				{
				  cbi(UCSRB,3);
				  init_MM53200(viala);
				  while(bit_is_set(LED1PORT,LED1BIT));
				  sbi(UCSRB,3);
				  break;
				}
			  else
				{
				  LED1OFF();LED2ON();
				  if(bit_is_set(BOUTON_ETAT_PIN,BOUTON_ETAT_BIT))
					{
					  LED2OFF();
					  while(1)
						{
						  //LED2ON();
						  envoi_uart(adr_voiture, commande_etat, 0, (u08*) &commande_etat, 10);
						  //LED2OFF();
						  tempo=0x0004FFFF;
						  while(tempo--)
							nop();
						}
						  break;
					}
				  else
					{
					  LED2OFF();LED3ON();
					  if(bit_is_set(BOUTON_BATT_VOITURE_PIN,BOUTON_BATT_VOITURE_BIT))
						{
						  LED3OFF();
						  while(1)
							{
							  //LED3ON();
							  envoi_uart(adr_voiture, commande_batt_voiture, 0, (u08*) &commande_batt_voiture, 10);
							  //LED3OFF();
							  tempo=0x0008FFFF;
							  while(tempo--)
								nop();
							}
							  break;
						}
					  else
						{
						  LED3OFF();LED4ON();
						  if(bit_is_set(BOUTON_BATT_TELE_PIN,BOUTON_BATT_TELE_BIT))
							{
							  while(1)
								{
								  temp=captureADC(0);
								  sbi(UCSRB,3);
								  uart_printf("Batt. Tele : %d\n",temp);
								  if(temp>200)
									{
									  LED1ON();LED2ON();LED3ON();LED4ON();
									}
								  else if((temp<=200) && (temp>180))
									{
									  LED1OFF();LED2ON();LED3ON();LED4ON();
									}
								  else if((temp<=180) && (temp>150))
									{
									  LED1OFF();LED2OFF();LED3ON();LED4ON();
									}
								  else if((temp<=150) && (temp>=0))
									{
									  LED1OFF();LED2OFF();LED3OFF();LED4ON();
									}
								  else
									{
									  LED1OFF();LED2OFF();LED3OFF();LED4OFF();
									}
								}
							}
						  LED1OFF();LED2OFF();LED3OFF();LED4OFF();
						}
					}
				}
			}
		}
	}
  sbi(UCSRB,4);  // Autorise reception
  sleep_mode();

  while(1)
    {
	  sleep();
    }
  return 0;
}
