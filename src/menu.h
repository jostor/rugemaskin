#ifndef MENU_H // header guards
#define MENU_H

#include <avr/pgmspace.h>
#include <FiniteStateMachine.h>


void printMainMenu(void);
void printChicken(void);
void printDateMenu(void);
void printTimeMenu(void);
void showString (PGM_P s);
void menuCheckInput(void);
void menuNewTemp(void);
void menuEditTime(void);
void menuEditDate(void);
void clearAndHome(void);
void printTempMenu(void);

extern FSM menuStateMachine;


#endif
