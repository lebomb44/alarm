#include "global.h"
#include "alarm.h"
#include "timer.h"
#include "uart0.h"
#include "config.h"
#include "power.h"
#include "buzzer.h"
#include "utr2.h"
#include "int.h"
#include "gm862.h"

typedef enum {
  ALARM_MONITORING_OFF = 0,
  ALARM_MONITORING_ON,
} ALARM_MONITORING_T;

ALARM_MONITORING_T alarm_monitoring = ALARM_MONITORING_OFF;

u08 alarm_detection_state = ALARM_DETECTION_STATE_NONE;

typedef enum {
  ALARM_DETECTION_SEM_WAIT = 0,
  ALARM_DETECTION_SEM_GET,
} ALARM_DETECTION_SEM_T;

ALARM_DETECTION_SEM_T alarm_detection_sem = ALARM_DETECTION_SEM_WAIT;

typedef enum {
  ALARM_SCHED_STATE_MONITORING_ON = 0,
  ALARM_SCHED_STATE_WAIT,
  ALARM_SCHED_STATE_POWER_ON,
  ALARM_SCHED_STATE_ENABLE,
  ALARM_SCHED_STATE_GSM_ENABLE,
  ALARM_SCHED_STATE_GPS_ENABLE,
  ALARM_SCHED_STATE_GSM_SEND,
  ALARM_SCHED_STATE_GSM_SEND_WITH_GPS,
  ALARM_SCHED_STATE_END,
} ALARM_SCHED_STATE_T;

ALARM_SCHED_STATE_T alarm_sched_state = ALARM_SCHED_STATE_MONITORING_ON;

u32 alarm_sched_time = 0;

void alarm_init(void)
{
  TCCR3A=0x00;
  TCCR3B=0x00;  // No clock source (Timer/Counter stopped)
  TCNT3=0x0000;
  cbi(ETIMSK, 5); // Timer/Counter3, Input Capture Interrupt Enable : Disable
  cbi(ETIMSK, 4); // Timer/Counter3, Output Compare A Match Interrupt Enable : Disable
  cbi(ETIMSK, 3); // Timer/Counter3, Output Compare B Match Interrupt Enable : Disable
  sbi(ETIMSK, 2); // Timer/Counter3, Overflow Interrupt Enable : Enable
  cbi(ETIMSK, 1); // Timer/Counter3, Output Compare C Match Interrupt Enable : Disable

  alarm_monitoring      = ALARM_MONITORING_OFF;
  alarm_sched_state     = ALARM_SCHED_STATE_MONITORING_ON;
  alarm_sched_time      = 0;
  alarm_detection_state = ALARM_DETECTION_STATE_NONE;
  alarm_detection_sem   = ALARM_DETECTION_SEM_WAIT;
}

void alarm_monitoring_on(void)
{
  alarm_monitoring_off();
  config_load();
  bip_enable(1);
  alarm_monitoring      = ALARM_MONITORING_ON;
  alarm_sched_state     = ALARM_SCHED_STATE_MONITORING_ON;
  alarm_sched_time      = config_alarm.mon_delay;
  alarm_detection_state = ALARM_DETECTION_STATE_NONE;
  alarm_detection_sem   = ALARM_DETECTION_SEM_GET;
  TCCR3B=0x05;  // clk/1024
}

void alarm_monitoring_off(void)
{
  TCCR3B=0x00;  // No clock source (Timer/Counter stopped)
  utr2_disable();
  int_disable();
  power_monitoring_disable();
  bip_enable(2);
  alarm_monitoring      = ALARM_MONITORING_OFF;
  alarm_sched_state     = ALARM_SCHED_STATE_MONITORING_ON;
  alarm_sched_time      = 0;
  alarm_detection_state = ALARM_DETECTION_STATE_NONE;
  alarm_detection_sem   = ALARM_DETECTION_SEM_WAIT;
}

