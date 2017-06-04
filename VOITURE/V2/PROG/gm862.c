#include "global.h"
#include "gm862.h"
#include "string.h"
#include "timer.h"
#include "uart0.h"
#include "uart1.h"
#include "config.h"
#include "int.h"
#include "power.h"
#include "keyboard.h"
#include "buzzer.h"
#include "alarm.h"

u08 gm862_gsm_msg[GM862_GSM_MSG_SIZE];

GM862_RECEIVE_SEM_T gm862_receive_sem = GM862_RECEIVE_SEM_WAIT;
GM862_RECEIVE_STATE_T gm862_receive_state = GM862_RECEIVE_STATE_OFF;
GM862_GPS_POSITION_T gm862_gps_position;

#define GM862_LINE_SIZE 512
u08 gm862line[GM862_LINE_SIZE];

u08 gm862_cmd(const u08* cmd)
{
  /* Garbage all waiting char */
  fifo_flush(&uart1_rx_fifo);
  /* Send command but begin with CR to force synchro new command */
  uart1_puts_P((const u08*)PSTR("\r")); uart1_puts_P(cmd); uart1_puts_P((const u08*)PSTR("\r"));
  /* Wait for received line with timeout */
  string_getLine(&uart1_rx_fifo, &gm862line[0], GM862_LINE_SIZE, 1000);
  DEBUG0_PUTS(&gm862line[0]); DEBUG0_PUTS_P((const u08*)PSTR(" CMD\r"));
  /* If line is equal to what we sent it means that the command has been correctly received */
  if(string_ncmp_P(&gm862line[0], cmd, string_len_P(cmd))) { DEBUG0_PUTS_P((const u08*)PSTR("CMD is OK\r")); return 0; }
  /* Else it is KO */
  DEBUG0_PUTS_P((const u08*)PSTR("CMD is KO\r"));
  return 1;
}

u08 gm862_ack(const u08* acq)
{
  /* Wait for acknowlege line with timeout */
  string_getLine(&uart1_rx_fifo, &gm862line[0], GM862_LINE_SIZE, 1000);
  DEBUG0_PUTS(&gm862line[0]); DEBUG0_PUTS_P((const u08*)PSTR(" ACK\r"));
  /* If acknowledge is equal to what we are expecting */
  if(string_ncmp_P(&gm862line[0], acq, string_len_P(acq))) { DEBUG0_PUTS_P((const u08*)PSTR("ACQ is OK\r")); return 0; }
  /* Else it is KO */
  DEBUG0_PUTS_P((const u08*)PSTR("ACK is KO\r"));
  return 1;
}

u08 gm862_ack_OK(void)
{
  return gm862_ack((const u08*)PSTR("OK"));
}

u08 gm862_cmd_ack_OK(const u08* cmd)
{
  if(gm862_cmd(cmd)) { return 1; }
  if(gm862_ack_OK()) { return 2; }
  return 0;
}

void gm862_init(void)
{
  /* Init HW ports */
  cbi(GM862_POWER_PORT, GM862_POWER_BIT);
  sbi(GM862_POWER_DDR , GM862_POWER_BIT);

  cbi(GM862_ENABLE_PORT, GM862_ENABLE_BIT);
  sbi(GM862_ENABLE_DDR , GM862_ENABLE_BIT);

  sbi(GM862_PWRMON_PORT, GM862_PWRMON_BIT);
  cbi(GM862_PWRMON_DDR , GM862_PWRMON_BIT);

  cbi(GM862_RESET_PORT, GM862_RESET_BIT);
  sbi(GM862_RESET_DDR , GM862_RESET_BIT);

  sbi(GM862_LED_STATUS_PORT, GM862_LED_STATUS_BIT);
  cbi(GM862_LED_STATUS_DDR , GM862_LED_STATUS_BIT);

  cbi(GM862_SENSE_PORT, GM862_SENSE_BIT);
  cbi(GM862_SENSE_DDR , GM862_SENSE_BIT);

  /* Init STATES */
  gm862_receive_sem   = GM862_RECEIVE_SEM_WAIT;
  gm862_receive_state = GM862_RECEIVE_STATE_OFF;

  gm862_gps_position.hour         = 0;
  gm862_gps_position.minutes      = 0;
  gm862_gps_position.secondes     = 0;
  gm862_gps_position.lat_sign     = 0;
  gm862_gps_position.lat_degrees  = 0;
  gm862_gps_position.lat_minutes  = 0;
  gm862_gps_position.long_sign    = 0;
  gm862_gps_position.long_degrees = 0;
  gm862_gps_position.long_minutes = 0;
  gm862_gps_position.nb_sat       = 0;
}

