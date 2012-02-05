#ifndef TimeHandling_h
#define TimeHandling_h

#include <WProgram.h>
#include <Time.h>


void printDigits(int digits);
void printDateDigits(int digits);
void printDate();
void printTime();
void digitalClockDisplay();
time_t processSyncMessage();
void asciiToTime(tmElements_t * time, char s[]);
void asciiToDate(tmElements_t * time, char s[]);


#endif
