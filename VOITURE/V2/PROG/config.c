#include "global.h"
#include "config.h"
#include "string.h"
#include "eeprom.h"
#include "uart0.h"
#include "uart1.h"
#include "power.h"
#include "keyboard.h"
#include "gm862.h"
#include "alarm.h"
# include <avr/eeprom.h>

CONFIG_ALARM_T config_alarm;
CONFIG_ALARM_T EEMEM config_alarm_EE = { /*FIXME OCM : 7*/ 2, 3, 3, 300, "+33689350159", "++3689350159" };

CONFIG_KEYBOARD_T config_keyboard;
CONFIG_KEYBOARD_T EEMEM config_keyboard_EE={{8,8,8,8}, 30, 1};

CONFIG_SEM_T config_sem = 0;

void config_init(void)
{
  config_sem = CONFIG_SEM_WAIT;
}

void config_load(void)
{
  u08 i=0;
  /* Copy from EEPROM adress 0 to size */
  for(i=0; i<sizeof(CONFIG_ALARM_T); i++)
  {
    *(((u08*)&config_alarm)+i) = eeprom_read(i);
  }
}

void config_save(void)
{
  u08 i=0;
  /* Copy to EEPROM adress 0 to size */
  for(i=0; i<sizeof(CONFIG_ALARM_T); i++)
  {
    eeprom_write(i, *(((u08*)&config_alarm)+i));
  }
}

void config_keyboard_load(void)
{
  u08 i=0;
  /* Copy from end of prevois struct to size */
  for(i=0; i<sizeof(CONFIG_KEYBOARD_T); i++)
  {
    *(((u08*)&config_keyboard)+i) = eeprom_read(i+sizeof(CONFIG_ALARM_T));
  }
}

void config_keyboard_save(void)
{
  u08 i=0;
  /* Copy to end of previous struct to size */
  for(i=0; i<sizeof(CONFIG_KEYBOARD_T); i++)
  {
    eeprom_write(i+sizeof(CONFIG_ALARM_T), *(((u08*)&config_keyboard)+i));
  }
}

void config_show(void)
{
  u08 i=0;
  uart0_puts_P((const u08*)PSTR("\n  Alarm Monitoring Use : ")); uart0_putd('d',(u32)(config_alarm.monitoring_use));
  uart0_puts_P((const u08*)PSTR("\n  Alarm Monitoring Delay x8 (max 450=1h) : ")); uart0_putd('d',(u32)(config_alarm.mon_delay));
  uart0_puts_P((const u08*)PSTR("\n  Alarm Wait Delay x8 (max 450=1h) : ")); uart0_putd('d',(u32)(config_alarm.wait_delay));
  uart0_puts_P((const u08*)PSTR("\n  Alarm Buzzer Delay (max 2184s) : ")); uart0_putd('d',(u32)(config_alarm.buzzer_delay));
  uart0_puts_P((const u08*)PSTR("\n  Alarm Tel0 (12 car) : ")); uart0_puts(&(config_alarm.tel0[0]));
  uart0_puts_P((const u08*)PSTR("\n  Alarm Tel1 (12 car) : ")); uart0_puts(&(config_alarm.tel1[0]));
  uart0_puts_P((const u08*)PSTR("\n  Keyboard Code (")); uart0_putd('d',(u32)CONFIG_CODE_NB); uart0_puts_P((const u08*)PSTR(" num) : ")); for(i=0; i<CONFIG_CODE_NB; i++) { uart0_putc(config_keyboard.code[i]+'0'); }
  uart0_puts_P((const u08*)PSTR("\n  Keyboard Delay (max 2184s) : ")); uart0_putd('d',(u32)(config_keyboard.delay));
  uart0_puts_P((const u08*)PSTR("\n  Keyboard Debug0 : ")); uart0_putd('d',(u32)(config_keyboard.debug0));
  uart0_puts_P((const u08*)PSTR("\n"));
}

