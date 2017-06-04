#ifndef _MM53200_
#define _MM53200_

// Temps des bits pour un quartz a 8 MHz
#define LOW0 65339 //292 us 65256
#define HIGH0 64745 //692 us
#define LOW1 65000 //590 us
#define HIGH1 65075 //385 us

// SIGNAL
#define SIGNALPORT PORTD
#define SIGNALPIN PIND
#define SIGNALBIT 1

#define SIGNALON() {sbi(SIGNALPORT,SIGNALBIT);}
#define SIGNALOFF() {cbi(SIGNALPORT,SIGNALBIT);}
#define SIGNALTOGGLE() {if(bit_is_set(SIGNALPIN,SIGNALBIT)) cbi(SIGNALPORT,SIGNALBIT); else sbi(SIGNALPORT,SIGNALBIT);}

#define maison 0x07B9
#define viala 0x07B8

void init_MM53200(u16 code);
void send_MM53200(void);

#endif
