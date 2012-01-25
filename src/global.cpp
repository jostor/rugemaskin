#include <nokia_3310_lcd.h>
#include <Fat16.h>

// declaration of g_lcd
Nokia_3310_lcd g_lcd=Nokia_3310_lcd();
SdCard card;
Fat16 file;
unsigned char g_backLight = 1;
int g_temperature;
