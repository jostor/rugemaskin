#include <nokia_3310_lcd.h>
#include <Fat16.h>
#include "global.h"

// declaration of g_lcd
Nokia_3310_lcd g_lcd=Nokia_3310_lcd();
SdCard card;
Fat16 file;
unsigned char g_backLight = 1;
float g_temperature;
float g_currentTemperature;