u08 gm862_power_on(void)
{
  u16 timeout=0;
  /* Electricaly power OFF */
  gm862_power_off();
  /* Electricaly power ON */
  sbi(GM862_POWER_PORT, GM862_POWER_BIT);
  /* Wait 2s for startup and capacitor */
  delay_ms(2000);
  /* Check that LED status is ON while blinking */
  timeout = 300; while(timeout) { if(gm862_led_status_state()) { break; } delay_ms(10); timeout--; } if(timeout == 0) { gm862_power_off(); return 1; }
  return 0;
}

void gm862_power_off(void)
{
  /* Electricaly power OFF */
  cbi(GM862_POWER_PORT, GM862_POWER_BIT);
  delay_ms(2000);
}

u08 gm862_enable(void)
{
  u08 timeout=0;
  /* Send a pulse on enable pin */
  sbi(GM862_ENABLE_PORT, GM862_ENABLE_BIT);
  delay_ms(2000);
  cbi(GM862_ENABLE_PORT, GM862_ENABLE_BIT);
  delay_ms(1000);
  /* Verify that embeded SW has started up */
  timeout = 30; while(timeout) { if(gm862_pwrmon_state())                                   { break; } delay_ms(100);  timeout--; } if(timeout == 0) { gm862_power_off(); return 1; }
  /* Verify UART link */
  timeout = 5;  while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT"))          ==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { gm862_power_off(); return 1; }
  /* Set UART baud rate */
  timeout = 5;  while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+IPR=38400"))==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { gm862_power_off(); return 1; }
  /* Say that we are in Europe */
  timeout = 5;  while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT#BND=0"))    ==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { gm862_power_off(); return 1; }
  /* Select advanced mode */
  timeout = 5;  while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT#SELINT=2")) ==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { gm862_power_off(); return 1; }
  timeout = 5;  while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT#SMSMODE=1"))==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { gm862_power_off(); return 1; }
  /* Select basic error message */
  timeout = 5;  while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CMEE=0"))   ==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { gm862_power_off(); return 1; }
  delay_ms(2000);
  return 0;
}

u08 gm862_disable(void)
{
  gm862_gsm_disable();
  /* Shutdown embeded SW and disconnect from GSM network */
  return gm862_cmd_ack_OK((const u08*)PSTR("AT#SHDN"));
}

void gm862_reset(void)
{
  /* Reset SW by using HW reset */
  sbi(GM862_RESET_PORT, GM862_RESET_BIT);
  delay_ms(2000);
  cbi(GM862_RESET_PORT, GM862_RESET_BIT);
}

