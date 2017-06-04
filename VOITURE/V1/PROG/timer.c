#include "global.h"
#include "gps.h"
#include "gsm.h"
#include "uart.h"
#include "timer.h"

volatile u16 index_sirene=0;

// RAZ du système pour mise en veille
// uart_rec, int0, int1, gsm_rec, timer_US, timer_led, comparateur_batterie
void sleep_mode(void)
{
  sbi(MCUCR,5);
  EIMSK=0x00;
  EIFR=0xFF;
  TCCR0=0x00;
  TCCR1B=0x00;
  TCCR2=0x00;
  TCCR3B=0x00;
  TCNT0=0x00;
  TCNT1=0x0000;
  TCNT2=0x00;
  TCNT3=0x0000;
  USEMPORT=0x0F;
  BUZZERPORT=0x7D;
  cbi(UCSR0B,3);  // Desactive transmition
  cbi(PORTE,1); // RAZ Tx
  cbi(TX_enable_PORT,TX_enable_BIT);
  cbi(VCC_ampli_PORT,VCC_ampli_BIT);
  GSMGPSPORT=GSMGPS_disable;
  coordonnees_gps[0]=0x00;
  distance_US_min=0x0000;
  distance_US_max=0xFFFF;
  nbre_mesures_US=0;
  mesures_US=0;
  nbre_US_alerte=0;
  nbre_bips_sirene=2;
  nbre_bips_us=0;
  etat_contact = (CONTACTPIN & (1<<CONTACTBIT0 | 1<<CONTACTBIT1 | 1<<CONTACTBIT2));
  etat_capteur = (CAPTEURPIN & (1<<CAPTEURBIT0 | 1<<CAPTEURBIT1 | 1<<CAPTEURBIT2 | 1<<CAPTEURBIT3));
  commande_alerte=0;
  mode_alarme=0;
  LED2OFF();LED3OFF();LED4OFF();
}

/*
// Generation US
ISR(TIMER0_OVF_vect)
{
  TCNT0=US_FREQ_H;
  TCCR1B=0x01;   // CLK/1 declenchement timer1
  USEMTOGGLE();  // Emission des US
  if(nbre_bips_us>0)
    {
	  nbre_bips_us--;
	  if(nbre_bips_us==0)
		{
		  TCCR0=0x00;  // Arret de compteur
		  USEMOFF();   // Arret des emissions US
		}
	}
}
*/

/*
// Temps US
ISR(TIMER1_OVF_vect)
{
  //nbre_US_alerte++;
}
*/

void Sirene_ON(u32 bips_sirene)
{
  nbre_bips_sirene=8*bips_sirene;
  sbi(SIRENEPORT,SIRENE0BIT);
  sbi(SIRENEPORT,SIRENE1BIT);
  //LED3ON();
  SIRENE_ON;
}

// Sirene
ISR(TIMER2_OVF_vect)
{
  //ARRET_US;
  if(nbre_bips_sirene>0)
    {
	  nbre_bips_sirene--;
	  if(nbre_bips_sirene==0)
		{
		  SIRENE_OFF;
		  cbi(SIRENEPORT,SIRENE0BIT);
  		  cbi(SIRENEPORT,SIRENE1BIT);
		  //LED3OFF();
		}
	}
}
/*
  ARRET_US;

  if(index_sirene<SIRENE_TPS_H)
    {
	  TCNT2=SIRENE_FREQ_H;
	  if(bit_is_set(SIRENEPIN,SIRENE1BIT)) {cbi(SIRENEPORT,SIRENE1BIT);} else {sbi(SIRENEPORT,SIRENE1BIT);}
	  sbi(SIRENEPORT,SIRENE0BIT);
	  BUZZERTOGGLE();
	  index_sirene++;
	  LED3ON();
	}
  else if(index_sirene<SIRENE_TPS_L)
	{
	  TCNT2=SIRENE_FREQ_L;
	  cbi(SIRENEPORT,SIRENE1BIT);
	  cbi(SIRENEPORT,SIRENE0BIT);
	  BUZZERPORT=(BUZZERPORT & 0x7D);
	  index_sirene++;
	  LED3OFF();
	}
  else
    {
	  TCNT2=SIRENE_FREQ_L;
	  index_sirene=0;
	  if(nbre_bips_sirene>0)
	    {
		  nbre_bips_sirene--;
		  if(nbre_bips_sirene==0)
			{
			  TCCR2=0x00;
			  cbi(SIRENEPORT,SIRENE1BIT);
	  		  cbi(SIRENEPORT,SIRENE0BIT);
	  		  BUZZERPORT=(BUZZERPORT & 0x7D);
			}
		}
	}
*/


// SCRUTATION & ENVOI ALERTE & DECLENCHEMENT US
ISR(TIMER3_OVF_vect)
{
  // Tension batterie faible
  if((niveau_batterie=captureADC(BATTERIEBIT0))<NIVEAU_BATTERIE_FAIBLE)
	{
	  commande_alerte=commande_alerte_batterie;
	//uart_printf("Batterie = %d\n",niveau_batterie);
	}
  // Scrutation des contacts
  if(etat_contact != (CONTACTPIN & (1<<CONTACTBIT0 | 1<<CONTACTBIT1 | 1<<CONTACTBIT2)) || etat_capteur != (CAPTEURPIN & (1<<CAPTEURBIT0 | 1<<CAPTEURBIT1 | 1<<CAPTEURBIT2 | 1<<CAPTEURBIT3)))
	{
	  commande_alerte=commande_alerte_choc;
	  Sirene_ON(NB_BIPS_ALERTE_CONTACTS);
	}
  // Intrusion détectée et envoi par radio
  if(commande_alerte)
	{
	  TCCR3B=0x04;
	  //gps_coord();
	  envoi_uart(adr_telecommande, commande_alerte, 0, (u08*) &commande_alerte, NBRE_EMISSIONS);
	  //gsm_send(coordonnees_gps);
	//uart_printf("Envoi Alerte\n");
	}
  // Declenchement US
  //if((mode_alarme==commande_on_avec_US) && (nbre_bips_sirene==0))
	//{
	  //nbre_bips_us=13;
	  //TCNT0=US_FREQ_H;
	  //TCNT1=0x0000;
	  //TCCR1B=0x00;
	  //TCCR0=0x01;
	  //EIMSK=0x30;
	//}
}
