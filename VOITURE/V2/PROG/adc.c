/****************************************************************************
 Titre	:   Lecture sur l ADC
 Auteur:    LeBomb
 Date:      17/02/2010
 Software:  WinAVR
 Cible:     Microcontroleur Atmel AT90S4414/8515/Mega

 DESCRIPTION
       Acquisition 12 bits sur l ADC
 UTILISATION
       Liste des fonctions utiles dans adc.h
*****************************************************************************/

#include "global.h"
#include "adc.h"

void adc_init(void)
{
  /* Turn ON ADC */
  sbi(ADCSRA, 7);
}

u16 adc_get(u08 channel)
{
  u16 temp_ADC=0;
  /* Select Channel using AVCC with external capacitor at AREF pin */
  ADMUX=(0x40+channel);
  /* Launch acquisition div factor 128 */
  ADCSRA=0xD7; //0b110100111
  /* Wait for end of acquisition */
  while(!bit_is_set(ADCSRA,4)); //0b00010000
  /* Compute acquisition */
  temp_ADC = (u16) ADCL;
  temp_ADC += (((u16) ADCH) << 8);

  return temp_ADC;
}