u08 gm862_led_status_state(void)
{
  /* Read on HW pin */
  if(bit_is_set(GM862_LED_STATUS_PIN, GM862_LED_STATUS_BIT))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

u08 gm862_pwrmon_state(void)
{
  /* Read status on HW pin */
  if(bit_is_set(GM862_PWRMON_PIN, GM862_PWRMON_BIT))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/* WARNING : this function is NOT used */
u08 gm862_alarm_state(void)
{
  /* Read alarm in HW pin */
  if(bit_is_set(GM862_SENSE_PIN, GM862_SENSE_BIT))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/* Connect GM862 to GSM network */
u08 gm862_gsm_enable(void)
{
  u08 timeout=0;

  /* Send PIN authentification */
  timeout = 5;  while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CPIN=0000"))==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { gm862_power_off(); return 2; }
  /* Select TEXT mode */
  timeout = 5;  while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CMGF=1"))   ==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { gm862_gsm_disable(); gm862_power_off(); return 2; }
  timeout = 30;
  while(timeout)
  {
    /* Wait for end of connection to GSM network */
    if(gm862_cmd((const u08*)PSTR("AT+CREG?"))==0)
    {
      if(gm862_ack((const u08*)PSTR("+CREG: 0,"))==0)
      {
        if(gm862line[9]=='1') { if(gm862_ack_OK()==0) { break; } }
      }
    }
    delay_ms(2000);
    timeout--;
  }
  /* If timeout the module is NOT usable ! */
  if(timeout == 0) { gm862_disable(); gm862_power_off(); return 3; }
  return 0;
}

u08 gm862_gsm_disable(void)
{
  return gm862_gsm_receive_disable();
}

u08 gm862_gsm_tel_valid(u08* tel)
{
  u08 i=0;
  u08 n=0;
  /* Check Tel number before using IT ! */
  if((tel[0] == '+') && (tel[1] == '3') && (tel[2] == '3'))
  {
    n=0;
    for(i=3; i<12; i++) { if(('0'<=tel[i]) && (tel[i]<='9')) { n++; } }
    if(n==9) { return 1; }
  }
  return 0;
}

u08 gm862_gsm_send(u08* s, u08* tel)
{
  u08 i=0;
  u16 timeout=0;
  DEBUG0_PUTS_P((const u08*)PSTR("GSM sending SMS...\n"));
  /* Check if tel is usable */
  if(gm862_gsm_tel_valid(&tel[0]))
  {
    DEBUG0_PUTS_P((const u08*)PSTR("Tel is valid\n"));
    fifo_flush(&uart1_rx_fifo);
    /* Send SMS command */
    uart1_puts_P((const u08*)PSTR("\r")); uart1_puts_P((const u08*)PSTR("AT+CMGS=\""));
    /* Followed by tel number */
    for(i=0; i<12; i++) { uart1_putc(tel[i]); } uart1_puts_P((const u08*)PSTR("\"\r"));
    delay_ms(2000);
    /* Wait for echo on command */
    if(gm862_ack((const u08*)PSTR("AT+CMGS="))==0)
    {
      DEBUG0_PUTS_P((const u08*)PSTR("GSM Send cmd OK\n"));
      /* Now check the echo on tel number */
      for(i=0; i<12; i++) { if(gm862line[9+i] != tel[i]) { uart1_putc(0x1B); uart1_putc('\r'); return 2; }}
      DEBUG0_PUTS_P((const u08*)PSTR("GSM Send tel OK\n"));
      /* Wait for promt on SMS message */
      timeout = 10; while((fifo_get(&uart1_rx_fifo) != '>') && (timeout > 0)) { delay_ms(1000); timeout--; }
      /* If no promt until timeout finish command and return */
      if(timeout==0) { uart1_putc(0x1B); uart1_putc('\r'); return 3; }
      DEBUG0_PUTS_P((const u08*)PSTR("SMS prompt OK\n"));
      /* Now send the SMS message */
      uart1_puts(s);
      /* Delay before end of message */
      delay_ms(1000);
      /* Say that the message is finished */
      uart1_putc(0x1A); uart1_putc('\r'); uart1_putc('\r'); /* CTRL+Z : send */
      fifo_flush(&uart1_rx_fifo);
      DEBUG0_PUTS_P((const u08*)PSTR("SMS sent !\n"));
      delay_ms(10000);
    }
    else
    {
      DEBUG0_PUTS_P((const u08*)PSTR("ERROR while trying to send SMS !\n"));
      return 1;
    }
  }
  return 0;
}

/* Construc standard message */
void gm862_gsm_cat_alarm_msg(void)
{
  u08 temp[11];
  /* Add information about alarm detection */
  if(alarm_detection_state != ALARM_DETECTION_STATE_NONE)
  {
    string_ncat_P((const u08*)PSTR("ALARM CAR !\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
    string_ncat_P((const u08*)PSTR("DETECTION ON :"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
    if(alarm_detection_state & ALARM_DETECTION_STATE_UTR2 ) { string_ncat_P((const u08*)PSTR(" UTR2" ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); }
    if(alarm_detection_state & ALARM_DETECTION_STATE_INT  ) { string_ncat_P((const u08*)PSTR(" INT "  ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
                                                               u32_2string('b', (u32)int_value_init_get(), &temp[0]); string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
                                                               string_ncat_P((const u08*)PSTR("->"  ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
                                                               u32_2string('b', (u32)int_value_isr_get(), &temp[0]); string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
                                                            }
    if(alarm_detection_state & ALARM_DETECTION_STATE_POWER) { string_ncat_P((const u08*)PSTR(" POWER"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); }
    string_ncat_P((const u08*)PSTR("\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  }
  /* Construct GPS localisation on google maps */
  string_ncat_P((const u08*)PSTR("http://maps.google.fr/maps?q="), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  /* Cat latitude */
  temp[0]=gm862_gps_position.lat_sign; temp[1]='\0';
  string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  u32_2string('d', (u32) gm862_gps_position.lat_degrees, &temp[0]);
  string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  string_ncat_P((const u08*)PSTR("."), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  u32_2string('d', (gm862_gps_position.lat_minutes*1000)/6, &temp[0]);
  string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  string_ncat_P((const u08*)PSTR(","), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);

  /* Cat longitude */
  temp[0]=gm862_gps_position.long_sign; temp[1]='\0';
  string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  u32_2string('d', (u32) gm862_gps_position.long_degrees, &temp[0]);
  string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  string_ncat_P((const u08*)PSTR("."), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  u32_2string('d', (gm862_gps_position.long_minutes*1000)/6, &temp[0]);
  string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  string_ncat_P((const u08*)PSTR("&z=17"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  string_ncat_P((const u08*)PSTR("\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);

  /* Add power level (unit mV) */
  string_ncat_P((const u08*)PSTR("Batt : "), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  /* Concert ADC to mV */
  u32_2string('d', ((((u32)power_self_voltage())-8135)*1000)/3711, &temp[0]);
  string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
  string_ncat_P((const u08*)PSTR(" mV\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
}

u08 gm862_gsm_receive_on(void)
{
  u08 timeout=0;

  fifo_flush(&uart1_rx_fifo);

  /* Choose which kind of asynchronous UART message we want on SMS reception */
  timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CNMI=3,3,2,1,1")) ==0) { break; } delay_ms(5000); timeout--; } if(timeout == 0) { return 1; }
  /* Semect TEXT mode */
  timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CMGF=1"))         ==0) { break; } delay_ms(5000); timeout--; } if(timeout == 0) { return 2; }

  delay_ms(1000);
  /* Flush the receive buffer to ignore response char */
  fifo_flush(&uart1_rx_fifo);

  return 0;
}

u08 gm862_gsm_receive_enable(void)
{
  /* Flush the receive buffer to ignore previous char */
  fifo_flush(&uart1_rx_fifo);
  /* Enable routing in UART1 RX INT */
  gm862_receive_state = GM862_RECEIVE_STATE_ON;
  return 0;
}

u08 gm862_gsm_receive_disable(void)
{
  /* Disable routing in UART1 RX INT */
  gm862_receive_state = GM862_RECEIVE_STATE_OFF;
  /* Flush the receive buffer to ignore previous char */
  fifo_flush(&uart1_rx_fifo);
  return 0;
}

u08 gm862_gsm_callback(void)
{
  u08 temp[11];
  u08 tel[13];
  u08 i=0;
  /* Wait to receive the complete message */
  delay_ms(3000);
  /* Check that it is a NEW (unread) message */
  if(gm862_ack((const u08*)PSTR("+CMGR: \"REC UNREAD\",\""))==0)
  {
    /* Check the tel number from which the message has been received */
    if(gm862_gsm_tel_valid(&gm862line[21]))
    {
      /* Get the usable tel number */
      for(i=0; i<12; i++) { tel[i] = gm862line[21+i]; }
      gm862_gsm_msg[0]='\0';
      /* Analyse the SMS message */
      do
      {
        /* Wait for the complete line */
        delay_ms(1000);
        /* Get each line and analyse it */
        string_getLine(&uart1_rx_fifo, &gm862line[0], GM862_LINE_SIZE, 1000);
        DEBUG0_PUTS(&gm862line[0]);
        if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("TEL0="), sizeof("TEL0=")-1))
        {
          if(gm862_gsm_tel_valid(&gm862line[5])) { for(i=0; i<12; i++) { config_alarm.tel0[i] = gm862line[5+i]; } config_alarm.tel0[12]='\0'; string_ncat_P((const u08*)PSTR("TEL0 OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); }
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("TEL1="), sizeof("TEL1=")-1))
        {
          if(gm862_gsm_tel_valid(&gm862line[5])) { for(i=0; i<12; i++) { config_alarm.tel1[i] = gm862line[5+i]; } config_alarm.tel1[12]='\0'; string_ncat_P((const u08*)PSTR("TEL1 OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); }
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("MON_USE="), sizeof("MON_USE=")-1))
        {
          if(('0'<=gm862line[8]) && (gm862line[8]<='7')) { config_alarm.monitoring_use = gm862line[8]-'0'; string_ncat_P((const u08*)PSTR("MON_USE OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); }
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("CONFIG_LOAD"), sizeof("CONFIG_LOAD")-1))
        {
          config_load(); config_keyboard_load(); string_ncat_P((const u08*)PSTR("CONFIG_LOAD OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("CONFIG_MEM"), sizeof("CONFIG_MEM")-1))
        {
          config_save(); config_keyboard_save(); config_load(); config_keyboard_load(); string_ncat_P((const u08*)PSTR("CONFIG_MEM OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("CONFIG_SHOW"), sizeof("CONFIG_SHOW")-1))
        {
          string_ncat_P((const u08*)PSTR("MON="            ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); u32_2string('d', (u32)(config_alarm.monitoring_use), &temp[0]); string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
          string_ncat_P((const u08*)PSTR("\nMON_DELAY="    ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); u32_2string('d', (u32)(config_alarm.mon_delay     ), &temp[0]); string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
          string_ncat_P((const u08*)PSTR("\nWAIT_DELAY="   ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); u32_2string('d', (u32)(config_alarm.wait_delay    ), &temp[0]); string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
          string_ncat_P((const u08*)PSTR("\nBUZZER_DELAY=" ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); u32_2string('d', (u32)(config_alarm.buzzer_delay  ), &temp[0]); string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
          string_ncat_P((const u08*)PSTR("\nTEL0="         ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); string_ncat(&config_alarm.tel0[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
          string_ncat_P((const u08*)PSTR("\nTEL1="         ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE); string_ncat(&config_alarm.tel1[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
          string_ncat_P((const u08*)PSTR("\n"              ), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("GPS_ON"), sizeof("GPS_ON")-1))
        {
          gm862_gps_enable(); string_ncat_P((const u08*)PSTR("GPS_ON OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("GPS_OFF"), sizeof("GPS_OFF")-1))
        {
          gm862_gps_disable(); string_ncat_P((const u08*)PSTR("GPS_OFF OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("GPS_GET"), sizeof("GPS_GET")-1))
        {
          gm862_gps_position_get(); string_ncat_P((const u08*)PSTR("GPS_GET OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("CODE_SHOW"), sizeof("CODE_SHOW")-1))
        {
          string_ncat_P((const u08*)PSTR("CODE="), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
          for(i=0; i<CONFIG_CODE_NB; i++) { temp[i] = config_keyboard.code[i]+'0'; } temp[i] = '\n'; i++; temp[i] = '\0';
          string_ncat(&temp[0], &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("CODE="), sizeof("CODE=")-1))
        {
          for(i=0; i<CONFIG_CODE_NB; i++) { config_keyboard.code[i] = gm862line[5+i]-'0'; }
          string_ncat_P((const u08*)PSTR("CODE OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("BUZZER_ON"), sizeof("BUZZER_ON")-1))
        {
          buzzer_enable(0xFFFF); string_ncat_P((const u08*)PSTR("BUZZER_ON OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("BUZZER_OFF"), sizeof("BUZZER_OFF")-1))
        {
          buzzer_disable(); string_ncat_P((const u08*)PSTR("BUZZER_OFF OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("ALARM_MON_ON"), sizeof("ALARM_MON_ON")-1))
        {
          alarm_monitoring_on(); string_ncat_P((const u08*)PSTR("ALARM_MON_ON OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("ALARM_MON_OFF"), sizeof("ALARM_MON_OFF")-1))
        {
          alarm_monitoring_off(); string_ncat_P((const u08*)PSTR("ALARM_MON_OFF OK\n"), &gm862_gsm_msg[0], GM862_GSM_MSG_SIZE);
        }
        else if(string_ncmp_P(&gm862line[0], (const u08*)PSTR("POWER_OFF"), sizeof("POWER_OFF")-1))
        {
          /* Stop alarm monitoring */
          alarm_monitoring_off();
          /* Try to disconnect from GSM network properly */
          if(gm862_disable()) { delay_ms(1000); gm862_disable(); } else { delay_ms(3000); }
          DEBUG0_PUTS_P((const u08*)PSTR("Alarm monitoring STOPPED\n"));
          /* Power OFF the system */
          power_disable();
        }
      }
      while (gm862line[0]!='\0');
      /* Add standard message at end of response */
      gm862_gsm_cat_alarm_msg();
      /* Send the total message */
      gm862_gsm_send(&gm862_gsm_msg[0], &tel[0]);
      return 1;
    }
  }
  return 0;
}

u08 gm862_gsm_receive(void)
{
  u08 timeout=0;
  /* Check the five starge zone in SIM card for SMS */
  /* Read the storage zone */
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGR=1"))==0) { break; } delay_ms(5000); timeout--; }
  /* Analyse the response and build answer SMS */
  if(gm862_gsm_callback()) { timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CMGD=1"))==0) { break; } delay_ms(5000); timeout--; } }
  /* Now delete the stored SMS */
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGD=1"))==0) { break; } delay_ms(5000); timeout--; }
  delay_ms(1000);
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGR=2"))==0) { break; } delay_ms(5000); timeout--; }
  if(gm862_gsm_callback()) { timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CMGD=2"))==0) { break; } delay_ms(5000); timeout--; } }
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGD=2"))==0) { break; } delay_ms(5000); timeout--; }
  delay_ms(1000);
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGR=3"))==0) { break; } delay_ms(5000); timeout--; }
  if(gm862_gsm_callback()) { timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CMGD=3"))==0) { break; } delay_ms(5000); timeout--; } }
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGD=3"))==0) { break; } delay_ms(5000); timeout--; }
  delay_ms(1000);
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGR=4"))==0) { break; } delay_ms(5000); timeout--; }
  if(gm862_gsm_callback()) { timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CMGD=4"))==0) { break; } delay_ms(5000); timeout--; } }
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGD=4"))==0) { break; } delay_ms(5000); timeout--; }
  delay_ms(1000);
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGR=5"))==0) { break; } delay_ms(5000); timeout--; }
  if(gm862_gsm_callback()) { timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT+CMGD=5"))==0) { break; } delay_ms(5000); timeout--; } }
  timeout=5; while(timeout) { if(gm862_cmd((const u08*)PSTR("AT+CMGD=5"))==0) { break; } delay_ms(5000); timeout--; }
  delay_ms(1000);
  return 0;
}

u08 gm862_gps_enable(void)
{
  u08 timeout=0;
  u16 data=0;

  /* Power ON the GPS part of the module */
  timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT$GPSP=1")) ==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { return 1; }
  /* Reset the GPS part in Coldsatrt */
  timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT$GPSR=1")) ==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { return 1; }
  /* GPS Antenna supplied by the module */
  timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT$GPSAT=1"))==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { return 1; }

  timeout=5;
  while(timeout)
  {
    /* Acquire internal voltage on GPS part */
    if(gm862_cmd((const u08*)PSTR("AT$GPSAV?"))==0)
    {
      /* Wait for response */
      if(gm862_ack((const u08*)PSTR("$GPSAV:"))==0)
      {
        /* Convert the char into usable data */
        data = string2u16(&gm862line[8], 4);
        DEBUG0_PUTD('d', (u32) data);DEBUG0_PUTS_P((const u08*)PSTR(" mV\n"));
        /* Check the validity of the voltage */
        if((3400 < data) && (data < 4200))
        {
          if(gm862_ack_OK()==0) { break; }
        }
        /* If the voltage is incorrect we should power off the module */
        else { gm862_gps_disable(); }
      }
    }
    delay_ms(2000);
    timeout--;
  }
  if(timeout == 0) { return 2; }

  timeout=5;
  while(timeout)
  {
    /* Acquire internal current on GPS part */
    if(gm862_cmd((const u08*)PSTR("AT$GPSAI?"))==0)
    {
      /* Wait for response */
      if(gm862_ack((const u08*)PSTR("$GPSAI: "))==0)
      {
        /* Convert the char into usable data */
        data = string2u16(&gm862line[8], 2);
        DEBUG0_PUTD('d', (u32) data);DEBUG0_PUTS_P((const u08*)PSTR(" mA\n"));
        /* Check the validity of the current */
        if((5 < data) && (data < 60))
        {
          if(gm862_ack_OK()==0) { break; }
        }
        /* If the current is incorrect we should power off the module */
        else { gm862_gps_disable(); }
      }
    }
    delay_ms(2000);
    timeout--;
  }
  if(timeout == 0) { return 2; }
  return 0;
}

u08 gm862_gps_disable(void)
{
  u08 timeout=5;
  /* Only shutdown the GPS part of the module */
  timeout=5; while(timeout) { if(gm862_cmd_ack_OK((const u08*)PSTR("AT$GPSP=0"))==0) { break; } delay_ms(1000); timeout--; } if(timeout == 0) { return 1; }
  return 0;
}

u08 gm862_gps_position_get(void)
{
  u16 i=0;
  u08 n=0;
  u08 timeout=0;

  timeout=5;
  while(timeout)
  {
    /* Send command to get position */
    if(gm862_cmd((const u08*)PSTR("AT$GPSACP"))==0)
    {
      /* Wait for response on the command */
      if(gm862_ack((const u08*)PSTR("$GPSACP: "))==0)
      {
        /* Convert response into usable data */
        gm862_gps_position.hour        = (u08) string2u16(&gm862line[9 ], 2);
        gm862_gps_position.minutes     = (u08) string2u16(&gm862line[11], 2);
        gm862_gps_position.secondes    = (u08) string2u16(&gm862line[13], 2);

        /* Convert module data into google usable data */
        i=9; n=0; while((gm862line[i]!='\0') && (i<GM862_LINE_SIZE)) { if(gm862line[i]==',') { n++; if(n==1) { break; } } i++; }
        gm862_gps_position.lat_degrees = (u08) string2u16(&gm862line[i+1], 2);
        if(gm862line[i+10]=='S') { gm862_gps_position.lat_sign = '-'; } else { gm862_gps_position.lat_sign = '+'; }
        gm862_gps_position.lat_minutes = string2u16(&gm862line[i+3], 2);
        gm862_gps_position.lat_minutes = (gm862_gps_position.lat_minutes)*10000 + string2u16(&gm862line[i+6], 4);

        /* Convert module data into google usable data */
        i=9; n=0; while((gm862line[i]!='\0') && (i<GM862_LINE_SIZE)) { if(gm862line[i]==',') { n++; if(n==2) { break; } } i++; }
        gm862_gps_position.long_degrees = (u08) string2u16(&gm862line[i+1], 3);
        if(gm862line[i+11]=='W') { gm862_gps_position.long_sign = '-'; } else { gm862_gps_position.long_sign = '+'; }
        gm862_gps_position.long_minutes = string2u16(&gm862line[i+4], 2);
        gm862_gps_position.long_minutes = (gm862_gps_position.long_minutes)*10000 + string2u16(&gm862line[i+7], 4);

        /* Get the number of satellite used for the geolocalisation */
        i=9; n=0; while((gm862line[i]!='\0') && (i<GM862_LINE_SIZE)) { if(gm862line[i]==',') { n++; if(n==10) { break; } } i++; }
        gm862_gps_position.nb_sat = (u08) string2u16(&gm862line[i+1], 2);

        DEBUG0_PUTS(&gm862line[0]); DEBUG0_PUTS_P((const u08*)PSTR("\r"));
        DEBUG0_PUTD('d', (u32) gm862_gps_position.hour);         DEBUG0_PUTS_P((const u08*)PSTR(" hour\n"));
        DEBUG0_PUTD('d', (u32) gm862_gps_position.minutes);      DEBUG0_PUTS_P((const u08*)PSTR(" minutes\n"));
        DEBUG0_PUTD('d', (u32) gm862_gps_position.secondes);     DEBUG0_PUTS_P((const u08*)PSTR(" secondes\n"));
        DEBUG0_PUTD('d', (u32) gm862_gps_position.lat_degrees);  DEBUG0_PUTS_P((const u08*)PSTR(" lat_degrees\n"));
        DEBUG0_PUTD('d', (u32) gm862_gps_position.lat_minutes);  DEBUG0_PUTS_P((const u08*)PSTR(" lat_minutes\n"));
        DEBUG0_PUTD('d', (u32) gm862_gps_position.long_degrees); DEBUG0_PUTS_P((const u08*)PSTR(" long_degrees\n"));
        DEBUG0_PUTD('d', (u32) gm862_gps_position.long_minutes); DEBUG0_PUTS_P((const u08*)PSTR(" long_minutes\n"));
        DEBUG0_PUTD('d', (u32) gm862_gps_position.nb_sat);       DEBUG0_PUTS_P((const u08*)PSTR(" nb_sat\n"));
      }
      /* ACK OK should also be received */
      if(gm862_ack_OK()==0) { break; }
    }
    delay_ms(2000);
    timeout--;
  }
  /* If this is the end because of timeout return ERROR */
  if(timeout == 0) { return 3; }
  /* Else OK */
  return 0;
}

void gm862_cycle(void)
{
  /* Compute only if IT UART1 RX gives us the SEM */
  if(gm862_receive_sem == GM862_RECEIVE_SEM_GET)
  {
    delay_ms(1000);
    /* Flush the buffer because previous data are NOT usable */
    fifo_flush(&uart1_rx_fifo);
    /* Dump and analyse the SIM message storage */
    gm862_gsm_receive();
    delay_ms(3000);
    /* Flush the buffer against remaining data */
    fifo_flush(&uart1_rx_fifo);
    gm862_receive_sem = GM862_RECEIVE_SEM_WAIT;
  }
}
