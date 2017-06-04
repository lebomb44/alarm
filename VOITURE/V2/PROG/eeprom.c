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

u08 eeprom_read(u16 eeprom_address)
{
  /* Wait for completion of previous write */
  while(bit_is_set(EECR,EEWE)) {}
  /* Set up adress */
  EEAR = eeprom_address;
  /* Launch read cycle */
  sbi(EECR,EERE);
  /* Return data */
  return EEDR;
}


void eeprom_write(u16 eeprom_address, u08 eeprom_data)
{
  /* Wait for completion of previous write */
  while(bit_is_set(EECR,EEWE)) {}
  /* Set up address and data registers */
  EEAR = eeprom_address;
  EEDR = eeprom_data;
  /* Launch write cycle */
  sbi(EECR,EEMWE);
  sbi(EECR,EEWE);
  /* Wait for completion of write */
  while(bit_is_set(EECR,EEWE)) {}
}
