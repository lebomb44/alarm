#include "global.h"
#include "power.h"
#include "adc.h"
#include "timer.h"
#include "uart0.h"
#include "alarm.h"

/* SEM exchange between power timer and power cycle */
typedef enum {
  POWER_MONITORING_SEM_WAIT = 0,
  POWER_MONITORING_SEM_GET,
} POWER_MONITORING_SEM_T;

POWER_MONITORING_SEM_T power_monitoring_sem = 0;

void power_init(void)
{
  cbi(POWER_CAR_SENSE_POWER_PORT, POWER_CAR_SENSE_POWER_BIT);
  sbi(POWER_CAR_SENSE_POWER_DDR , POWER_CAR_SENSE_POWER_BIT);

  cbi(POWER_CAR_SENSE_DDR , POWER_CAR_SENSE_BIT);
  cbi(POWER_CAR_SENSE_PORT, POWER_CAR_SENSE_BIT);

  cbi(POWER_SELF_SENSE_POWER_PORT, POWER_SELF_SENSE_POWER_BIT);
  sbi(POWER_SELF_SENSE_POWER_DDR , POWER_SELF_SENSE_POWER_BIT);

  cbi(POWER_SELF_SENSE_DDR , POWER_SELF_SENSE_BIT);
  cbi(POWER_SELF_SENSE_PORT, POWER_SELF_SENSE_BIT);

  sbi(TEMP_SENSE_POWER_PORT, TEMP_SENSE_POWER_BIT);
  sbi(TEMP_SENSE_POWER_DDR , TEMP_SENSE_POWER_BIT);

  cbi(TEMP_SENSE_DDR , TEMP_SENSE_BIT);
  cbi(TEMP_SENSE_PORT, TEMP_SENSE_BIT);

  cbi(POWER_RESET_PORT, POWER_RESET_BIT);
  sbi(POWER_RESET_DDR , POWER_RESET_BIT);

  /* Power ON */
  sbi(POWER_PORT, POWER_BIT);
  sbi(POWER_DDR , POWER_BIT);

  /* Reset timer */
  TCCR1A=0x00;
  TCCR1B=0x00;  // No clock source (Timer/Counter stopped)
  TCNT1=0x0000;
  cbi(TIMSK , 5); // Timer/Counter1, Input Capture Interrupt Enable : Disable
  cbi(TIMSK , 4); // Timer/Counter1, Output Compare A Match Interrupt Enable : Disable
  cbi(TIMSK , 3); // Timer/Counter1, Output Compare B Match Interrupt Enable : Disable
  sbi(TIMSK , 2); // Timer/Counter1, Overflow Interrupt Enable : Enable
  cbi(ETIMSK, 0); // Timer/Counter1, Output Compare C Match Interrupt Enable : Disable

  power_monitoring_sem = POWER_MONITORING_SEM_WAIT;
}

void power_enable(void)
{
  sbi(POWER_PORT, POWER_BIT);
}

void power_disable(void)
{
  cbi(POWER_PORT, POWER_BIT);
}

void power_reset(void)
{
  sbi(POWER_RESET_PORT, POWER_RESET_BIT);
}

u16 power_car_voltage(void)
{
  u16 v=0;
  u08 i=0;
  /* Power ON acquisition */
  sbi(POWER_CAR_SENSE_POWER_PORT, POWER_CAR_SENSE_POWER_BIT);
  /* Wait for the capacitor load */
  delay_ms(1000);
  v=0;
  /* Get 64 times to increase precision */
  for(i=0; i<64; i++)
  { v+=adc_get(POWER_CAR_SENSE_BIT); }
  /* Power OFF acquisition */
  cbi(POWER_CAR_SENSE_POWER_PORT, POWER_CAR_SENSE_POWER_BIT);
  return v;
}

u16 power_self_voltage(void)
{
  u16 v=0;
  u08 i=0;
  /* Power ON acquisition */
  sbi(POWER_SELF_SENSE_POWER_PORT, POWER_SELF_SENSE_POWER_BIT);
  /* Wait for the capacitor load */
  delay_ms(1000);
  v=0;
  /* Get 64 times to increase precision */
  for(i=0; i<64; i++)
  { v+=adc_get(POWER_SELF_SENSE_BIT); }
  /* Power OFF acquisition */
  cbi(POWER_SELF_SENSE_POWER_PORT, POWER_SELF_SENSE_POWER_BIT);
  return v;
}

u16 power_temperature(void)
{
  u16 v=0;
  u08 i=0;
  /* Power ON acquisition */
  cbi(TEMP_SENSE_POWER_PORT, TEMP_SENSE_POWER_BIT);
  /* Wait for the capacitor load */
  delay_ms(1000);
  v=0;
  /* Get 64 times to increase precision */
  for(i=0; i<64; i++)
  { v+=adc_get(TEMP_SENSE_BIT); }
  /* Power OFF acquisition */
  sbi(TEMP_SENSE_POWER_PORT, TEMP_SENSE_POWER_BIT);
  return v;
}

void power_monitoring_enable(void)
{
  /* Reset timer */
  TCCR1B=0x00;  // No clock source (Timer/Counter stopped)
  TCNT1=0x0000;
  /* Launch timer */
  TCCR1B=0x05;  // clk/1024
}

void power_monitoring_disable(void)
{
  /* Reset timer */
  TCCR1B=0x00;  // No clock source (Timer/Counter stopped)
  TCNT1=0x0000;
}

void power_cycle(void)
{
  u16 v0=0;
  u16 v1=0;
  u16 v2=0;
  /* Compute only if IT TIMER1 gives us the SEM */
  if(power_monitoring_sem == POWER_MONITORING_SEM_GET)
  {
    v0=power_car_voltage(); /* 0.0V / 15.0V */
    v1=power_self_voltage(); /* 10.5V (47279) / 15.0V (62371) */ /* 11.0V (48956) / 15.0V (62371) */
    v2=power_temperature(); /* 60°C / -10°C */
    /* Check got values */
    if((v0<1) || (v0>62371) || (v1<47278) || (v1>62371) || (v2<26324) || (v2>44816))
    {
      DEBUG0_PUTS_P((const u08*)PSTR("POWER Sensor\n"));
      /* Disable power monitoring */
      power_monitoring_disable();
      /* Launch alarm */
      alarm_detection_power();
    }
    /* Reset SEM to prevent unautorized cycle */
    power_monitoring_sem = POWER_MONITORING_SEM_WAIT;  }
}

ISR(TIMER1_OVF_vect)
{
  /* Give SEM to cycle */
  power_monitoring_sem = POWER_MONITORING_SEM_GET;
}
