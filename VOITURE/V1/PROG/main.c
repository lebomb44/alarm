#include "global.h"
#include "eeprom.h"
#include "gsm.h"
#include "int.h"
#include "timer.h"
#include "uart.h"



void init(void)
{
  // Resistances de tirage
  PORTA=0x7D;
  PORTB=0x0F;
  PORTC=0xBB;
  PORTD=0xF7;
  PORTE=0xFD;
  PORTF=0xF6;
  PORTG=0xFF;
  // Initialisations des Ports
  DDRA=VAL_DDRA;
  DDRB=VAL_DDRB;
  DDRC=VAL_DDRC;
  DDRD=VAL_DDRD;
  DDRE=VAL_DDRE;
  DDRF=VAL_DDRF;
  DDRG=VAL_DDRG;
  // Initialisations des parametres
  adr_device = 100; //read_eeprom(0x00);
  adr_telecommande = 110; //read_eeprom(0x01);
  commande_on_avec_US = 120; //read_eeprom(0x10);
  commande_on_sans_US = 121; //read_eeprom(0x11);
  commande_off = 122; //read_eeprom(0x12);
  commande_etat = 123; //read_eeprom(0x13);
  commande_batt_voiture = 124;//read_eeprom(0x14);
  commande_alerte_choc = 125; //read_eeprom(0x15);
  commande_alerte_US = 126; //read_eeprom(0x16);
  commande_alerte_batterie = 127; //read_eeprom(0x17);
  // Initialisations des Interruptions Exterieures
  EICRB = 0x0B; // Front montant INT4, Front montant INT5
  // Initialisations de Timer0
  //TCCR0=0x00;		// Arret Timer0
  //sbi(TIMSK,0);    // Interruption Timer0 owerflow
  //TCNT0=0x00;         // Initialisation timer0
  // Initialisations de Timer1
  //TCCR1B=0x00;		// Arret Timer1
  //sbi(TIMSK,2);    // Interruption Timer1 owerflow
  //TCNT1=0x0000;         // Initialisation timer1
  // Initialisations de Timer2
  TCCR2=0x00;		// Arret Timer2
  sbi(TIMSK,6);    // Interruption Timer2 owerflow
  TCNT2=0x00;         // Initialisation timer2
  // Initialisations de Timer3
  TCCR3B=0x00;		// Arret Timer3
  sbi(ETIMSK,2);    // Interruption Timer3 owerflow
  TCNT3=0x0000;         // Initialisation timer3
  // Initialisation de la transmission 0
  UBRR0H=0x00;    // Baud Rate
  UBRR0L=0xCF;    // Baud Rate 2400 bps
  sbi(UCSR0B,2);  // Mode X bits
  //sbi(UCSR0B,3);  // Autorise transmition
  sbi(UCSR0B,4);  // Autorise reception
  UCSR0C=0x86;    // Mode 9 bits
  sbi(UCSR0B,7);  // Autorisation de l'Interruption Receiver Enable
  // Initialisation de la transmission 1
  UBRR1H=0x00;    // Baud Rate
  UBRR1L=0x0C;    // Baud Rate 38400 bps
  sbi(UCSR1B,2);  // Mode X bits
  sbi(UCSR1B,3);  // Autorise transmition
  //sbi(UCSR1B,4);  // Autorise reception
  UCSR1C=0x86;    // Mode 9 bits
  //sbi(UCSR1B,7);  // Autorisation de l'Interruption Receiver Enable
}

int main(void)
{
  init();
  // Ports, Directions, INTs, Timers, UARTs
  sleep_mode();
  sbi(SREG,7);
  // Sirene : 1 seconde
  Sirene_ON(1);

  while(1)
    {
	  sleep();
    }
  return 0;
}
