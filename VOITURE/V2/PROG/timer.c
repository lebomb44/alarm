#include "global.h"
#include "timer.h"

void sleep_mode(void)
{
  sbi(MCUCR,5);
}

void delay_1ms(void)
{
  u16 i;
  i = QUARTZ/10;
  while(i)
  {
    i--;
    nop();
  }
}

void delay_ms(u16 delay)
{
  while(delay)
  {
    delay_1ms();
    delay--;
  }
}
