#ifndef _BUZZER_
#define _BUZZER_

#include "global.h"

#define BUZZER_POWER_DDR DDRB
#define BUZZER_POWER_PORT PORTB
#define BUZZER_POWER_PIN PINB
#define BUZZER_POWER_BIT 2

void buzzer_init(void);
void buzzer_enable(u16 delay);
void buzzer_disable(void);

#define BIP_DDR DDRE
#define BIP_PORT PORTE
#define BIP_PIN PINE
#define BIP_BIT 7

void bip_init(void);
void bip_enable(u16 delay);

#endif
