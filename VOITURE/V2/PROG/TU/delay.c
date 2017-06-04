/****************************************************************************
 Titre	:   Lecture ecriture dans l'eeprom
 Auteur:    LeBomb
 Date:      02/09/2004
 Software:  WinAVR
 Cible:     Microcontroleur Atmel AT90S4414/8515/Mega

 DESCRIPTION
       Routines de base pour creer un delai
	   N'utilise pas de timer
	   Les delais sont peu precis
 UTILISATION
       Liste des fonctions utiles dans attente.h
	   Definir la valeur du quartz dans global.h
*****************************************************************************/

# include "global.h"
# include "delay.h"

void delay_3cyc(u08 dec) 
     // 3 cycles, 8 bits
{
  while ( dec ) dec--;
}


void delay_4cyc(u16 dec) 
     // 4 cycles, 16 bits
{
    while ( dec ) dec--;
}

void delay_us(u16 us)
{
  us=((us*(QUARTZ/4000))-3);
  while(us)
    us--;
}


void delay_ms(u16 ms) 
{
  u16 dec;
    while ( ms ) 
      {
	dec = ( (QUARTZ / 4) - 1);
	while ( dec-- ) // attention aux simplifications a la compilation
    rien();
	ms--;// gcc optimise souvent, ce qui n est pas pratique ici
      }
}
