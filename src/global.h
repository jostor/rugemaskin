#ifndef GLOBAL_H // header guards
#define GLOBAL_H

#include <nokia_3310_lcd.h>
#include <Fat16.h>
#include <avr/eeprom.h>
#define macro_eeprom_read_to(dst_p, eeprom_field, dst_size) eeprom_read_block(dst_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(dst_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define macro_eeprom_read(dst, eeprom_field) macro_eeprom_read_to(&dst, eeprom_field, sizeof(dst))
#define macro_eeprom_write_from(src_p, eeprom_field, src_size) eeprom_write_block(src_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(src_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define macro_eeprom_write(src, eeprom_field) { typeof(src) x = src; macro_eeprom_write_from(&x, eeprom_field, sizeof(x)); }
#define MIN(x,y) ( x > y ? y : x )

struct __eeprom_data
{
	float temperature;
	unsigned char temperature_l;
	unsigned char temperature_h;
};

extern SdCard card;
extern Fat16 file;
// extern tells the compiler this variable is declared elsewhere
extern Nokia_3310_lcd g_lcd;

extern unsigned char g_backLight;
extern float g_temperature;
extern float g_currentTemperature;


#endif
