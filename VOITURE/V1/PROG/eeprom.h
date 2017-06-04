#ifndef _EEPROM_
#define _EEPROM_

# define EERIE 3
# define EEMWE 2
# define EEWE 1
# define EERE 0


u08 read_eeprom(u16 address_eeprom);
void write_eeprom(u16 address_eeprom,u08 data_eeprom);

#endif
