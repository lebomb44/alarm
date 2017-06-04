/****************************************************************************
 Titre	:   Lecture ecriture dans l'eeprom
 Auteur:    LeBomb
 Date:      02/09/2004
 Software:  WinAVR
 Cible:     Microcontroleur Atmel AT90S4414/8515/Mega

 DESCRIPTION
       Routines de base pour lire et ecrire dans l'eeprom interne au microcontroleur
	   Lecture sur 16 bits pour l'adresse et 8 bits pour les donnes uniquement
	   Ecriture sur 16 bits pour l'adresse et 8 bits pour les donnees uniquement
	   Lecture/Ecriture par scrutation de flags
	   Donc ne gere pas les signaux d'interruption eeprom_ready
 UTILISATION
       Liste des fonctions utiles dans eeprom.h
*****************************************************************************/

# include "global.h"
# include "eeprom.h"


u08 read_eeprom(u16 address_eeprom)
{
  EEAR = address_eeprom;
  sbi(EECR,EERE);
  return EEDR;
}


void write_eeprom(u16 address_eeprom, u08 data_eeprom)
{
  cli();
  while(bit_is_set(EECR,EEWE)) {}
  EEAR = address_eeprom;
  EEDR = data_eeprom;
  sbi(EECR,EEMWE);
  sbi(EECR,EEWE);
  while(bit_is_set(EECR,EEWE)) {}
}
