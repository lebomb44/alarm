#ifndef _DELAY_
#define _DELAY_

# include "global.h"

void delay_3cyc(u08 dec);
void delay_4cyc(u16 dec);
void delay_us(u16 us);
void delay_ms(u16 ms);

void attente(u08, u08*);

#endif
