#include "global.h"
#include "keyboard.h"
#include "adc.h"
#include "timer.h"
#include "uart0.h"
#include "config.h"
#include "spares.h"
#include "power.h"
#include "buzzer.h"
#include "alarm.h"
#include "gm862.h"

/* SEM exchange between INT and cycle */
typedef enum {
  KEYBOARD_SEM_WAIT = 0,
  KEYBOARD_SEM_GET,
} KEYBOARD_SEM_T;

KEYBOARD_SEM_T keyboard_sem=0;

/* STATE for evaluation CODE */
typedef enum {
  KEYBOARD_STATE0 = 0,
  KEYBOARD_STATE_END = KEYBOARD_STATE0+CONFIG_CODE_NB,
  KEYBOARD_STATE_OK,
  KEYBOARD_STATE_KO,
} KEYBOARD_STATE_T;

KEYBOARD_STATE_T keyboard_state=0;

#define KEYBOARD_TOUCH_SAMPLE_NB 64 /* Max 64 */

u16 keyboard_delay = 0;
u08 keyboard_touch[CONFIG_CODE_NB] = {0};

void keyboard_init(void)
{
  config_keyboard_load();

  cbi(KEYBOARD_INT_PORT, KEYBOARD_INT_BIT);
  cbi(KEYBOARD_INT_DDR , KEYBOARD_INT_BIT);
  cbi(KEYBOARD_ADC_PORT, KEYBOARD_ADC_BIT);
  cbi(KEYBOARD_ADC_DDR , KEYBOARD_ADC_BIT);
  /* Init the keuboard state on touch 0 */
  keyboard_state = KEYBOARD_STATE0;
  /* Get Touch value */
  keyboard_touch[keyboard_state-KEYBOARD_STATE0] = keyboard_touch_get();

  cbi(EICRB, 2); // The falling edge of INTn generates asynchronously an interrupt request
  sbi(EICRB, 3); // The falling edge of INTn generates asynchronously an interrupt request

  /* Reset HW timer */
  TCCR0=0x00;  // No clock source (Timer/Counter stopped)
  TCNT0=0x00;
  sbi(TIMSK, 0); // Timer/Counter0 Overflow Interrupt Enable : Enable
  cbi(TIMSK, 1); // Timer/Counter0 Output Compare Match Interrupt Enable : Disable

  /* Reset delay timer */
  keyboard_delay = 0;
  /* Give hand to the task */
  keyboard_sem = KEYBOARD_SEM_GET;

  /* Init INT */
//sbi(EIFR , KEYBOARD_INT_BIT); // External Interrupt Flags 5 Cleared
//sbi(EIMSK, KEYBOARD_INT_BIT); // External Interrupt Request 5 Enable
}

ISR(INT5_vect)
{
  /* Get Touch value */
  keyboard_touch[keyboard_state-KEYBOARD_STATE0] = keyboard_touch_get();
  /* Give SEM to keyboard cycle */
  keyboard_sem = KEYBOARD_SEM_GET;
  keyboard_cycle();
}

u08 keyboard_touch_get(void)
{
  u08 touch=0;
  if(bit_is_clear(SPARE1_PIN, SPARE1_BIT)) { touch = touch + 0x01; }
  if(bit_is_clear(SPARE2_PIN, SPARE2_BIT)) { touch = touch + 0x02; }
  if(bit_is_clear(SPARE3_PIN, SPARE3_BIT)) { touch = touch + 0x04; }
  if(bit_is_clear(SPARE4_PIN, SPARE4_BIT)) { touch = touch + 0x08; }
  if(touch==0) { return 10; }
  if(touch==10) { touch=0; }
  return touch;
}

