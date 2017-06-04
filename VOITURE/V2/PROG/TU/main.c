# include "global.h"
# include "delay.h"

void init(void)
{
  // Initialisation primaire des ports
  sbi(LEDDDR,LEDBIT);
}

int main(void)
{
u08* port=0;
u08 bit=0;
port=(u08*) &LEDPORT;
bit=LEDBIT;
  init();
  while(1)
  {
    //LEDON;
    //sbi((u08*) _SFR_MEM8(port),bit);
	sbi(*port,bit);
    delay_ms(100);
    //LEDOFF;
    //cbi((u08*) _SFR_MEM8(port),bit);
	cbi(*port,bit);
    delay_ms(100);
  }
  return (0);
}
