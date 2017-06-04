#include "global.h"
#include "gsm.h"

void gsm_send(u08 *texte)
{
  u08 i=0;
  GSMGPSPORT=GSM_enable;
  while(texte[i] != '\0')
    {
      while(!bit_is_set(UCSR1A,5));
      UDR1 = texte[i];
	  i++;
	}
  while(!bit_is_set(UCSR1A,5));
  GSMGPSPORT=GSMGPS_disable;
}
