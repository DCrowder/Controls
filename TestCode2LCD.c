#include <PIC16f887.h>

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

#include <htc.h>

#include <pic.h> /* This header file contains symbolic register addresses
that may be used to perform input/output operations
and to access other resources on the PIC 16F877 microcontroller*/



__CONFIG(FOSC_INTRC_NOCLKOUT & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_OFF);
__CONFIG(BOR4V_BOR21V & WRT_OFF);

// LCD Ports
#define LCD_E   RC5
#define LCD_RW  RC6
#define LCD_RS  RC7
#define LCD_DAT PORTD

// LCD Directions
#define LCD_E_Dir   TRISC5
#define LCD_RW_Dir  TRISC6
#define LCD_RS_Dir  TRISC7
#define LCD_DAT_Dir TRISD


#define EDelay 1000 //304 cycles = 38 usec


void delay(unsigned int time)
{
    unsigned int i;

   for(i = 0; i < time; i++); //Loop

}

void ToggleEpinOfLCD(void)
{
	LCD_E = 1;                // Give a pulse on E pin
	delay(EDelay);            // so that LCD can latch the
	LCD_E = 0;                // data from data bus
	delay(EDelay);
}


void WriteCommandToLCD(unsigned char Command)
{
	LCD_RS = 0;               // It is a command
	LCD_DAT = Command;        // Write Command value on data bus

	ToggleEpinOfLCD();
}

void WriteDataToLCD(char LCDChar)
{
	LCD_RS = 1;               // It is data
	LCD_DAT = LCDChar;        // Write Data value on data bus

	ToggleEpinOfLCD();
}

void InitLCD(void)
{
	// Firstly make all pins output
	LCD_E   	     = 0;      // E = 0
        LCD_RW               = 0;
	LCD_RS    	     = 0;      // D = 0
	LCD_DAT              = 0;      // CLK = 0
        
	LCD_E_Dir            = 0;      // Make Output
        LCD_RW_Dir           = 0;      // Make Output
        LCD_RS_Dir           = 0;      // Make Output
	LCD_DAT_Dir          = 0;      // Make Output

  ///////////////// Reset process from datasheet //////////////
   delay(320000);                   // Determined by 15 ms and 8 Mhz clk--page 11 of SPLC780 datasheet
   WriteCommandToLCD(0x30);

   delay(32800);                    // Determined by 4.1 ms--32800
   WriteCommandToLCD(0x30);

   delay(800);                      // 100 usec
   WriteCommandToLCD(0x30);

   delay(120000);                   // 15 msec
  /////////////////////////////////////////////////////

   WriteCommandToLCD(0x38);          //function set
   WriteCommandToLCD(0x0C);          //display on/off control
                                     //display on,cursor off,blink off
   WriteCommandToLCD(0x01);          //clear display
   WriteCommandToLCD(0x06);          //entry mode, set increment
}



void WriteStringToLCD(const char *s)
{

    WriteDataToLCD(*s);   // print first character on LCD
}



void ClearLCDScreen(void)
{
	WriteCommandToLCD(0x01);    // Clear the screen
	delay(EDelay);              // Delay for cursor to return at zero position
}

void main(void)
{        
        InitLCD();
        ClearLCDScreen();
        InitLCD();
        ClearLCDScreen();

        WriteStringToLCD("Hello world!");

}