void keyboard_cycle(void)
{
  u08 i=0;
  u16 timeout=0;

  if(keyboard_sem == KEYBOARD_SEM_GET)
  {
    /* Disable and clear External INT not to be corrupted */
    cbi(EIMSK, KEYBOARD_INT_BIT); // External Interrupt Request 5 Disable
    sbi(EIFR , KEYBOARD_INT_BIT); // External Interrupt Flags 5 Cleared
    /* If it is the first time, first touch */
    if(keyboard_state == KEYBOARD_STATE0)
    {
      /* Reset Timer */
      TCCR0=0x00;  // No clock source (Timer/Counter stopped)
      TCNT0=0x00;
      keyboard_delay = 0;
      /* Launch Timer */
      TCCR0=0x07;  // clk/1024
    }
    /* Get the CONFIG_CODE_NB touchs */
    if((KEYBOARD_STATE0 <= keyboard_state) && (keyboard_state < (KEYBOARD_STATE0+CONFIG_CODE_NB)))
    {
      DEBUG0_PUTS_P((const u08*)PSTR("Keyboard Touch : ")); DEBUG0_PUTD('d', (u32)keyboard_touch[keyboard_state-KEYBOARD_STATE0]); DEBUG0_PUTS_P((const u08*)PSTR("\n"));
      keyboard_state++;
    }
    if(keyboard_state == KEYBOARD_STATE_END)
    {
      /* Check the code */
      for(i=0; i<CONFIG_CODE_NB; i++)
      {
        if(keyboard_touch[i] != config_keyboard.code[i]) { break; }
      }
      /* Code ic correct */
      if(i==CONFIG_CODE_NB)
      {
        keyboard_state = KEYBOARD_STATE_OK;
        DEBUG0_PUTS_P((const u08*)PSTR("Keyboard TOUCHS are valid\n"));
      }
      /* Code is incorrect */
      else
      {
        /* Go to invalid wait state */
        keyboard_state = KEYBOARD_STATE_KO;
        DEBUG0_PUTS_P((const u08*)PSTR("Keyboard TOUCH are NOT valid\n"));
      }
    }
    if(keyboard_state == KEYBOARD_STATE_OK)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("Keyboard CODE OK\n"));
      TCCR0=0x00;  // No clock source (Timer/Counter stopped)
      TCNT0=0x00;
      keyboard_delay = 0;
      if(alarm_monitoring_is_on())
      {
        /* Stop alarm monitoring */
        alarm_monitoring_off();
        /* Try to disconnect from GSM network properly */
        if(gm862_disable()) { delay_ms(1000); gm862_disable(); } else { delay_ms(3000); }
        DEBUG0_PUTS_P((const u08*)PSTR("Alarm monitoring STOPPED\n"));
        /* Power OFF the system */
        power_disable();
      }
      else
      {
        /* Start alarm monitoring */
        alarm_monitoring_on();
        DEBUG0_PUTS_P((const u08*)PSTR("Alarm monitoring STARTED\n"));
      }
      /* Go back to wait state to stop monitoring */
      keyboard_state = KEYBOARD_STATE0;
    }
    else
    {
      /* BIP for all other states */
      bip_enable(1);
    }
    /* Reset SEM to prevent unautorized cycle */
    keyboard_sem = KEYBOARD_SEM_WAIT;
    /* If touch is valid but CODE not fully completed */
    if((KEYBOARD_STATE0 <= keyboard_state) && (keyboard_state < (KEYBOARD_STATE0+CONFIG_CODE_NB)))
    {
      /* Wait for the touch to be relaxed BUT protect with timeout against long push */
      timeout=3000; while((keyboard_touch_get()<10) && (timeout>0)) { delay_ms(1); timeout--; }
      /* delay because of bounce */
      delay_ms(10);
      /* Enable interrupt for next touch */
      sbi(EIFR , KEYBOARD_INT_BIT); // External Interrupt Flags 5 Cleared
      sbi(EIMSK, KEYBOARD_INT_BIT); // External Interrupt Request 5 Enable
      DEBUG0_PUTS_P((const u08*)PSTR("Keyboard TOUCH relaxed\n"));
    }
  }
}

ISR(TIMER0_OVF_vect)
{
  keyboard_delay++;
  /* Check if it is end of time. 1s = (1/8000000)*256*1024*30 so 2184s MAX due to 16bits data */
  if(keyboard_delay >= (config_keyboard.delay)*30)
  {
    /* Stop and reset timer */
    TCCR0=0x00;  // No clock source (Timer/Counter stopped)
    TCNT0=0x00;
    keyboard_delay = 0;
    keyboard_state = KEYBOARD_STATE0;
    bip_enable(4);
    /* If alarm monitoring is NOT in progress */
    if(alarm_monitoring_is_off())
    {
      /* Power OFF the system */
      power_disable();
    }
    /* Else cleard touch interrupt for next touch */
    sbi(EIFR , KEYBOARD_INT_BIT); // External Interrupt Flags 5 Cleared
    sbi(EIMSK, KEYBOARD_INT_BIT); // External Interrupt Request 5 Enable
  }
}
