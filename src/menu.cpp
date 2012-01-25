#include "menu.h"
#include "global.h"
#include <WProgram.h>
#include <avr/pgmspace.h>
#include <Fat16.h>
#include <Fat16util.h> // use functions to print strings from flash memory

// function clearAndHome()
// clear the terminal screen and send the cursor home
void clearAndHome()
{
	Serial.print(27, BYTE); // ESC
	Serial.print("[2J"); // clear screen
	Serial.print(27, BYTE); // ESC
	Serial.print("[H"); // cursor to home
}

void menuMainMenu(void)
{
	clearAndHome();
	showString (PSTR("\n\r             .-=-."));
	showString (PSTR("\n\r           .'     \\_"));
	showString (PSTR("\n\r        __.|    9 )_\\"));
	showString (PSTR("\n\r   _.-''          /"));
	showString (PSTR("\n\r<`'     ..._    <'"));
	showString (PSTR("\n\r `._ .-'    `.  |"));
	showString (PSTR("\n\r  ; `.    .-'  /"));
	showString (PSTR("\n\r   \\  `~~'  _.'"));
	showString (PSTR("\n\r    `\"...\"'% _"));
	showString (PSTR("\n\r      \\__  |`.   "));
	showString (PSTR("\n\r      /`."));

	showString (PSTR("\n\r1    Edit time"));
	showString (PSTR("\n\r2    Toggle backlight"));
	showString (PSTR("\n\r3    Print temperature log"));
	showString (PSTR("\n\r4    Set temperature"));
	showString (PSTR("\n\rm    Main menu"));

}

void showString (PGM_P s)
{
	char c;
	while ((c = pgm_read_byte(s++)) != 0)
		Serial.print(c);
}

void menuCheckInput(void)
{
	unsigned char c = Serial.read();
	switch (c)
	{
	case '1':
	{
		showString (PSTR("\n\rHei"));
		break;
	}
	case '2':
	{
		g_backLight ^= 1;
		digitalWrite(7,g_backLight);
		menuMainMenu();
		showString (PSTR("\n\rBacklight is "));
		if (g_backLight)
			showString (PSTR("on"));
		else
			showString (PSTR("off"));
		break;
	}
	case '3':
	{
		// open a file
		  if (file.open("TMPLOG.TXT", O_READ)) {
		    PgmPrintln("Opened TMPLOG.TXT");
		  }
		  Serial.println();

		  // copy file to serial port
		  int16_t n;
		  uint8_t buf[7];// nothing special about 7, just a lucky number.
		  while ((n = file.read(buf, sizeof(buf))) > 0) {
		    for (uint8_t i = 0; i < n; i++) Serial.write(buf[i]);
		  }
		  /* easier way
		  int16_t c;
		  while ((c = file.read()) > 0) Serial.write((char)c);
		  */
		  PgmPrintln("\n\rDone");
		break;
	}
	case '4':
		{
			showString (PSTR("\n\rPlease enter new temperature (xx.xx): "));
			char * c;
			int i = 0;
			while (i<=5)
			{
				if (i <= 5)
				{
					c[i] = Serial.read();
					Serial.print(c[i]);
					i++;
				}
			}
			c[i]='\0';

			g_temperature = atoi(c);
			Serial.println(g_temperature);
			//EEPROM.write(eeprom.temperature,g_temperature);
			break;
		}

	case 'm':
	{
		menuMainMenu();
		break;
	}
	}
}
