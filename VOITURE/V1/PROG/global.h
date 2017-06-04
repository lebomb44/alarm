/****************************************************************************
 Titre	:   Parametres globaux
 Auteur:    LeBomb
 Date:      02/09/2004
 Software:  WinAVR
 Cible:     Microcontroleur Atmel AT90S4414/8515/Mega

 DESCRIPTION
       Definit et declare tous les parametres globaux relatifs a l'application
 UTILISATION
       Definir la valeur du QUARTZ
	   Choix du microcontroleur utilise:
		   librairy <avr/iomXX.h>
		   dans le makefile MCU = atmegaXX
*****************************************************************************/

#ifndef _GLOBAL_
#define _GLOBAL_

typedef unsigned char  u08;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef signed char  s08;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

#define QUARTZ 8000 // frequence du quartz en KHz, sert pour les fonctions d'attente

#define GCC33

// includes generaux
#include <avr/io.h>
#include <avr/iom128.h>  // definition du processeur
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <stdlib.h>

#define extr16_08_0(i) (*(char *)(&i))
#define extr16_08_1(i) (*((char *)(&i)+1))

#define extr32_16_0(i) (*(int *)(&i)) // poids faible
#define extr32_16_1(i) (* (((int *)(&i)) +1)) //poids fort
#define extr32_16_23(i) (*((int *)((char *)(&i)+1))) // les 2 octets du milieu

#define extr32_08_0(i) (*(char *)(&i))
#define extr32_08_1(i) (*((char *)(&i)+1))
#define extr32_08_2(i) (*((char *)(&i)+2))
#define extr32_08_3(i) (*((char *)(&i)+3))

# define sbi(reg,bit) { reg = reg |   (1<<bit) ; }
# define cbi(reg,bit) { reg = reg & (~(1<<bit)); }
#define nop() __asm__ __volatile__ ("NOP");
#define rien() __asm__ __volatile__ (" ");
#define sleep() __asm__ __volatile__ ("SLEEP");

/* Utilisation des ports
PORTA : buzzer et contacts

PORTB : SPI, sirènes et US

PORTC : ampli

PORTD : GSM et entrees

PORTE : emetteur, leds et US

PORTF : CA/N

PORTG : enable GPS/GSM

*/

#define VAL_DDRA 0x82
#define VAL_DDRB 0xF0
#define VAL_DDRC 0x44
#define VAL_DDRD 0x00
#define VAL_DDRE 0xCE
#define VAL_DDRF 0x00
#define VAL_DDRG 0x03

// LED1 : UART (orange 2)
#define LED1PORT PORTE
#define LED1PIN PINE
#define LED1BIT 2

#define LED1ON() {cbi(LED1PORT,LED1BIT);}
#define LED1OFF() {sbi(LED1PORT,LED1BIT);}
#define LED1TOGGLE() {if(bit_is_set(LED1PIN,LED1BIT)) cbi(LED1PORT,LED1BIT); else sbi(LED1PORT,LED1BIT);}

// LED2 : Commandes (verte 1)
#define LED2PORT PORTE
#define LED2PIN PINE
#define LED2BIT 3

#define LED2ON() {cbi(LED2PORT,LED2BIT);}
#define LED2OFF() {sbi(LED2PORT,LED2BIT);}
#define LED2TOGGLE() {if(bit_is_set(LED2PIN,LED2BIT)) cbi(LED2PORT,LED2BIT); else sbi(LED2PORT,LED2BIT);}

// LED3 : Sirene (rouge 4)
#define LED3PORT PORTE
#define LED3PIN PINE
#define LED3BIT 6

#define LED3ON() {cbi(LED3PORT,LED3BIT);}
#define LED3OFF() {sbi(LED3PORT,LED3BIT);}
#define LED3TOGGLE() {if(bit_is_set(LED3PIN,LED3BIT)) cbi(LED3PORT,LED3BIT); else sbi(LED3PORT,LED3BIT);}

// LED4 : Contact & Capteur (rouge 3)
#define LED4PORT PORTE
#define LED4PIN PINE
#define LED4BIT 7

#define LED4ON() {cbi(LED4PORT,LED4BIT);}
#define LED4OFF() {sbi(LED4PORT,LED4BIT);}
#define LED4TOGGLE() {if(bit_is_set(LED4PIN,LED4BIT)) cbi(LED4PORT,LED4BIT); else sbi(LED4PORT,LED4BIT);}

// BUZZER
#define BUZZERPORT PORTA
#define BUZZERPIN PINA
#define BUZZERBIT 1

#define BUZZERON() {BUZZERPORT=(0x02 | (BUZZERPORT & 0x7F));}
#define BUZZEROFF() {BUZZERPORT=(0x80 | (BUZZERPORT & 0xFD));}
#define BUZZERTOGGLE() {if(bit_is_set(BUZZERPIN,BUZZERBIT)) {BUZZEROFF();} else {BUZZERON();}}

