#ifndef _KEYBOARD_
#define _KEYBOARD_

#include "global.h"

#define KEYBOARD_INT_DDR  DDRE
#define KEYBOARD_INT_PORT PORTE
#define KEYBOARD_INT_PIN  PINE
#define KEYBOARD_INT_BIT  5

#define KEYBOARD_ADC_DDR  DDRF
#define KEYBOARD_ADC_PORT PORTF
#define KEYBOARD_ADC_PIN  PINF
#define KEYBOARD_ADC_BIT  3

void keyboard_init(void);
u08 keyboard_touch_get(void);
void keyboard_cycle(void);

#endif
