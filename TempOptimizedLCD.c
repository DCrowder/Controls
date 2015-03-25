//      Author:     David Crowder
//      Name:       TempOptimizedLCD.c
//      Purpose:    Cleaned up file of LCD.  This file will replace the LCD
//                  test file if successful.

//      MCU header info
#include <PIC16f887.h>
#include <htc.h>
#include <pic.h>

//      Configuration Bits
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

//      Delay time for Enable pin: defined by the LCD module datasheet.
#define EDelay 1000 //304 cycles = 38 usec


void delay(unsigned int time)
{
    //  Delay function used for any delay in the system
    unsigned int i;

   for(i = 0; i < time; i++); //Loop

}

void ToggleEpinOfLCD(void)
{
    //  Toggles the Enable pin of the LCD to send data/instructions
	LCD_E = 1;                // Give a pulse on E pin
	delay(EDelay);            // so that LCD can latch the
	LCD_E = 0;                // data from data bus
	delay(EDelay);
}


void WriteCommandToLCD(unsigned char Command)
{
    //  Writes a command to the LCD
	LCD_RS = 0;               // It is a command
	LCD_DAT = Command;        // Write Command value on data bus

	ToggleEpinOfLCD();
}

void WriteDataToLCD(char LCDChar)

{
    //  Writes data to the LCD
	LCD_RS = 1;               // It is data
	LCD_DAT = LCDChar;        // Write Data value on data bus

	ToggleEpinOfLCD();
}

void InitLCD(void)
{
    //  Initializes the LCD when the MCU program first runs

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
    //  This subroutine writes each character until a null terminator is reached
    WriteDataToLCD(*s);                         // print a character on LCD
}



void ClearLCDScreen(void)
{
    //  Clears the LCD screen
	WriteCommandToLCD(0x01);                // Clear the screen
	delay(EDelay);                          // Delay for cursor to return at zero position
}

void main(void)
{        
        InitLCD();                              // Initialize LCD module
        ClearLCDScreen();                       // Clear the screen

        WriteStringToLCD("Hello world!");       // Write string to LCD
        while(1);                               //  Puts program into infinite loop to freeze screen
}
