#ifndef _GM862_
#define _GM862_

#include "global.h"

#define GM862_POWER_DDR  DDRC
#define GM862_POWER_PORT PORTC
#define GM862_POWER_PIN  PINC
#define GM862_POWER_BIT  5

#define GM862_ENABLE_DDR  DDRC
#define GM862_ENABLE_PORT PORTC
#define GM862_ENABLE_PIN  PINC
#define GM862_ENABLE_BIT  6

#define GM862_PWRMON_DDR  DDRC
#define GM862_PWRMON_PORT PORTC
#define GM862_PWRMON_PIN  PINC
#define GM862_PWRMON_BIT  7

#define GM862_RESET_DDR  DDRG
#define GM862_RESET_PORT PORTG
#define GM862_RESET_PIN  PING
#define GM862_RESET_BIT  2

#define GM862_LED_STATUS_DDR  DDRA
#define GM862_LED_STATUS_PORT PORTA
#define GM862_LED_STATUS_PIN  PINA
#define GM862_LED_STATUS_BIT  7

#define GM862_SENSE_DDR  DDRD
#define GM862_SENSE_PORT PORTD
#define GM862_SENSE_PIN  PIND
#define GM862_SENSE_BIT  0

#define GM862_GSM_MSG_SIZE 159
extern u08 gm862_gsm_msg[GM862_GSM_MSG_SIZE];

typedef enum {
  GM862_RECEIVE_SEM_WAIT = 0,
  GM862_RECEIVE_SEM_GET,
} GM862_RECEIVE_SEM_T;

extern GM862_RECEIVE_SEM_T gm862_receive_sem;

typedef enum {
  GM862_RECEIVE_STATE_OFF = 0,
  GM862_RECEIVE_STATE_ON,
} GM862_RECEIVE_STATE_T;

extern GM862_RECEIVE_STATE_T gm862_receive_state;

typedef struct _GM862_GPS_POSITION_T {
  u08 hour;
  u08 minutes;
  u08 secondes;
  u08 lat_sign;
  u08 lat_degrees;
  u32 lat_minutes;
  u08 long_sign;
  u08 long_degrees;
  u32 long_minutes;
  u08 nb_sat;
} GM862_GPS_POSITION_T;

extern GM862_GPS_POSITION_T gm862_gps_position;

u08 gm862_cmd(const u08* cmd);
u08 gm862_ack(const u08* acq);
u08 gm862_ack_OK(void);
u08 gm862_cmd_ack_OK(const u08* cmd);
void gm862_init(void);
u08 gm862_power_on(void);
void gm862_power_off(void);
u08 gm862_enable(void);
u08 gm862_disable(void);
void gm862_reset(void);
u08 gm862_led_status_state(void);
u08 gm862_pwrmon_state(void);
u08 gm862_alarm_state(void);
u08 gm862_gsm_enable(void);
u08 gm862_gsm_disable(void);
u08 gm862_gsm_tel_valid(u08* tel);
u08 gm862_gsm_send(u08* s, u08* tel);
void gm862_gsm_cat_alarm_msg(void);
u08 gm862_gsm_receive_on(void);
u08 gm862_gsm_receive_enable(void);
u08 gm862_gsm_receive_disable(void);
u08 gm862_gsm_receive(void);
u08 gm862_gps_enable(void);
u08 gm862_gps_disable(void);
u08 gm862_gps_position_get(void);
void gm862_cycle(void);

#endif
