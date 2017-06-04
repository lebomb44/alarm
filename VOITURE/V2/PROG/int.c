#include "global.h"
#include "int.h"
#include "uart0.h"
#include "alarm.h"

u08 int_value_init;
u08 int_value_isr;

void int_init(void)
{
  cbi(INT_LEVEL0_PORT, INT_LEVEL0_BIT);
  sbi(INT_LEVEL0_DDR , INT_LEVEL0_BIT);

  cbi(INT_LEVEL1_PORT, INT_LEVEL1_BIT);
  sbi(INT_LEVEL1_DDR , INT_LEVEL1_BIT);

  cbi(INT_LEVEL2_PORT, INT_LEVEL2_BIT);
  sbi(INT_LEVEL2_DDR , INT_LEVEL2_BIT);

  cbi(INT_LEVEL3_PORT, INT_LEVEL3_BIT);
  sbi(INT_LEVEL3_DDR , INT_LEVEL3_BIT);

  sbi(INT_LEVEL_SENSE_PORT, INT_LEVEL_SENSE_BIT);
  cbi(INT_LEVEL_SENSE_DDR , INT_LEVEL_SENSE_BIT);

  cbi(INT_DOOR_SENSE_PORT, INT_DOOR_SENSE_BIT);
  cbi(INT_DOOR_SENSE_DDR , INT_DOOR_SENSE_BIT);

  cbi(EICRA, 2); // The falling edge of INTn generates asynchronously an interrupt request
  sbi(EICRA, 3); // The falling edge of INTn generates asynchronously an interrupt request

  sbi(EICRB, 0); // The rising edge of INTn generates asynchronously an interrupt request
  sbi(EICRB, 1); // The rising edge of INTn generates asynchronously an interrupt request

  /* Initialize the INT detection values */
  int_value_init=0;
  int_value_isr=0;
}

u08 int_value_init_get(void)
{
  return int_value_init;
}

u08 int_value_isr_get(void)
{
  return int_value_isr;
}

void int_enable(void)
{
  u08 i=0;
  /* Clear possible previous INT */
  cbi(EIMSK, 1); // External Interrupt Request 1 Disable
  sbi(EIFR , 1); // External Interrupt Flags 1 Cleared
  /* Clear possible previous INT */
  cbi(EIMSK, 4); // External Interrupt Request 4 Disable
  sbi(EIFR , 4); // External Interrupt Flags 4 Cleared
  /* Search for the current configuration */
  for(i=0; i<16; i++)
  {
    if(i & 0x01) { sbi(INT_LEVEL0_PORT, INT_LEVEL0_BIT); } else { cbi(INT_LEVEL0_PORT, INT_LEVEL0_BIT); }
    if(i & 0x02) { sbi(INT_LEVEL1_PORT, INT_LEVEL1_BIT); } else { cbi(INT_LEVEL1_PORT, INT_LEVEL1_BIT); }
    if(i & 0x04) { sbi(INT_LEVEL2_PORT, INT_LEVEL2_BIT); } else { cbi(INT_LEVEL2_PORT, INT_LEVEL2_BIT); }
    if(i & 0x08) { sbi(INT_LEVEL3_PORT, INT_LEVEL3_BIT); } else { cbi(INT_LEVEL3_PORT, INT_LEVEL3_BIT); }
    if(bit_is_set(INT_LEVEL_SENSE_PIN, INT_LEVEL_SENSE_BIT)) { break; }
  }
  int_value_init = i;
  DEBUG0_PUTS_P((const u08*)PSTR("INT Enable config : ")); DEBUG0_PUTD('b', (u32)int_value_init); DEBUG0_PUTS_P((const u08*)PSTR("\n"));
  sbi(EIMSK, 1); // External Interrupt Request 1 Enable
  sbi(EIMSK, 4); // External Interrupt Request 4 Enable
}

void int_disable(void)
{
  cbi(EIMSK, 1); // External Interrupt Request 1 Disable
  cbi(EIMSK, 4); // External Interrupt Request 4 Disable
}

ISR(INT1_vect)
{
  u08 i=0;
  /* Power off the detection */
  int_disable();
  /* Search the detected position */
  for(i=0; i<16; i++)
  {
    if(i & 0x01) { sbi(INT_LEVEL0_PORT, INT_LEVEL0_BIT); } else { cbi(INT_LEVEL0_PORT, INT_LEVEL0_BIT); }
    if(i & 0x02) { sbi(INT_LEVEL1_PORT, INT_LEVEL1_BIT); } else { cbi(INT_LEVEL1_PORT, INT_LEVEL1_BIT); }
    if(i & 0x04) { sbi(INT_LEVEL2_PORT, INT_LEVEL2_BIT); } else { cbi(INT_LEVEL2_PORT, INT_LEVEL2_BIT); }
    if(i & 0x08) { sbi(INT_LEVEL3_PORT, INT_LEVEL3_BIT); } else { cbi(INT_LEVEL3_PORT, INT_LEVEL3_BIT); }
    if(bit_is_set(INT_LEVEL_SENSE_PIN, INT_LEVEL_SENSE_BIT)) { break; }
  }
  int_value_isr = i;
  /* Alarm is detected */
  DEBUG0_PUTS_P((const u08*)PSTR("INT1 : INT Switch Sensor : ISR=")); DEBUG0_PUTD('b', (u32)int_value_isr); DEBUG0_PUTS_P((const u08*)PSTR(" different from INIT=")); DEBUG0_PUTD('b', (u32)int_value_init); DEBUG0_PUTS_P((const u08*)PSTR("\n"));

  /* Activate alarm */
  alarm_detection_int();
}

ISR(INT4_vect)
{
  /* Alarm is detected */
  DEBUG0_PUTS_P((const u08*)PSTR("INT1 : INT Door Sensor\n"));
  /* Power off the detection */
  int_disable();
  /* Activate alarm */
  alarm_detection_int();
}