void config_cycle(void)
{
  u08 i=0;
  u16 data0=0;
  u16 data1=0;
  u08 char_received=0;
  /* Compute only if SEM is given by UART0 RX IT */
  if(config_sem == CONFIG_SEM_GET)
  {
    /* Disable receive mechanism on UART1 */
    gm862_gsm_receive_disable();
    /* Compute all command in buffer */
    while(data0 != 0xFFFF)
    {
      data0 = fifo_get(&uart0_rx_fifo);
      if(data0 == 'h')
      {
        uart0_puts_P((const u08*)PSTR("Help\n"));
        uart0_puts_P((const u08*)PSTR("  'h' : Help\n"));
        uart0_puts_P((const u08*)PSTR("  'o' : Power ON\n"));
        uart0_puts_P((const u08*)PSTR("  'f' : Power OFF\n"));
        uart0_puts_P((const u08*)PSTR("  'z' : GM862 ON\n"));
        uart0_puts_P((const u08*)PSTR("  's' : GM862 OFF\n"));
        uart0_puts_P((const u08*)PSTR("  'e' : GM862 Enable\n"));
        uart0_puts_P((const u08*)PSTR("  'r' : GM862 GSM Enable\n"));
        uart0_puts_P((const u08*)PSTR("  't' : GM862 GSM Receive Enable\n"));
        uart0_puts_P((const u08*)PSTR("  'x' : GM862 GSM Send\n"));
        uart0_puts_P((const u08*)PSTR("  'c' : GM862 GSM Receive\n"));
        uart0_puts_P((const u08*)PSTR("  'y' : GM862 GPS Enable\n"));
        uart0_puts_P((const u08*)PSTR("  'v' : GM862 GPS Get\n"));
        uart0_puts_P((const u08*)PSTR("  'd' : GM862 Disable\n"));
        uart0_puts_P((const u08*)PSTR("  'é' : Direct routing to GM862\n"));
        uart0_puts_P((const u08*)PSTR("  'l' : Config Load\n"));
        uart0_puts_P((const u08*)PSTR("  'm' : Config Save\n"));
        uart0_puts_P((const u08*)PSTR("  'j' : Config Show\n"));
        uart0_puts_P((const u08*)PSTR("  'k' : Config Get\n"));
        uart0_puts_P((const u08*)PSTR("  'a' : Alarm MON ON\n"));
        uart0_puts_P((const u08*)PSTR("  'q' : Alarm MON OFF\n"));
        uart0_puts_P((const u08*)PSTR("  '1' : Show Temperature\n"));
        uart0_puts_P((const u08*)PSTR("  '2' : Show Self Voltage\n"));
        uart0_puts_P((const u08*)PSTR("  '3' : Show Car Voltage\n"));
        uart0_puts_P((const u08*)PSTR("  'b' : Keyboard Touch get\n"));
      }
      else if(data0 == 'o')
      {
        uart0_puts_P((const u08*)PSTR("Power ON\n"));
        /* Stop keyboard timeout timer. If not it will power off */
        TCCR0=0x00;  // No clock source (Timer/Counter stopped)
        /* Disable and clear External INT for keyboard */
        cbi(EIMSK, KEYBOARD_INT_BIT); // External Interrupt Request 5 Disable
        sbi(EIFR , KEYBOARD_INT_BIT); // External Interrupt Flags 5 Cleared
        power_enable();
      }
      else if(data0 == 'f')
      {
        uart0_puts_P((const u08*)PSTR("Power OFF\n"));
        power_disable();
      }
      else if(data0 == 'z')
      {
        uart0_puts_P((const u08*)PSTR("GM862 ON\n"));
        gm862_power_on();
      }
      else if(data0 == 's')
      {
        uart0_puts_P((const u08*)PSTR("GM862 OFF\n"));
        gm862_power_off();
      }
      else if(data0 == 'e')
      {
        uart0_puts_P((const u08*)PSTR("GM862 Enable\n"));
        gm862_enable();
      }
      else if(data0 == 'r')
      {
        uart0_puts_P((const u08*)PSTR("GM862 GSM Enable\n"));
        gm862_gsm_enable();
      }
      else if(data0 == 't')
      {
        uart0_puts_P((const u08*)PSTR("GM862 GSM Receive ON\n"));
        gm862_gsm_receive_on();
      }
      else if(data0 == 'x')
      {
        uart0_puts_P((const u08*)PSTR("GM862 GSM Send\n"));
        gm862_gsm_send((u08*)"Test\n", &(config_alarm.tel0[0]));
      }
      else if(data0 == 'c')
      {
        uart0_puts_P((const u08*)PSTR("GM862 GSM Receive\n"));
        gm862_gsm_receive();
      }
      else if(data0 == 'y')
      {
        uart0_puts_P((const u08*)PSTR("GM862 GPS Enable\n"));
        gm862_gps_enable();
      }
      else if(data0 == 'v')
      {
        uart0_puts_P((const u08*)PSTR("GM862 GPS Get\n"));
        gm862_gps_position_get();
      }
      else if(data0 == 'd')
      {
        uart0_puts_P((const u08*)PSTR("GM862 Disable\n"));
        gm862_gps_disable();
        gm862_gsm_disable();
        gm862_disable();
      }
      else if(data0 == 'é')
      {
        uart0_puts_P((const u08*)PSTR("Direct routing to GM862, 'é' to STOP\n"));
        /* Stay in this loop until we receive "end of routing" char */
        while(1)
        {
          /* Get data from user */
          data0 = fifo_get(&uart0_rx_fifo); if(data0=='é') { break; }
          /* If data is valid send it to GM862 */
          if(data0!=0xFFFF) { uart1_putc(data0); }
          /* Get data from GM862 */
          data1 = fifo_get(&uart1_rx_fifo);
          /* If data is valid send it to user */
          if(data1!=0xFFFF) { uart0_putc(data1); }
        }
        uart0_puts_P((const u08*)PSTR("\nDirect routing to GM862 STOPPED\n"));
      }
      else if(data0 == 'l')
      {
        uart0_puts_P((const u08*)PSTR("Config Load"));
        config_load();
        config_keyboard_load();
        config_show();
      }
      else if(data0 == 'm')
      {
        uart0_puts_P((const u08*)PSTR("Config Save"));
        config_show();
        config_save();
        config_keyboard_save();
      }
      else if(data0 == 'j')
      {
        uart0_puts_P((const u08*)PSTR("Config Show"));
        config_show();
      }
      else if(data0 == 'k')
      {
        uart0_puts_P((const u08*)PSTR("Config Get :"));
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Alarm Monitoring Use (")); uart0_putd('d',(u32)(config_alarm.monitoring_use)); uart0_puts_P((const u08*)PSTR(") : "));
          /* Wait until we get usable data */
          do { data0 = fifo_get(&uart0_rx_fifo); } while(data0 == 0xFFFF);
          /* If it is NOT end of input */
          if((data0 != '\n') && (data0 != '\r'))
          {
            /* Compute the user value */
            i = (u08)data0; config_alarm.monitoring_use = (u08)string2u16(&i, 1);
            /* Echo to the user */
            uart0_putd('d',(u32)(config_alarm.monitoring_use));
          }
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Alarm Monitoring Delay x8 (max 450=1h) (")); uart0_putd('d',(u32)(config_alarm.mon_delay)); uart0_puts_P((const u08*)PSTR(") : "));
          data1=0;
          char_received=0;
          /* Check if we are out of range u16 for next input */
          while(data1<6554)
          {
            /* Wait until we get usable data 0-9 or end of input by CR-LF*/
            do { data0 = fifo_get(&uart0_rx_fifo); } while(((data0 == 0xFFFF) || (data0 < '0') || (data0 > '9')) && (data0 != '\n') && (data0 != '\r'));
            /* If it is end of input */
            if((data0=='\n') || (data0=='\r')) { break; }
            /* Else echo the usable data */
            uart0_putc((u08)data0);
            /* Compute data */
            data1 = data1*10; data1 = data1 + (((u08)data0) - '0');
            /* Say that valid data has been received */
            char_received=1;
          }
          if(char_received==1)
          {
            if(data1>450) { data1=450; }
            config_alarm.mon_delay = data1;
          }
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Alarm Wait Delay x8 (max 450=1h) (")); uart0_putd('d',(u32)(config_alarm.wait_delay)); uart0_puts_P((const u08*)PSTR(") : "));
          data1=0;
          char_received=0;
          /* Check if we are out of range u16 for next input */
          while(data1<6554)
          {
            /* Wait until we get usable data 0-9 or end of input by CR-LF*/
            do { data0 = fifo_get(&uart0_rx_fifo); } while(((data0 == 0xFFFF) || (data0 < '0') || (data0 > '9')) && (data0 != '\n') && (data0 != '\r'));
            /* If it is end of input */
            if((data0=='\n') || (data0=='\r')) { break; }
            /* Else echo the usable data */
            uart0_putc((u08)data0);
            /* Compute data */
            data1 = data1*10; data1 = data1 + (((u08)data0) - '0');
            /* Say that valid data has been received */
            char_received=1;
          }
          if(char_received==1)
          {
            if(data1>450) { data1=450; }
            config_alarm.wait_delay = data1;
          }
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Alarm Buzzer Delay (max 2184s) (")); uart0_putd('d',(u32)(config_alarm.buzzer_delay)); uart0_puts_P((const u08*)PSTR(") : "));
          data1=0;
          char_received=0;
          /* Check if we are out of range u16 for next input */
          while(data1<6554)
          {
            /* Wait until we get usable data 0-9 or end of input by CR-LF*/
            do { data0 = fifo_get(&uart0_rx_fifo); } while(((data0 == 0xFFFF) || (data0 < '0') || (data0 > '9')) && (data0 != '\n') && (data0 != '\r'));
            /* If it is end of input */
            if((data0=='\n') || (data0=='\r')) { break; }
            /* Else echo the usable data */
            uart0_putc((u08)data0);
            /* Compute data */
            data1 = data1*10; data1 = data1 + (((u08)data0) - '0');
            /* Say that valid data has been received */
            char_received=1;
          }
          if(char_received==1)
          {
            if(data1>2184) { data1=2184; }
            config_alarm.buzzer_delay = data1;
          }
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Alarm Tel0 (12 car) (")); uart0_puts(&(config_alarm.tel0[0])); uart0_puts_P((const u08*)PSTR(") : "));
          /* Get tel number "+336123456789" */
          for(i=0; i<12; i++)
          {
            /* Wait for data, no check on data */
            do { data0 = fifo_get(&uart0_rx_fifo); } while(data0 == 0xFFFF);
            /* If it is end of input */
            if((data0=='\n') || (data0=='\r')) { break; }
            config_alarm.tel0[i] = (u08) data0;
            /* Echo to the user */
            uart0_putc(config_alarm.tel0[i]);
          }
          /* Compute end of string */
          config_alarm.tel0[12]='\0';
          /* WARN the user if it is incorrect but valuable */
          if(!gm862_gsm_tel_valid(&(config_alarm.tel0[0]))) { uart0_puts_P((const u08*)PSTR("\nWARNING : Tel0 is INVALID !")); }
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Alarm Tel1 (12 car) (")); uart0_puts(&(config_alarm.tel1[0])); uart0_puts_P((const u08*)PSTR(") : "));
          /* Get tel number "+336123456789" */
          for(i=0; i<12; i++)
          {
            /* Wait for data, no check on data */
            do { data0 = fifo_get(&uart0_rx_fifo); } while(data0 == 0xFFFF);
            /* If it is end of input */
            if((data0=='\n') || (data0=='\r')) { break; }
            config_alarm.tel1[i] = (u08) data0;
            /* Echo to the user */
            uart0_putc(config_alarm.tel1[i]);
          }
          /* Compute end of string */
          config_alarm.tel1[12]='\0';
          /* WARN the user if it is incorrect but valuable */
          if(!gm862_gsm_tel_valid(&(config_alarm.tel1[0]))) { uart0_puts_P((const u08*)PSTR("\nWARNING : Tel1 is INVALID !")); }
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Keyboard Code (")); uart0_putd('d',(u32)CONFIG_CODE_NB); uart0_puts_P((const u08*)PSTR(" num) (")); for(i=0; i<CONFIG_CODE_NB; i++) { uart0_putc(config_keyboard.code[i]+'0'); } uart0_puts_P((const u08*)PSTR(") : "));
          for(i=0; i<CONFIG_CODE_NB; i++)
          {
            /* Wait until we get usable data 0-9 */
            do { data0 = fifo_get(&uart0_rx_fifo); } while(((data0 == 0xFFFF) || (data0 < '0') || (data0 > '9')) && (data0 != '\n') && (data0 != '\r'));
            /* If it is end of input */
            if((data0=='\n') || (data0=='\r')) { break; }
            /* Compute data */
            config_keyboard.code[i] = ((u08) data0) - '0';
            /* Echo to the user */
            uart0_putc(config_keyboard.code[i]+'0');
          }
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Keyboard Delay (max 2184s) (")); uart0_putd('d',(u32)(config_keyboard.delay)); uart0_puts_P((const u08*)PSTR(") : "));
          data1=0;
          char_received=0;
          /* Check if we are out of range u16 for next input */
          while(data1<6554)
          {
            /* Wait until we get usable data 0-9 or end of input by CR-LF*/
            do { data0 = fifo_get(&uart0_rx_fifo); } while(((data0 == 0xFFFF) || (data0 < '0') || (data0 > '9')) && (data0 != '\n') && (data0 != '\r'));
            /* If it is end of input */
            if((data0=='\n') || (data0=='\r')) { break; }
            /* Else echo the usable data */
            uart0_putc((u08)data0);
            /* Compute data */
            data1 = data1*10; data1 = data1 + (((u08)data0) - '0');
            /* Say that valid data has been received */
            char_received=1;
          }
          if(char_received==1)
          {
            if(data1>2184) { data1=2184; }
            config_keyboard.delay = data1;
          }
        fifo_flush(&uart0_rx_fifo);
        uart0_puts_P((const u08*)PSTR("\n  Keyboard Debug0 (")); uart0_putd('d',(u32)(config_keyboard.debug0)); uart0_puts_P((const u08*)PSTR(") : "));
          data1=0;
          char_received=0;
          /* Check if we are out of range u16 for next input */
          while(data1<6554)
          {
            /* Wait until we get usable data 0-9 or end of input by CR-LF*/
            do { data0 = fifo_get(&uart0_rx_fifo); } while(((data0 == 0xFFFF) || (data0 < '0') || (data0 > '9')) && (data0 != '\n') && (data0 != '\r'));
            /* If it is end of input */
            if((data0=='\n') || (data0=='\r')) { break; }
            /* Else echo the usable data */
            uart0_putc((u08)data0);
            /* Compute data */
            data1 = data1*10; data1 = data1 + (((u08)data0) - '0');
            /* Say that valid data has been received */
            char_received=1;
          }
          if(char_received==1)
          {
            if(data1>1) { data1=1; }
            config_keyboard.debug0 = data1;
          }
        uart0_puts_P((const u08*)PSTR("\nConfig GET END !\n"));
      }
      else if(data0 == 'a')
      {
        alarm_monitoring_on();
        uart0_puts_P((const u08*)PSTR("Alarm MON ON\n"));
      }
      else if(data0 == 'q')
      {
        alarm_monitoring_off();
        uart0_puts_P((const u08*)PSTR("Alarm MON OFF\n"));
      }
      else if(data0 == '1')
      {
        data0=power_temperature();
        uart0_puts_P((const u08*)PSTR("Temperature : ")); uart0_putd('d', (u32)data0); uart0_puts_P((const u08*)PSTR("\n"));
      }
      else if(data0 == '2')
      {
        data0=power_self_voltage();
        uart0_puts_P((const u08*)PSTR("Self Voltage : ")); uart0_putd('d', (u32)data0); uart0_puts_P((const u08*)PSTR("\n"));
      }
      else if(data0 == '3')
      {
        data0=power_car_voltage();
        uart0_puts_P((const u08*)PSTR("Car Voltage : ")); uart0_putd('d', (u32)data0); uart0_puts_P((const u08*)PSTR("\n"));
      }
      else if(data0 == 'b')
      {
        uart0_puts_P((const u08*)PSTR("Keyboard Touch get (Press ENTER when ready) : "));
        /* Wait for user signal ready */
        do { data0 = fifo_get(&uart0_rx_fifo); } while((data0!='\n') && (data0!='\r'));
        /* Do the acquisition */
        data0=keyboard_touch_get();
        /* Print it */
        uart0_putd('d', (u32)data0); uart0_puts_P((const u08*)PSTR("\n"));
      }
      else
      {
        if(data0 != 0xFFFF)
        {
          uart0_puts_P((const u08*)PSTR("CMD Unknown : ")); uart0_putd('d',(u32)(data0)); uart0_puts_P((const u08*)PSTR("\n"));
        }
      }
    }
    /* Reset SEM to prevent unautorized cycle */
    config_sem = CONFIG_SEM_WAIT;
    /* Enable receive mechanism on UART1 */
    gm862_gsm_receive_enable();
  }
}
