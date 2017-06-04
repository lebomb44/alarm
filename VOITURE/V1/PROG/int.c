#include "global.h"
#include "int.h"
#include "timer.h"
#include "uart.h"

void delay_ms(u16 ms) 
{
  u16 dec;
  while ( ms ) 
    {
	  dec = ( (QUARTZ / 4) - 1);
	  while ( dec-- ) // attention aux simplifications a la compilation
	  nop();
	  ms--;// gcc optimise souvent, ce qui n est pas pratique ici
    }
}

/*
// Choc
ISR(INT4_vect)
{
  //commande_alerte=commande_alerte_choc;
  //Sirene_ON(NB_BIPS_ALERTE_CHOC);
}
*/

// Reception US
ISR(INT5_vect)
{
  commande_alerte=commande_alerte_US;
  Sirene_ON(NB_BIPS_ALERTE_US);
  /*volatile u16 compteur_US;
  TCCR1B=0x00;
  TCCR0=0x00;
  compteur_US=(TCNT1-6000)/469;
  ARRET_US;
//uart_printf("Compteur : %d\n",compteur_US);
  if(compteur_US<distance_US_min || compteur_US>distance_US_max) // Mesure en dehors du domaine
    {
	  nbre_US_alerte++;
	  if(nbre_US_alerte > NBRE_US_ALERTE_MAX)
		{
		  commande_alerte=commande_alerte_US;
		  distance_US_min = 0x0000;
		  distance_US_max = 0xFFFF;
		  nbre_mesures_US=0;
		  mesures_US=0;
		  nbre_US_alerte=0;
		//uart_printf("Alerte US : %d\n",compteur_US);
		//Sirene_ON(NB_BIPS_ALERTE_US);
		}
	}
  else if(nbre_mesures_US < NBRE_MESURES_US_MAX) // Etalonnage
    {
	  mesures_US = mesures_US + compteur_US;
	  nbre_mesures_US++;
	//uart_printf("Etalonnage US : %d...\n",compteur_US);
	  if(nbre_mesures_US == NBRE_MESURES_US_MAX) // Etalonnage fini, calcul des extrema
	    {
		  mesures_US = mesures_US/NBRE_MESURES_US_MAX;
		  distance_US_min = (mesures_US*8) /10;
		  distance_US_max = (mesures_US*12) /10;
		//uart_printf("Etalonnage US fini\n  MIN=%d MAX=%d\n",distance_US_min,distance_US_max);
		}
	}
  else // Pas d'intrusion
	{
	  nbre_US_alerte=0;
	}*/
}
