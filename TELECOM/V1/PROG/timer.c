#include "global.h"
#include "timer.h"

volatile u16 index_buzzer=0;

void sleep_mode(void)
{
  MCUCR = 0x80; // Sleep mode
}

// Buzzer
SIGNAL(SIG_OVERFLOW0)
{
  if(index_buzzer<BUZZER_TPS_H)
    {
	  TCNT0=BUZZER_FREQ_H;
	  BUZZERTOGGLE();
	  index_buzzer++;
	  LED4ON();
	}
  else if(index_buzzer<BUZZER_TPS_L)
	{
	  TCNT0=BUZZER_FREQ_L;
	  BUZZERPORT=BUZZERPORT & 0xC3;
	  index_buzzer++;
	  LED4OFF();
	}
  else
    {
	  TCNT0=BUZZER_FREQ_L;
	  index_buzzer=0;
	}
}

// Led
//SIGNAL(SIG_OVERFLOW2)
//{
//}
