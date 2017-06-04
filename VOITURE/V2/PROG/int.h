#ifndef _INT_
#define _INT_

#define INT_LEVEL0_DDR  DDRG
#define INT_LEVEL0_PORT PORTG
#define INT_LEVEL0_PIN  PING
#define INT_LEVEL0_BIT  1

#define INT_LEVEL1_DDR  DDRD
#define INT_LEVEL1_PORT PORTD
#define INT_LEVEL1_PIN  PIND
#define INT_LEVEL1_BIT  7

#define INT_LEVEL2_DDR  DDRD
#define INT_LEVEL2_PORT PORTD
#define INT_LEVEL2_PIN  PIND
#define INT_LEVEL2_BIT  6

#define INT_LEVEL3_DDR  DDRG
#define INT_LEVEL3_PORT PORTG
#define INT_LEVEL3_PIN  PING
#define INT_LEVEL3_BIT  0

#define INT_LEVEL_SENSE_DDR  DDRD
#define INT_LEVEL_SENSE_PORT PORTD
#define INT_LEVEL_SENSE_PIN  PIND
#define INT_LEVEL_SENSE_BIT  1

#define INT_DOOR_SENSE_DDR  DDRE
#define INT_DOOR_SENSE_PORT PORTE
#define INT_DOOR_SENSE_PIN  PINE
#define INT_DOOR_SENSE_BIT  4

void int_init(void);
u08 int_value_init_get(void);
u08 int_value_isr_get(void);
void int_enable(void);
void int_disable(void);

#endif
