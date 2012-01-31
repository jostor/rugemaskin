#ifndef MENU_H // header guards
#define MENU_H

#include <avr/pgmspace.h>
#include <FiniteStateMachine.h>


void menuMainMenu(void);
void showString (PGM_P s);
void menuCheckInput(void);
void menuNewTemp(void);
void clearAndHome(void);
void printTempMenu(void);

extern FSM menuStateMachine;


#endif
