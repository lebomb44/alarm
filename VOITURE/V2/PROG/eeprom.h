#ifndef _EEPROM_
#define _EEPROM_

#include "global.h"

# define EERIE 3
# define EEMWE 2
# define EEWE 1
# define EERE 0

u08 eeprom_read(u16 eeprom_address);
void eeprom_write(u16 eeprom_address,u08 eeprom_data);

#endif
