/****************************************************************************
 Titre	:   Trames MM53200
 Auteur:    LeBomb
 Date:      20/09/2004
 Software:  WinAVR
 Cible:     Microcontroleur Atmel AT90S4414/8515/Mega

 DESCRIPTION
       Génération des trames d'un MM53200 (equivalent UM3750) pour une horloge a 100 KHz (R=100Kohms et C=180pF)
	   Utilisation : void send_MM53200(u16 code)
	   Inclure send_MM2000(code) dans la fonction d'interruption du timer1 : SIGNAL(SIG_OVERFLOW1)
 UTILISATION
       Liste des fonctions utiles dans MM53200.h
*****************************************************************************/

#include "global.h"
#include "MM53200.h"

volatile u08 cycle_MM53200=0;
volatile u08 pos_MM53200=0;
volatile u16 code_MM53200=0;

void init_MM53200(u16 code)
{
  LED1ON();
  cycle_MM53200=5;
  pos_MM53200=26;
  code_MM53200=code;
  cbi(SIGNALPORT,SIGNALBIT);
  // Initialisations de Timer1
  TCNT1=0x0000;     // Initialisation timer1
  sbi(TIMSK,2);    // Interruption Timer1 owerflow
  sbi(TCCR1B,1);   // CLK/8 declenchement timer1
}

void send_MM53200(void)
{
  if(cycle_MM53200)
  {
	pos_MM53200--;
	if(pos_MM53200 == 25)
	{
	  TCNT1=HIGH1;
	  sbi(SIGNALPORT,SIGNALBIT);
	}
	
	if(pos_MM53200<25 && pos_MM53200>0)
	{
	  if(code_MM53200 & (1<<((pos_MM53200-1)/2))) // Si c'est un 1
	  {
	    if(bit_is_clear(pos_MM53200,0)) // Niveau bas
		{
		  TCNT1=LOW1;
		  cbi(SIGNALPORT,SIGNALBIT);
		}
		else // Niveau haut
		{
		  TCNT1=HIGH1;
		  sbi(SIGNALPORT,SIGNALBIT);
		}
	  }
	  else // Si c'est un 0
	  {
	    if(bit_is_clear(pos_MM53200,0)) // Niveau bas
		{
		  TCNT1=LOW0;
		  cbi(SIGNALPORT,SIGNALBIT);
		}
		else // Niveau haut
		{
		  TCNT1=HIGH0;
		  sbi(SIGNALPORT,SIGNALBIT);
		}
	  }
	}
	if(pos_MM53200 == 0) // Pause entre deux trame
	{
	  TCNT1=0xD3F0; // 11.28 ms 0x4FBF
	  cbi(SIGNALPORT,SIGNALBIT);
	}

	if(pos_MM53200 == 0) // Reinitialisation de la trame
	{
	  cycle_MM53200--;
	  pos_MM53200=26;
	}
  }
  else
  {
	LED1OFF();
	cbi(TCCR1B,0);
  }
}

SIGNAL(SIG_OVERFLOW1)
{
  send_MM53200();
}
