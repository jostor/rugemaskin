#ifndef GLOBAL_H // header guards
#define GLOBAL_H

#include <nokia_3310_lcd.h>
#include <Fat16.h>

extern SdCard card;
extern Fat16 file;
// extern tells the compiler this variable is declared elsewhere
extern Nokia_3310_lcd g_lcd;

extern unsigned char g_backLight;
extern int g_temperature;

#endif
