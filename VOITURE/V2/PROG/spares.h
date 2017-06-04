#ifndef _SPARES_
#define _SPARES_

#define SPARE1_DDR  DDRD
#define SPARE1_PORT PORTD
#define SPARE1_PIN  PIND
#define SPARE1_BIT  5

#define SPARE2_DDR  DDRD
#define SPARE2_PORT PORTD
#define SPARE2_PIN  PIND
#define SPARE2_BIT  4

#define SPARE3_DDR  DDRG
#define SPARE3_PORT PORTG
#define SPARE3_PIN  PING
#define SPARE3_BIT  4

#define SPARE4_DDR  DDRG
#define SPARE4_PORT PORTG
#define SPARE4_PIN  PING
#define SPARE4_BIT  3

#define SPARE5_DDR  DDRB
#define SPARE5_PORT PORTB
#define SPARE5_PIN  PINB
#define SPARE5_BIT  7

#define SPARE6_DDR  DDRB
#define SPARE6_PORT PORTB
#define SPARE6_PIN  PINB
#define SPARE6_BIT  6

#define SPARE7_DDR  DDRB
#define SPARE7_PORT PORTB
#define SPARE7_PIN  PINB
#define SPARE7_BIT  0

#define SPARE8_DDR  DDRE
#define SPARE8_PORT PORTE
#define SPARE8_PIN  PINE
#define SPARE8_BIT  7

#define SPARE9_DDR  DDRA
#define SPARE9_PORT PORTA
#define SPARE9_PIN  PINA
#define SPARE9_BIT  5

#define SPARE10_DDR  DDRA
#define SPARE10_PORT PORTA
#define SPARE10_PIN  PINA
#define SPARE10_BIT  4

#define SPARE11_DDR  DDRA
#define SPARE11_PORT PORTA
#define SPARE11_PIN  PINA
#define SPARE11_BIT  3

#define SPARE12_DDR  DDRA
#define SPARE12_PORT PORTA
#define SPARE12_PIN  PINA
#define SPARE12_BIT  2

#define SPARE13_DDR  DDRF
#define SPARE13_PORT PORTF
#define SPARE13_PIN  PINF
#define SPARE13_BIT  7

#define SPARE14_DDR  DDRF
#define SPARE14_PORT PORTF
#define SPARE14_PIN  PINF
#define SPARE14_BIT  6

#define SPARE15_DDR  DDRF
#define SPARE15_PORT PORTF
#define SPARE15_PIN  PINF
#define SPARE15_BIT  5

#define SPARE16_DDR  DDRF
#define SPARE16_PORT PORTF
#define SPARE16_PIN  PINF
#define SPARE16_BIT  4

#define SPARE17_DDR  DDRC
#define SPARE17_PORT PORTC
#define SPARE17_PIN  PINC
#define SPARE17_BIT  0

#define SPARE18_DDR  DDRC
#define SPARE18_PORT PORTC
#define SPARE18_PIN  PINC
#define SPARE18_BIT  1

#define SPARE19_DDR  DDRC
#define SPARE19_PORT PORTC
#define SPARE19_PIN  PINC
#define SPARE19_BIT  2

#define SPARE20_DDR  DDRC
#define SPARE20_PORT PORTC
#define SPARE20_PIN  PINC
#define SPARE20_BIT  3

#define SPARE21_DDR  DDRC
#define SPARE21_PORT PORTC
#define SPARE21_PIN  PINC
#define SPARE21_BIT  4

void spares_init(void);

#endif
