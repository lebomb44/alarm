#ifndef _CONFIG_
#define _CONFIG_

#include "global.h"

typedef struct _CONFIG_ALARM_T {
  u08 monitoring_use;
  u16 mon_delay;
  u16 wait_delay;
  u16 buzzer_delay;
  u08 tel0[13];
  u08 tel1[13];
} CONFIG_ALARM_T;

extern CONFIG_ALARM_T config_alarm;

#define CONFIG_CODE_NB 4
typedef struct _CONFIG_KEYBOARD_T {
  u08 code[CONFIG_CODE_NB];
  u16 delay;
  u08 debug0;
} CONFIG_KEYBOARD_T;

extern CONFIG_KEYBOARD_T config_keyboard;

/* SEM Exchange between UART0 and config cycle */
typedef enum {
  CONFIG_SEM_WAIT = 0,
  CONFIG_SEM_GET,
} CONFIG_SEM_T;

extern CONFIG_SEM_T config_sem;

void config_init(void);
void config_load(void);
void config_save(void);
void config_keyboard_load(void);
void config_keyboard_save(void);
void config_show(void);
void config_cycle(void);

#endif
