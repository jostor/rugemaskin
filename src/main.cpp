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

//#include <SdFat.h>
#include <Time.h>
#include <Wire.h>
#include <nokia_3310_lcd.h>
#include <DS1307RTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "TimeHandling.h"
#include "global.h"
#include "menu.h"

#include <Fat16.h>
#include <Fat16util.h> // use functions to print strings from flash memory

#include <EEPROM.h>

struct eepromStruct
{
	int temperature;
};

eepromStruct eeprom;




//SdFat sd;
//SdFile myFile;

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS A1

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


void error_P(const char* str) {
  PgmPrint("error: ");
  SerialPrintln_P(str);
  if (card.errorCode) {
    PgmPrint("SD error: ");
    Serial.println(card.errorCode, HEX);
  }
  while(1);
}

/*
 * Write an unsigned number to file
 */
void writeNumber(uint32_t n) {
  uint8_t buf[10];
  uint8_t i = 0;
  do {
    i++;
    buf[sizeof(buf) - i] = n%10 + '0';
    n /= 10;
  } while (n);
  file.write(&buf[sizeof(buf) - i], i); // write the part of buf with the number
}


void setup()
{
    //Set up the pins for the Serial communication
    pinMode(0, INPUT);
    pinMode(1, OUTPUT);
    Serial.begin(115200);
    Serial.print("hei");

    g_temperature = EEPROM.read(eeprom.temperature);

    digitalWrite(7,g_backLight);

    Serial.println(FreeRam());

    g_lcd.init();
    g_lcd.clear();

    g_lcd.writeStringBig( 0, 0, "123456", MENU_NORMAL );
    g_lcd.writeStringBig( 0, 0, "123456", MENU_NORMAL );
    g_lcd.writeStringBig( 0, 3, "7890+-.", MENU_NORMAL );

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
//    if(timeStatus()!= timeSet)
//    	Serial.println("Unable to sync with the RTC");
//    else
//        Serial.println("RTC has set the system time");


//    // Initialize SdFat or print a detailed error message and halt
//    // Use half speed like the native library.
//    // change to SPI_FULL_SPEED for more performance.
//    if (!sd.init(SPI_HALF_SPEED, chipSelect)) sd.initErrorHalt();
//
//    // open the file for write at end like the Native SD library
//    if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
//      sd.errorHalt("opening test.txt for write failed");
//    }
//    // if the file opened okay, write to it:
//    Serial.print("Writing to test.txt...");
//    myFile.println("testing 1, 2, 3.");
//
//    // close the file:
//    myFile.close();
//    Serial.println("done.");
//
//    // re-open the file for reading:
//    if (!myFile.open("test.txt", O_READ)) {
//      sd.errorHalt("opening test.txt for read failed");
//    }
//    Serial.println("test.txt:");
//
//    // read from the file until there's nothing else in it:
//    //int data;
////    while ((data = myFile.read()) > 0) Serial.write(data);
//    // close the file:
//    //myFile.close();


    // initialize the SD card
      if (!card.init()) error("card.init");

      // initialize a FAT16 volume
      if (!Fat16::init(&card)) error("Fat16::init");

      // create a new file
      char name[] = "WRITE00.TXT";
      for (uint8_t i = 0; i < 100; i++) {
        name[5] = i/10 + '0';
        name[6] = i%10 + '0';
        // O_CREAT - create the file if it does not exist
        // O_EXCL - fail if the file exists
        // O_WRITE - open for write
        if (file.open(name, O_CREAT | O_EXCL | O_WRITE)) break;
      }
      if (!file.isOpen()) error ("file.open");
      PgmPrint("Writing to: ");
      Serial.println(name);

      // write 100 line to file
      for (uint8_t i = 0; i < 100; i++) {
        file.write("line "); // write string from RAM
        writeNumber(i);
        file.write_P(PSTR(" millis = ")); // write string from flash
        writeNumber(millis());
        file.write("\r\n"); // file.println() would work also
      }
      // close file and force write of all data to the SD card
      file.close();
      PgmPrintln("Done");


    // Start up the temperature library
    sensors.begin();

    menuMainMenu();

}

void loop()
{
	sensors.requestTemperatures(); // Send the command to get temperatures
		  //Serial.println("DONE");

		  //Serial.print("Temperature for the device 1 (index 0) is: ");
		  //Serial.println(sensors.getTempCByIndex(0));
	// create a new file
	      char name[] = "TMPLOG.TXT";
	        // O_CREAT - create the file if it does not exist
	        // O_EXCL - fail if the file exists
	        // O_WRITE - open for write
	        file.open(name, O_CREAT | O_APPEND | O_WRITE);

	      if (!file.isOpen()) error ("file.open");
	        writeNumber(year());
	        file.write(".");
	        writeNumber(month());
	        file.write(".");
	        writeNumber(day());
	        file.write(". ");
	        writeNumber(hour());
	        file.write(":");
	        writeNumber(minute());
	        file.write(":");
	        writeNumber(second());
	        file.write(" ; ");
	        writeNumber(sensors.getTempCByIndex(0));
	        file.write("\r\n"); // file.println() would work also
	      // close file and force write of all data to the SD card
	      file.close();

	menuCheckInput();
	//Serial.println(FreeRam());
	//Serial.print("Requesting temperatures...");
	  sensors.requestTemperatures(); // Send the command to get temperatures
	  //Serial.println("DONE");

	  //Serial.print("Temperature for the device 1 (index 0) is: ");
	  //Serial.println(sensors.getTempCByIndex(0));

	  //digitalClockDisplay();
	//delay(1000);
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

