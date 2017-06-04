#include "global.h"
#include "utr2.h"
#include "uart0.h"
#include "alarm.h"

void utr2_init(void)
{
  cbi(UTR2_SENSE_DDR , UTR2_SENSE_BIT);
  sbi(UTR2_SENSE_PORT, UTR2_SENSE_BIT);

  cbi(UTR2_LED_CONTROL_DDR , UTR2_LED_CONTROL_BIT);
  cbi(UTR2_LED_CONTROL_PORT, UTR2_LED_CONTROL_BIT);

  cbi(UTR2_POWER_PORT, UTR2_POWER_BIT);
  sbi(UTR2_POWER_DDR , UTR2_POWER_BIT);

  cbi(EICRB, 4); // The low level of INTn generates an interrupt request
  cbi(EICRB, 5); // The low level of INTn generates an interrupt request
}

void utr2_enable(void)
{
  u32 i=0;
  /* Clear possible previous INT */
  cbi(EIMSK, 6); // External Interrupt Request 6 Disable
  sbi(EIFR , 6); // External Interrupt Flags 6 Cleared
  /* Power ON UTR2 module */
  sbi(UTR2_POWER_PORT, UTR2_POWER_BIT);
  i=800000;
  /* Check that module is power ON */
  while(i) { if(bit_is_set(UTR2_LED_CONTROL_PIN, UTR2_LED_CONTROL_BIT)) { break; } i--; }
  /* if not, these is a problem, so power off the module */
  if(i==0) { utr2_disable(); return; }
  sbi(EIMSK, 6); // External Interrupt Request 6 Enable
}

void utr2_disable(void)
{
  /* Power OFF UTR2 module */
  cbi(EIMSK, 6); // External Interrupt Request 6 Disable
  cbi(UTR2_POWER_PORT, UTR2_POWER_BIT);
}

ISR(INT6_vect)
{
  /* Alarm is detected */
  DEBUG0_PUTS_P((const u08*)PSTR("INT6 : UTR2 Sensor\n"));
  /* Power off the detection */
  utr2_disable();
  /* Activate alarm */
  alarm_detection_utr2();
}