u08 alarm_monitoring_is_on(void)
{
  if(alarm_monitoring == ALARM_MONITORING_OFF)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

u08 alarm_monitoring_is_off(void)
{
  if(alarm_monitoring == ALARM_MONITORING_OFF)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void alarm_detection_utr2(void)
{
  if(!(alarm_detection_state & ALARM_DETECTION_STATE_UTR2))
  {
    DEBUG0_PUTS_P((const u08*)PSTR("ALARM_DETECTION_STATE_UTR2\n"));
    alarm_sched_state     = ALARM_SCHED_STATE_WAIT;
    alarm_sched_time      = config_alarm.wait_delay;
    alarm_detection_state = alarm_detection_state | ALARM_DETECTION_STATE_UTR2;
    alarm_detection_sem   = ALARM_DETECTION_SEM_GET;
    TCCR3B=0x05;  // clk/1024
  }
}

void alarm_detection_int(void)
{
  if(!(alarm_detection_state & ALARM_DETECTION_STATE_INT))
  {
    DEBUG0_PUTS_P((const u08*)PSTR("ALARM_DETECTION_STATE_INT\n"));
    alarm_sched_state     = ALARM_SCHED_STATE_WAIT;
    alarm_sched_time      = config_alarm.wait_delay;
    alarm_detection_state = alarm_detection_state | ALARM_DETECTION_STATE_INT;
    alarm_detection_sem   = ALARM_DETECTION_SEM_GET;
    TCCR3B=0x05;  // clk/1024
  }
}

void alarm_detection_power(void)
{
  if(!(alarm_detection_state & ALARM_DETECTION_STATE_POWER))
  {
    DEBUG0_PUTS_P((const u08*)PSTR("ALARM_DETECTION_STATE_POWER\n"));
    alarm_sched_state     = ALARM_SCHED_STATE_WAIT;
    alarm_sched_time      = config_alarm.wait_delay;
    alarm_detection_state = alarm_detection_state | ALARM_DETECTION_STATE_POWER;
    alarm_detection_sem   = ALARM_DETECTION_SEM_GET;
    TCCR3B=0x05;  // clk/1024
  }
}

void alarm_cycle(void)
{
  if(alarm_detection_sem == ALARM_DETECTION_SEM_GET)
  {
    TCCR3B=0x00;  // No clock source (Timer/Counter stopped)
    TCNT3=0x0000;
    gm862_gsm_receive_disable();
    if(alarm_sched_state == ALARM_SCHED_STATE_MONITORING_ON)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_MON_ON\n"));
      if(alarm_sched_time == 0)
      {
        if(config_alarm.monitoring_use & ALARM_DETECTION_STATE_UTR2)  { utr2_enable(); }
        if(config_alarm.monitoring_use & ALARM_DETECTION_STATE_INT)   { int_enable(); }
        if(config_alarm.monitoring_use & ALARM_DETECTION_STATE_POWER) { power_monitoring_enable(); }
        alarm_detection_sem = ALARM_DETECTION_SEM_WAIT;
        return;
      }
      else { alarm_sched_time--; }
    }
    if(alarm_sched_state == ALARM_SCHED_STATE_WAIT)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_WAIT\n"));
      if(alarm_sched_time == 0)
      {
        alarm_sched_state = ALARM_SCHED_STATE_POWER_ON;
        utr2_disable();
        int_disable();
        power_monitoring_disable();
        buzzer_enable(config_alarm.buzzer_delay);
      }
      else { alarm_sched_time--; }
    }
    if(alarm_sched_state == ALARM_SCHED_STATE_POWER_ON)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_POWER_ON\n"));
      if(gm862_power_on()) { gm862_power_off(); } else { alarm_sched_state = ALARM_SCHED_STATE_ENABLE; }
    }
    if(alarm_sched_state == ALARM_SCHED_STATE_ENABLE)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_ENABLE\n"));
      if(gm862_enable()) { gm862_power_off(); alarm_sched_state = ALARM_SCHED_STATE_POWER_ON; } else { alarm_sched_state = ALARM_SCHED_STATE_GSM_ENABLE; }
    }
    if(alarm_sched_state == ALARM_SCHED_STATE_GSM_ENABLE)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_GSM_ENABLE\n"));
      if(gm862_gsm_enable()) { gm862_disable(); alarm_sched_state = ALARM_SCHED_STATE_ENABLE; } else { alarm_sched_state = ALARM_SCHED_STATE_GPS_ENABLE; }
    }
    if(alarm_sched_state == ALARM_SCHED_STATE_GPS_ENABLE)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_GPS_ENABLE\n"));
      if(gm862_gps_enable()) { gm862_disable(); alarm_sched_state = ALARM_SCHED_STATE_ENABLE; } else { alarm_sched_state = ALARM_SCHED_STATE_GSM_SEND; }
    }
    if(alarm_sched_state == ALARM_SCHED_STATE_GSM_SEND)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_GSM_SEND\n"));
      delay_ms(10000);
      gm862_gsm_msg[0]='\0'; gm862_gsm_cat_alarm_msg();
      if(gm862_gsm_send(&gm862_gsm_msg[0], &config_alarm.tel0[0]) || gm862_gsm_send(&gm862_gsm_msg[0], &config_alarm.tel1[0]))
      { gm862_reset(); alarm_sched_state = ALARM_SCHED_STATE_ENABLE; }
      else
      { gm862_gsm_receive_on(); alarm_sched_state = ALARM_SCHED_STATE_GSM_SEND_WITH_GPS; }
    }
    if(alarm_sched_state == ALARM_SCHED_STATE_GSM_SEND_WITH_GPS)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_GSM_SEND_WITH_GPS\n"));
      gm862_gsm_receive_disable();
      gm862_gps_position_get();
      if((gm862_gps_position.nb_sat>=3) && ((gm862_gps_position.lat_degrees>0) || (gm862_gps_position.lat_minutes>0) || (gm862_gps_position.long_degrees>0) || (gm862_gps_position.long_minutes>0)))
      {
        gm862_gsm_msg[0]='\0'; gm862_gsm_cat_alarm_msg();
        if(gm862_gsm_send(&gm862_gsm_msg[0], &config_alarm.tel0[0]) || gm862_gsm_send(&gm862_gsm_msg[0], &config_alarm.tel1[0]))
        { gm862_reset(); alarm_sched_state = ALARM_SCHED_STATE_ENABLE; }
        else
        { alarm_sched_state = ALARM_SCHED_STATE_END; }
      }
    }
    if(alarm_sched_state == ALARM_SCHED_STATE_END)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ALARM_SCHED_STATE_END\n"));
    }
    alarm_detection_sem = ALARM_DETECTION_SEM_WAIT;
    gm862_gsm_receive_enable();
    TCCR3B=0x05;  // clk/1024
  }
}

ISR(TIMER3_OVF_vect) /* 8.4s = (1/8000000)*65536*1024 */
{
  alarm_detection_sem = ALARM_DETECTION_SEM_GET;
}
