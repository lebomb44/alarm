#include "global.h"
#include "timer.h"
#include "buzzer.h"

u16 buzzer_delay=0;
u16 buzzer_delay_max=0;

void buzzer_init(void)
{
  cbi(BUZZER_POWER_PORT, BUZZER_POWER_BIT);
  sbi(BUZZER_POWER_DDR , BUZZER_POWER_BIT);
  TCCR2=0x00;  // No clock source (Timer/Counter stopped)
  TCNT2=0x00;
  sbi(TIMSK, 6); // Timer/Counter2 Overflow Interrupt Enable : Enable
  cbi(TIMSK, 7); // Timer/Counter2 Output Compare Match Interrupt Enable : Disable
}

void buzzer_enable(u16 delay)
{
  /* Stop timer */
  TCCR2=0x00;  // No clock source (Timer/Counter stopped)
  TCNT2=0x0000;
  buzzer_delay = 0;
  /* Check max delay in seconds */
  if(delay>2184) { delay = 2184; } /* 2184s MAX */
  buzzer_delay_max = delay*30; /* 1s = 1unit */
  sbi(BUZZER_POWER_PORT, BUZZER_POWER_BIT);
  /* Launch timer */
  TCCR2=0x05;  // clk/1024
}

void buzzer_disable(void)
{
  /* Stop timer */
  TCCR2=0x00;  // No clock source (Timer/Counter stopped)
  TCNT2=0x0000;
  /* Reset buzzer */
  buzzer_delay = 0;
  cbi(BUZZER_POWER_PORT, BUZZER_POWER_BIT);
}

ISR(TIMER2_OVF_vect) /* 1s = (1/8000000)*256*1024*30 */
{
  buzzer_delay++;
  /* Check if it is end of time */
  if(buzzer_delay >= buzzer_delay_max)
  {
    buzzer_disable();
  }
}

void bip_init(void)
{
  cbi(BIP_PORT, BIP_BIT);
  sbi(BIP_DDR , BIP_BIT);
}

void bip_enable(u16 delay)
{
  while(delay>0)
  {
    sbi(BIP_PORT, BIP_BIT);
    delay_ms(100);
    cbi(BIP_PORT, BIP_BIT);
    delay_ms(100);
    delay--;
  }
}
