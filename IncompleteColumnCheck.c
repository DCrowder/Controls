//      Author:     David Crowder
//      Name:       IncompleteColumnCheck.c
//      Purpose:    Checks the columns of the number pad and displays the value
//                  to the screen.

//      MCU header info
#include <PIC16f887.h>
#include <htc.h>
#include <pic.h>

//      Configuration Bits
__CONFIG(FOSC_INTRC_NOCLKOUT & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_OFF);
__CONFIG(BOR4V_BOR21V & WRT_OFF);

//      LCD Ports
#define LCD_E   RC5
#define LCD_RW  RC6
#define LCD_RS  RC7
#define LCD_DAT PORTD

//      LCD Directions
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
    WriteDataToLCD(*s);   // print first character on LCD
}



void ClearLCDScreen(void)
{
    //  Clears the LCD screen
	WriteCommandToLCD(0x01);    // Clear the screen
	delay(EDelay);              // Delay for cursor to return at zero position
}


// Numpad Ports
#define COL1 RE0
#define COL2 RE1
#define COL3 RE2

#define ROW1 RB0
#define ROW2 RB1
#define ROW3 RB2
#define ROW4 RB3

// Direction of Numpad Ports
#define DIRCOL1 TRISE0
#define DIRCOL2 TRISE1
#define DIRCOL3 TRISE2

#define DIRROW1 TRISB0
#define DIRROW2 TRISB1
#define DIRROW3 TRISB2
#define DIRROW4 TRISB3

char hexDigit(int n)
{
        return n + '0';
}


void ColumnsOutput()
{
    DIRCOL1 = 0;
    DIRCOL2 = 0;
    DIRCOL3 = 0;
}

void RowsInput()
{
        DIRROW1 = 1;
        DIRROW2 = 1;
        DIRROW3 = 1;
        DIRROW4 = 1;
}

void ColumnsInput()
{
    DIRCOL1 = 1;
    DIRCOL2 = 1;
    DIRCOL3 = 1;
}
void RowsOutput()
{
    DIRROW1 = 0;
    DIRROW2 = 0;
    DIRROW3 = 0;
    DIRROW4 = 0;
}

void main(void)
{
        ANSEL = 0; // all ports digital
        ANSELH = 0; // all ports digital
        CM1CON0 = 0; //comparators off
        CM2CON0 = 0;//comparators off
        ADCON0 = 0; //ADC off
        ADCON1 = 0; //ADC off
        WPUB = 1; //pullup
        IOCB = 0; //interrupt on change

        int RowVal = 0;
        char number[] = "0";

        //Columns are outputs, rows are inputs
        ColumnsOutput();
        RowsInput();

        //Turn on Columns
        COL1 = 1;
        COL2 = 1;
        COL3 = 1;
        
        InitLCD();
        ClearLCDScreen();
        InitLCD();
        ClearLCDScreen();


        while(1)
        {
            RowVal = ROW1;
            number[0] = hexDigit(RowVal);
            WriteStringToLCD(number);

            RowVal = ROW2;
            number[0] = hexDigit(RowVal);
            WriteStringToLCD(number);

            RowVal = ROW3;
            number[0] = hexDigit(RowVal);
            WriteStringToLCD(number);

            RowVal = ROW4;
            number[0] = hexDigit(RowVal);
            WriteStringToLCD(number);
        }

        while(1);
        // Written strings must be ordered by lines in 16 character segments


}

