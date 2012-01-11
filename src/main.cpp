// Ported to SdFat from the native Arduino SD library example by Bill Greiman
// On the Ethernet Shield, CS is pin 4. SdFat handles setting SS
const int chipSelect = 10;
/*
 SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

 created   Nov 2010
 by David A. Mellis
 updated 2 Dec 2010
 by Tom Igoe
 modified by Bill Greiman 11 Apr 2011
 This example code is in the public domain.

 */

#include <WProgram.h>

#include <SdFat.h>
#include <Time.h>
#include <Wire.h>
#include <nokia_3310_lcd.h>
#include <DS1307RTC.h>
#include "TimeHandling.h"
#include "global.h"
#include "pitches.h"

SdFat sd;
SdFile myFile;


int melody[] = {
  NOTE_D4, NOTE_E4,NOTE_F4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4,NOTE_A4, NOTE_F4, NOTE_G4, NOTE_A4,
    NOTE_A4, NOTE_B4,NOTE_A4, NOTE_G4, NOTE_F4, NOTE_D4,NOTE_A4, NOTE_B4,NOTE_A4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_D4, NOTE_A3, NOTE_D4, NOTE_D4,NOTE_A3, NOTE_D4};


// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4,4,4,4,4,4,4,4,4,4,2,4,4,2,
    8,8,8,8,4,4,8,8,8,8,4,4,4,4,2,4,4,2};


void setup()
{
    //Set up the pins for the Serial communication
    pinMode(0, INPUT);
    pinMode(1, OUTPUT);
    Serial.begin(9600);
    Serial.print("hei");


    g_lcd.init();
    g_lcd.clear();

    g_lcd.writeStringBig( 0, 0, "123456", MENU_NORMAL );
    g_lcd.writeStringBig( 0, 0, "123456", MENU_NORMAL );
    g_lcd.writeStringBig( 0, 3, "7890+-.", MENU_NORMAL );
//
//    // iterate over the notes of the melody:
//      for (int thisNote = 0; thisNote < 33; thisNote++) {
//
//        // to calculate the note duration, take one second
//        // divided by the note type.
//        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//        int noteDuration = 1000/noteDurations[thisNote];
//        tone(A0, melody[thisNote],noteDuration);
//
//        // to distinguish the notes, set a minimum time between them.
//        // the note's duration + 30% seems to work well:
//        int pauseBetweenNotes = noteDuration * 1.30;
//        delay(pauseBetweenNotes);
//        // stop the tone playing:
//        noTone(A0);
//      }

    //Set up the pins for the microSD shield
    tmElements_t tm;
    tm.Day=18;
    tm.Month=12;
    tm.Year=CalendarYrToTm(2011);
    tm.Hour=22;
    tm.Minute=25;
    tm.Second=0;
    time_t t = makeTime(tm);
    //RTC.set(t);

    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus()!= timeSet)
    	Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");


    // Initialize SdFat or print a detailed error message and halt
    // Use half speed like the native library.
    // change to SPI_FULL_SPEED for more performance.
    if (!sd.init(SPI_HALF_SPEED, chipSelect)) sd.initErrorHalt();

    // open the file for write at end like the Native SD library
    if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
      sd.errorHalt("opening test.txt for write failed");
    }
    // if the file opened okay, write to it:
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");

    // close the file:
    myFile.close();
    Serial.println("done.");

    // re-open the file for reading:
    if (!myFile.open("test.txt", O_READ)) {
      sd.errorHalt("opening test.txt for read failed");
    }
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    int data;
    while ((data = myFile.read()) > 0) Serial.write(data);
    // close the file:
    myFile.close();




}

void loop()
{
	digitalClockDisplay();
	delay(1000);
}






//int main(void)
//{
//	//init();
//
//	setup();
//
//	for (;;)
//		loop();
//
//	return 0;
//}

