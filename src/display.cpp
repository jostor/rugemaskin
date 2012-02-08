#include <WProgram.h>
#include <nokia_3310_lcd.h>
#include "display.h"
#include "global.h"

// Temp/humidity display using nokia 3310 LCD display shield from nuelectronics.com

//#include "Andrew.h"
//#include "pacman.h"

//keypad debounce parameter
#define DEBOUNCE_MAX 15
#define DEBOUNCE_ON  10
#define DEBOUNCE_OFF 3

#define NUM_KEYS 5

#define NUM_MENU_ITEM	5

// joystick number
#define UP_KEY 1
#define LEFT_KEY 0
#define CENTER_KEY 2
#define DOWN_KEY 4
#define RIGHT_KEY 3

// Pin used by Backlight, so we can turn it on and off. Pin setup in LCD init function
#define BL_PIN 7

// menu starting points
#define MENU_X	10		// 0-83
#define MENU_Y	0		// 0-5

// adc preset value, represent top value,incl. noise & margin,that the adc reads, when a key is pressed
// set noise & margin = 30 (0.15V@5V)
int  adc_key_val[5] ={
  30, 150, 360, 535, 760 };

// debounce counters
byte button_count[NUM_KEYS];
// button status - pressed/released
byte button_status[NUM_KEYS];
// button on flags for user program
byte button_flag[NUM_KEYS];

// menu definition
char menu_items[NUM_MENU_ITEM][12]={
  "TEMPERATURE",
  "HUMIDITY",
  "BACKLIGHT",
  "DEMO",
  "ABOUT"
};

void (*menu_funcs[NUM_MENU_ITEM])(void) = {
  temperature,
  humidity,
  backlight,
  about
};

char current_menu_item;
int blflag = 1;  // Backlight initially ON
char degree = 0x7b;  // Degree symbol


//Nokia_3310_lcd lcd=Nokia_3310_lcd();

void displayInit()
{
  // setup interrupt-driven keypad arrays
  // reset button arrays
  for(byte i=0; i<NUM_KEYS; i++){
    button_count[i]=0;
    button_status[i]=0;
    button_flag[i]=0;
  }

  // Setup timer2 -- Prescaler/256
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);
  TCCR2B = (1<<CS22)|(1<<CS21);

  ASSR |=(0<<AS2);

  // Use normal mode
  TCCR2A =0;
  //Timer2 Overflow Interrupt Enable
  TIMSK2 |= (0<<OCIE2A);
  TCNT2=0x6;  // counting starts from 6;
  TIMSK2 = (1<<TOIE2);

  SREG|=1<<SREG_I;

  g_lcd.init();
  g_lcd.clear();

  //menu initialization
  init_MENU();
  current_menu_item = 0;
}


/* loop */
void displayLoop() {
  byte i;
  for(i=0; i<NUM_KEYS; i++) {
    if(button_flag[i] !=0) {

      button_flag[i]=0;  // reset button flag
      switch(i){
      case UP_KEY:
        // current item to normal display
        g_lcd.writeString(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_NORMAL );
        current_menu_item -=1;
        if(current_menu_item <0)  current_menu_item = NUM_MENU_ITEM -1;
        // next item to highlight display
        g_lcd.writeString(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_HIGHLIGHT );
        break;
      case DOWN_KEY:
        // current item to normal display
        g_lcd.writeString(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_NORMAL );
        current_menu_item +=1;
        if(current_menu_item >(NUM_MENU_ITEM-1))  current_menu_item = 0;
        // next item to highlight display
        g_lcd.writeString(MENU_X, MENU_Y + current_menu_item, menu_items[current_menu_item], MENU_HIGHLIGHT );
        break;
      case LEFT_KEY:
        init_MENU();
        current_menu_item = 0;
        break;
      case RIGHT_KEY:
        g_lcd.clear();
        (*menu_funcs[current_menu_item])();
        g_lcd.clear();
        init_MENU();
        current_menu_item = 0;
        break;
      }
    }
  }
}

/* menu functions */
void init_MENU(void) {

  byte i;
  g_lcd.clear();
  g_lcd.writeString(MENU_X, MENU_Y, menu_items[0], MENU_HIGHLIGHT );

  for (i=1; i<NUM_MENU_ITEM; i++) {
    g_lcd.writeString(MENU_X, MENU_Y+i, menu_items[i], MENU_NORMAL);
  }
}

// waiting for center key press
void waitfor_OKkey() {
  byte i;
  byte key = 0xFF;
  while (key!= CENTER_KEY){
    for(i=0; i<NUM_KEYS; i++){
      if(button_flag[i] !=0){
        button_flag[i]=0;  // reset button flag
        if(i== CENTER_KEY) key=CENTER_KEY;
      }
    }
  }
}

// Check if joystick is moved or pressed
byte checkKeypressed() {
  byte key = 0xFF;

  for(int i=0; i<NUM_KEYS; i++){
    if(button_flag[i] !=0){
      button_flag[i]=0;  // reset button flag
      if(i== CENTER_KEY) key=CENTER_KEY;
    }
  }
  return key;
}

char numStr[8];

// Format a number to 2 decimal places
void formatNum( int num ) {
  // First part before decimalpoint
  itoa( num / 100, numStr, 10 );
  int pos = strlen( numStr );
  numStr[pos++] = '.';
  int decimal = num % 100;
  if( decimal > 9 ) {
    itoa( decimal, &numStr[pos], 10 );
  } else {
    numStr[pos++] = '0';
    itoa( decimal, &numStr[pos], 10 );
  }
}

