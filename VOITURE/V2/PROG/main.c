#include "global.h"
#include "fifo.h"
#include "string.h"
#include "eeprom.h"
#include "adc.h"
#include "timer.h"
#include "uart0.h"
#include "uart1.h"
#include "config.h"
#include "spares.h"
#include "power.h"
#include "keyboard.h"
#include "buzzer.h"
#include "utr2.h"
#include "int.h"
#include "gm862.h"
#include "alarm.h"

int main(void)
{
  adc_init();
  alarm_init();
  uart0_init();
  uart1_init();
  config_init();
  spares_init();
  power_init();
  keyboard_init();
  buzzer_init();
  bip_init();
  utr2_init();
  int_init();
  gm862_init();

  keyboard_cycle();

  sbi(LEDDDR, LEDBIT);
  sbi(SREG,7);
  sleep_mode();

  while(1)
  {
    config_cycle();
    //keyboard_cycle();
    power_cycle();
    alarm_cycle();
    gm862_cycle();
    LEDTOGGLE;
    sleep();
  }
  return 0;
}
