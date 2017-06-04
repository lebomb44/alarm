#ifndef _POWER_
#define _POWER_

#include "global.h"

#define POWER_CAR_SENSE_POWER_DDR  DDRE
#define POWER_CAR_SENSE_POWER_PORT PORTE
#define POWER_CAR_SENSE_POWER_PIN  PINE
#define POWER_CAR_SENSE_POWER_BIT  3

#define POWER_CAR_SENSE_DDR  DDRF
#define POWER_CAR_SENSE_PORT PORTF
#define POWER_CAR_SENSE_PIN  PINF
#define POWER_CAR_SENSE_BIT  2

#define POWER_SELF_SENSE_POWER_DDR  DDRE
#define POWER_SELF_SENSE_POWER_PORT PORTE
#define POWER_SELF_SENSE_POWER_PIN  PINE
#define POWER_SELF_SENSE_POWER_BIT  2

#define POWER_SELF_SENSE_DDR  DDRF
#define POWER_SELF_SENSE_PORT PORTF
#define POWER_SELF_SENSE_PIN  PINF
#define POWER_SELF_SENSE_BIT  1

#define TEMP_SENSE_POWER_DDR  DDRB
#define TEMP_SENSE_POWER_PORT PORTB
#define TEMP_SENSE_POWER_PIN  PINB
#define TEMP_SENSE_POWER_BIT  5

#define TEMP_SENSE_DDR  DDRF
#define TEMP_SENSE_PORT PORTF
#define TEMP_SENSE_PIN  PINF
#define TEMP_SENSE_BIT  0

#define POWER_RESET_DDR  DDRA
#define POWER_RESET_PORT PORTA
#define POWER_RESET_PIN  PINA
#define POWER_RESET_BIT  0

#define POWER_DDR  DDRA
#define POWER_PORT PORTA
#define POWER_PIN  PINA
#define POWER_BIT  1

void power_init(void);
void power_enable(void);
void power_disable(void);
void power_reset(void);
u16 power_car_voltage(void);
u16 power_self_voltage(void);
u16 power_temperature(void);
void power_monitoring_enable(void);
void power_monitoring_disable(void);
void power_cycle(void);

#endif
