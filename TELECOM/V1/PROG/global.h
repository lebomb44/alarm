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

#define QUARTZ 4000 // frequence du quartz en KHz, sert pour les fonctions d'attente

#define GCC33

// includes generaux
#include <avr/io.h>
#include <avr/iom8.h>  // definition du processeur
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

#define sbi(reg,bit) { reg = reg |   (1<<bit) ; }
#define cbi(reg,bit) { reg = reg & (~(1<<bit)); }
#define nop() __asm__ __volatile__ ("NOP");
#define rien() __asm__ __volatile__ (" ");
#define sleep() __asm__ __volatile__ ("SLEEP");

/* Utilisation des ports
PORTA : 

PORTB : 

PORTC :

PORTD :

*/

#define VAL_DDRB 0x00
#define VAL_DDRC 0x3C
#define VAL_DDRD 0xF2

// LED1
#define LED1PORT PORTD
#define LED1PIN PIND
#define LED1BIT 7

#define LED1ON() {sbi(LED1PORT,LED1BIT);}
#define LED1OFF() {cbi(LED1PORT,LED1BIT);}
#define LED1TOGGLE() {if(bit_is_set(LED1PIN,LED1BIT)) cbi(LED1PORT,LED1BIT); else sbi(LED1PORT,LED1BIT);}

// LED2
#define LED2PORT PORTD
#define LED2PIN PIND
#define LED2BIT 6

#define LED2ON() {sbi(LED2PORT,LED2BIT);}
#define LED2OFF() {cbi(LED2PORT,LED2BIT);}
#define LED2TOGGLE() {if(bit_is_set(LED2PIN,LED2BIT)) cbi(LED2PORT,LED2BIT); else sbi(LED2PORT,LED2BIT);}

// LED3
#define LED3PORT PORTD
#define LED3PIN PIND
#define LED3BIT 5

#define LED3ON() {sbi(LED3PORT,LED3BIT);}
#define LED3OFF() {cbi(LED3PORT,LED3BIT);}
#define LED3TOGGLE() {if(bit_is_set(LED3PIN,LED3BIT)) cbi(LED3PORT,LED3BIT); else sbi(LED3PORT,LED3BIT);}

// LED4
#define LED4PORT PORTD
#define LED4PIN PIND
#define LED4BIT 4

#define LED4ON() {sbi(LED4PORT,LED4BIT);}
#define LED4OFF() {cbi(LED4PORT,LED4BIT);}
#define LED4TOGGLE() {if(bit_is_set(LED4PIN,LED4BIT)) cbi(LED4PORT,LED4BIT); else sbi(LED4PORT,LED4BIT);}

// UART
#define RXPORT PORTD
#define RXPIN PIND
#define RXBIT 0
#define TXPORT PORTD
#define TXPIN PIND
#define TXBIT 1
#define NBRE_EMISSIONS 400

// Buzzer
#define BUZZERPORT PORTC
#define BUZZERPIN PINC
#define BUZZERBIT 3
#define BUZZER_TPS_H 500
#define BUZZER_TPS_L 533
#define BUZZER_FREQ_H 240
#define BUZZER_FREQ_L 0
#define BUZZERON() {BUZZERPORT=(0x0C | (BUZZERPORT & 0xC3));}
#define BUZZEROFF() {BUZZERPORT=(0x30 | (BUZZERPORT & 0xC3));}
#define BUZZERTOGGLE() {if(bit_is_set(BUZZERPIN,BUZZERBIT)) {BUZZEROFF();} else {BUZZERON();}}

// Boutons
#define BOUTON_MAISON_PIN PINB
#define BOUTON_MAISON_BIT 1
#define BOUTON_ON_AVEC_US_PIN PINB
#define BOUTON_ON_AVEC_US_BIT 2
#define BOUTON_ON_SANS_US_PIN PIND
#define BOUTON_ON_SANS_US_BIT 3
#define BOUTON_OFF_PIN PIND
#define BOUTON_OFF_BIT 2

#define BOUTON_VIALA_PIN PINB
#define BOUTON_VIALA_BIT 1
#define BOUTON_ETAT_PIN PINB
#define BOUTON_ETAT_BIT 2
#define BOUTON_BATT_VOITURE_PIN PIND
#define BOUTON_BATT_VOITURE_BIT 3
#define BOUTON_BATT_TELE_PIN PIND
#define BOUTON_BATT_TELE_BIT 2

volatile u08 adr_device;
volatile u08 adr_voiture;
volatile u08 commande_on_avec_US;
volatile u08 commande_on_sans_US;
volatile u08 commande_off;
volatile u08 commande_etat;
volatile u08 commande_batt_voiture;
volatile u08 commande_alerte_choc;
volatile u08 commande_alerte_US;
volatile u08 commande_alerte_batterie;
volatile u16 nbre_bips_sirene;

#endif

// PINB1 LED1 MAISON	VIALA
// PINB2 LED2 AVEC_US	ETAT
// PIND3 LED3 SANS_US	BATT VOITURE
// PIND2 LED4 OFF		BATT TELE