// SIGNAL
#define RX0PORT PORTE
#define RX0PIN PINE
#define RX0BIT 0
#define TX0PORT PORTE
#define TX0PIN PINE
#define TX0BIT 1
#define VCC_ampli_PORT PORTC
#define VCC_ampli_PIN PINC
#define VCC_ampli_BIT 2
#define TX_enable_PORT PORTC
#define TX_enable_PIN PINC
#define TX_enable_BIT 6
#define NBRE_EMISSIONS 10

// US
#define USEMPORT PORTB
#define USEMPIN PINB
#define USEMBIT 5
#define USEMON() {USEMPORT=(0x60 | (USEMPORT & 0x9F));}
#define USEMOFF() {USEMPORT=(USEMPORT & 0x9F);}
//#define USEMTOGGLE() {USEMPORT= ((USEMPORT & 0x9F) | ((USEMPORT & 0x60) ^ 0x60));}
#define USEMTOGGLE() {if(bit_is_set(USEMPIN,5)) {cbi(USEMPORT,5); sbi(USEMPORT,6);} else {sbi(USEMPORT,5); cbi(USEMPORT,6);}}

#define USREPORT PORTE
#define USREPIN PINE
#define USREBIT 5

#define US_TPS_H 1000
#define US_TPS_L 133
#define US_FREQ_H 178
#define US_FREQ_L 0
#define DISTANCE_US_MIN 0x0000
#define DISTANCE_US_MAX 0xFFFF
#define NBRE_MESURES_US_MAX 10
#define NBRE_US_ALERTE_MAX 10
#define NB_BIPS_ALERTE_US 1000
#define ARRET_US {TCCR0=0x00;TCCR1B=0x00;TCNT0=0x00;TCNT1=0x0000;EIMSK=EIMSK&0xDF;EIFR=0x20;}

// Sirene
#define SIRENEPORT PORTB
#define SIRENEPIN PINB
#define SIRENE0BIT 7
#define SIRENE1BIT 4
#define SIRENE_TPS_H 500
#define SIRENE_TPS_L 533
#define SIRENE_FREQ_H 240
#define SIRENE_FREQ_L 0
#define SIRENE_ON {TCCR2=0x05;}   // CLK/1024
#define SIRENE_OFF {TCCR2=0x00;}


// Contacts
#define CAPTEURPORT PORTD
#define CAPTEURPIN PIND
#define CAPTEURBIT0 4
#define CAPTEURBIT1 5
#define CAPTEURBIT2 6
#define CAPTEURBIT3 7
#define CHOCPORT PORTE
#define CHOCPIN PINE
#define CHOCBIT 4
#define CONTACTPORT PORTA
#define CONTACTPIN PINA
#define CONTACTBIT0 3
#define CONTACTBIT1 4
#define CONTACTBIT2 5
#define NB_BIPS_ALERTE_CHOC 1000
#define NB_BIPS_ALERTE_CONTACTS 1000
#define SCRUTATION_ON {TCCR3B=0x02;}
#define SCRUTATION_OFF {TCCR3B=0x00;}

// GSM & GPS
#define GSMGPSPORT PORTG
#define GSMGPSPIN PING
#define GPSBIT 0
#define GSMBIT 1
#define GPS_enable 0xFE
#define GSM_enable 0xFD
#define GSMGPS_disable 0xFF
#define RX1PORT PORTD
#define RX1PIN PIND
#define RX1BIT 2

// BATTERIE
#define BATTERIEPORT PORTF
#define BATTERIEPIN PINF
#define BATTERIEBIT0 0
#define BATTERIEBIT1 1
#define NIVEAU_BATTERIE_FAIBLE 150

volatile u08 adr_device;
volatile u08 adr_telecommande;
volatile u08 mode_alarme;
volatile u08 commande_alerte;
volatile u08 commande_on_avec_US;
volatile u08 commande_on_sans_US;
volatile u08 commande_off;
volatile u08 commande_etat;
volatile u08 commande_batt_voiture;
volatile u08 commande_alerte_choc;
volatile u08 commande_alerte_US;
volatile u08 commande_alerte_batterie;
volatile u08 niveau_batterie;
volatile u08 coordonnees_gps[21];
volatile u08 etat_contact;
volatile u08 etat_capteur;
volatile u16 distance_US_min;
volatile u16 distance_US_max;
volatile u16 nbre_mesures_US;
volatile u32 mesures_US;
volatile u08 nbre_US_alerte;
volatile u16 nbre_bips_sirene;
volatile u16 nbre_bips_us;

#endif
