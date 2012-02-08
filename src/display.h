#ifndef DISPLAY_H // header guards
#define DISPLAY_H


void displayInit();
void displayLoop();
void init_MENU(void);
void waitfor_OKkey();
byte checkKeypressed();
void formatNum( int num );
void temperature();
void humidity();
void about();
void backlight();
char get_key(unsigned int input);
void update_adc_key();

#endif
