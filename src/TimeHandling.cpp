#include <Time.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include "TimeHandling.h"
#include "global.h"

void asciiToTime(tmElements_t * time, char s[])
{
	time->Day=day();
	time->Month=month();
	time->Year=year() - 1970;
	time->Second=((s[5]-'0') + (10*(s[4]-'0')));
	time->Minute=((s[3]-'0') + (10*(s[2]-'0')));
	time->Hour=((s[1]-'0') + (10*(s[0]-'0')));
}
void asciiToDate(tmElements_t * time, char s[])
{
	time->Day=((s[1]-'0') + (10*(s[0]-'0')));
	time->Month=((s[3]-'0') + (10*(s[2]-'0')));
	time->Year=((s[7]-'0') + (10*(s[6]-'0')) + (100*(s[5]-'0')) + (1000*(s[4]-'0'))) - 1970;
	time->Second=second();
	time->Minute=minute();
	time->Hour=hour();
}


void printDate()
{
	printDateDigits(day());
	Serial.print(".");
	printDateDigits(month());
	Serial.print(".");
	printDateDigits(year());
}

void printTime()
{
	Serial.print(hour());
	printDigits(minute());
	printDigits(second());
	Serial.println();
}


void digitalClockDisplay(){
	// digital clock display of the time
	Serial.print(hour());
	printDigits(minute());
	printDigits(second());
	Serial.print(" ");
	Serial.print(day());
	Serial.print(" ");
	Serial.print(month());
	Serial.print(" ");
	Serial.print(year());
	Serial.println();
	g_lcd.clear();
	g_lcd.print(hour());
	g_lcd.print(minute());
	g_lcd.print(second());
}

void printDigits(int digits){
	// utility function for digital clock display: prints preceding colon and leading 0
	Serial.print(":");
	if(digits < 10)
		Serial.print('0');
	Serial.print(digits);
}
void printDateDigits(int digits){
	if(digits < 10)
		Serial.print('0');
	Serial.print(digits);
}

/*  code to process time sync messages from the serial port   */
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message

time_t processSyncMessage() {
	// return the time if a valid sync message is received on the serial port.
	while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of a header and ten ascii digits
		char c = Serial.read() ;
		Serial.print(c);
		if( c == TIME_HEADER ) {
			time_t pctime = 0;
			for(int i=0; i < TIME_MSG_LEN -1; i++){
				c = Serial.read();
				if( c >= '0' && c <= '9'){
					pctime = (10 * pctime) + (c - '0') ; // convert digits to a number
				}
			}
			return pctime;
		}
	}
	return 0;
}