// Display temperature in big digits, humidity in small digits underneath
void temperature() {
  int temp, humid;

  // Display non changing text, there is a slight delay while first reading is taken
  g_lcd.gotoXY( 62,1 );
  g_lcd.print( degree );
  g_lcd.print( "C" );
//  lcd.writeString(67, 1, "C", MENU_NORMAL);
  g_lcd.writeString(38, 5, "OK", MENU_HIGHLIGHT );
  g_lcd.writeString(40, 3, "%RH", MENU_NORMAL);

  long lastUpdate = 0;  // Force update
  byte i;
  byte key = 0xFF;

  // Loop to display temperaure/humidity with check for key press to exit
  while (key!= CENTER_KEY) {
    // Update temp
    if( millis() > lastUpdate + 1000) {
      // Read temperature and humidity
//      temp = (int)(sht1x.readTemperatureC() * 100);
//      humid = (int)(sht1x.readHumidity() * 100);

      // Display temp
      formatNum( g_currentTemperature );
      g_lcd.writeStringBig(8, 0, numStr, MENU_NORMAL);

      // Display Humidity
      formatNum( humid );
      g_lcd.writeString(10, 3, numStr, MENU_NORMAL);

      lastUpdate = millis();
    }
    key = checkKeypressed();
  }
}

// Display humidity in big digits, temperature in small digits underneath
void humidity() {
  int temp, humid;

  // Display non changing text
  g_lcd.writeString(66, 1, "%RH", MENU_NORMAL);
  g_lcd.gotoXY( 40,3 );
  g_lcd.print( degree );
  g_lcd.print( "C" );
//  g_lcd.writeString(40, 3, "C", MENU_NORMAL);
  g_lcd.writeString(38, 5, "OK", MENU_HIGHLIGHT );

  long lastUpdate = 0;  // Force update
  byte i;
  byte key = 0xFF;
  // Loop to display temperaure/humidity with check for key press to exit

  while (key!= CENTER_KEY){
    // Update temp
    if( millis() > lastUpdate + 1000) {
      // Read temperature and humidity
//      temp = (int)(sht1x.readTemperatureC() * 100);
//      humid = (int)(sht1x.readHumidity() * 100);

      // Display humidity
      formatNum( humid );
      g_lcd.writeStringBig(8, 0, numStr, MENU_NORMAL);

      // Display temperature
      formatNum( temp );
      g_lcd.writeString(10, 3, numStr, MENU_NORMAL);

      lastUpdate = millis();
    }
    key = checkKeypressed();
  }
}

// Display the about information
void about(){

  g_lcd.gotoXY( 0, 1 );
  g_lcd.print("Temp/Humidity" );
  g_lcd.gotoXY( 0, 2 );
  g_lcd.print( "Nokia 3110 LCD" );
  g_lcd.gotoXY( 0, 3 );
  g_lcd.print( "A D Lindsay" );
  g_lcd.writeString(38, 5, "OK", MENU_HIGHLIGHT );
  waitfor_OKkey();
}

// Display the simple graphics demo

// Allow backlight to be turned on and off
void backlight() {

  g_lcd.writeString( 0, 1, "Toggle", MENU_NORMAL);
  g_lcd.writeString( 0, 2, "Backlight", MENU_NORMAL);
  if( g_backLight != 0 ) {
    g_lcd.writeString( 60, 2, "Off", MENU_HIGHLIGHT);
  }
  else {
    g_lcd.writeString( 60, 2, "On", MENU_HIGHLIGHT);
  }

  waitfor_OKkey();

  if( g_backLight != 0 ) {
    g_backLight=0;
    digitalWrite( BL_PIN, LOW );
  }   else {
    g_backLight = 1;
    digitalWrite( BL_PIN, HIGH );
  }
  macro_eeprom_write(g_backLight,eeprom_backlight);
}

// The followinging are interrupt-driven keypad reading functions
//  which includes DEBOUNCE ON/OFF mechanism, and continuous pressing detection


// Convert ADC value to key number
char get_key(unsigned int input) {
  char k;

  for (k = 0; k < NUM_KEYS; k++) {
    if (input < adc_key_val[k]) {
      return k;
    }
  }

  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed

  return k;
}

void update_adc_key() {
  int adc_key_in;
  char key_in;
  byte i;

  adc_key_in = analogRead(0);
  key_in = get_key(adc_key_in);
  for(i=0; i<NUM_KEYS; i++) {
    if(key_in==i) { //one key is pressed
      if(button_count[i]<DEBOUNCE_MAX)       {
        button_count[i]++;
        if(button_count[i]>DEBOUNCE_ON)         {
          if(button_status[i] == 0)           {
            button_flag[i] = 1;
            button_status[i] = 1; //button debounced to 'pressed' status
          }
        }
      }
    } else  { // no button pressed
      if (button_count[i] >0) {
        button_flag[i] = 0;
        button_count[i]--;
        if(button_count[i]<DEBOUNCE_OFF) {
          button_status[i]=0;   //button debounced to 'released' status
        }
      }
    }
  }
}

// Timer2 interrupt routine -
// 1/(160000000/256/(256-6)) = 4ms interval

ISR(TIMER2_OVF_vect) {
  TCNT2  = 6;
  update_adc_key();
}

