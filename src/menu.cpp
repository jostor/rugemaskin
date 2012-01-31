#include "menu.h"
#include "global.h"
#include <WProgram.h>
#include <avr/pgmspace.h>
#include <Fat16.h>
#include <Fat16util.h> // use functions to print strings from flash memory
#include <FiniteStateMachine.h>
//#include <EEPROM.h>

const byte NUMBER_OF_STATES = 2;

State Main = State(menuMainMenu,menuCheckInput,clearAndHome);
State newTemp = State(printTempMenu,menuNewTemp,clearAndHome);

FSM menuStateMachine = FSM(Main);


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
	showString (PSTR("\n\r5    View temperature"));
	showString (PSTR("\n\rm    Main menu"));

}
void printTempMenu(void)
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


	showString (PSTR("\n\rPlease enter new temperature (xx.xx): "));

}

void showString (PGM_P s)
{
	char c;
	while ((c = pgm_read_byte(s++)) != 0)
		Serial.print(c);
}

void menuNewTemp(void)
{
	static int i = 1;
	static float temp = 0;
	int temp16;
	unsigned char temp_h;
	unsigned char temp_l;

	if (Serial.available())
	{
		unsigned char c = Serial.read();
		char string[]="";
		switch (c)
		{
			case '\r':
				i=1;
				g_temperature = temp;
				temp16 = temp*100;
				Serial.println(temp16);
				temp_l = temp16 & 0xff;
				//sprintf(string,"temp16: %x",temp16);
				//Serial.println(string);
				//sprintf(string,"temp_l: %x",temp_l);
				//Serial.println(string);
				temp_h = (temp16 >> 8) & 0xff;

				macro_eeprom_write(g_temperature,temperature);
				//EEPROM.write(eeprom.temperature_l,(temp_l));
				//EEPROM.write(eeprom.temperature_h,(temp_h));
				//showString (PSTR("\n\rNew temperature is set to: "));
				//Serial.print(temp);
				temp=0;
				menuStateMachine.transitionTo(Main);
				break;
			case '.':
				Serial.write(c);
				break;
			default:
				temp = temp + ((c-'0')*(pow(10,i)));
				Serial.write(c);
				i--;
				break;
		}
	}
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
			menuStateMachine.transitionTo(newTemp);
			break;
		}
	case '5':
		{
			showString (PSTR("\n\rCurrent temperature: "));
			Serial.print(g_currentTemperature);
			showString (PSTR("\n\rRequested temperature: "));
			Serial.print(g_temperature);
			break;
		}

	case 'm':
	{
		menuMainMenu();
		break;
	}
	}
}
