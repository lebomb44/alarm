#ifndef _UTR2_
#define _UTR2_

#define UTR2_SENSE_DDR DDRE
#define UTR2_SENSE_PORT PORTE
#define UTR2_SENSE_PIN PINE
#define UTR2_SENSE_BIT 6

#define UTR2_LED_CONTROL_DDR DDRB
#define UTR2_LED_CONTROL_PORT PORTB
#define UTR2_LED_CONTROL_PIN PINB
#define UTR2_LED_CONTROL_BIT 4

#define UTR2_POWER_DDR DDRB
#define UTR2_POWER_PORT PORTB
#define UTR2_POWER_PIN PINB
#define UTR2_POWER_BIT 3

void utr2_init(void);
void utr2_enable(void);
void utr2_disable(void);

#endif
