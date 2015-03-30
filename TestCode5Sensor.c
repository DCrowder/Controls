#include <PIC16f887.h>
#include <ctype.h>

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

#include <htc.h>

#include <pic.h> /* This header file contains symbolic register addresses
that may be used to perform input/output operations
and to access other resources on the PIC 16F887 microcontroller*/



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

// Numpad Ports
#define COL1 RE0
#define COL2 RE1
#define COL3 RE2

#define ROW1 RB0
#define ROW2 RB1
#define ROW3 RB2
#define ROW4 RB3

#define DIRCOL1 TRISE0
#define DIRCOL2 TRISE1
#define DIRCOL3 TRISE2

#define DIRROW1 TRISB0
#define DIRROW2 TRISB1
#define DIRROW3 TRISB2
#define DIRROW4 TRISB3

// ADC Module Information

#define PORT_SEN_DIR TRISA0
#define ANALOG_SEN ANS0
#define SENSOR ADRESL


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
   delay(150000);                   // Determined by 15 ms and 8 Mhz clk--p11 of SPLC780 datasheet
   WriteCommandToLCD(0x30);

   delay(35000);                    // Determined by 4.1 ms--32800
   WriteCommandToLCD(0x30);

   delay(1000);                      // 100 usec--800
   WriteCommandToLCD(0x30);

   delay(150000);                   // 15 msec--120000
  /////////////////////////////////////////////////////

   WriteCommandToLCD(0x38);          //function set
   delay(35000);
   WriteCommandToLCD(0b00001100);          //display on/off control
   delay(35000);                                     //display on,cursor off,blink off
   WriteCommandToLCD(0x01);          //clear display
   delay(35000);
   WriteCommandToLCD(0x06);          //entry mode, set increment
   delay(35000);

}

void InitADC(void)
{
    PORT_SEN_DIR = 1;       // Sets the sensor port as an input
    ANALOG_SEN = 1;         // Sets the sensor pin as analog
    ADCON1 = 0xFF;
    ADCON0 = 0x01;
    delay(EDelay);             // Delay the required length of acquisition (Fosc/2)
    ADCON0 = 0x03;          // Sets the GO/!DONE bit so that ADC begins


    while(ADCON0 == 0x03);  // Poll for GO/!DONE bit to be cleared
}

void WriteStringToLCD(const char *s)
{
    while(*s)
    {
	WriteDataToLCD(*s++);   // print first character on LCD
    }
}


void ClearLCDScreen(void)
{
	WriteCommandToLCD(0x01);    // Clear the screen
	delay(120000);              // Delay for cursor to return at zero position
}

char hexDigit(int n)
{
    if (n < 10) {
        return n + '0';
    } else {
        return (n - 10) + 'A';
    }
}

char * BinToText(int n)
{
  static int bin[4];
  bin[3] = n%10;                      // ones value
  bin[2] = (n%100 - bin[0])/10;        // tens value = tens and ones value minus the ones value
  bin[1] = (n - bin[1] - bin[0])/100;    // whole number minus the tens and ones values
  bin[0] = '\0';       // null terminator
  // convert all three values into ASCII digits
  
  static char ascii[4];
  ascii[0] = (char)(bin[0] + '0');   // ASCII form of binary value
  ascii[1] = (char)(bin[1] + '0');
  ascii[2] = (char)(bin[2] + '0');
  ascii[3] = bin[0]; // null terminator
     

     // return needed value
     return ascii;
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

    
    //const char * UserSensor[] = BinToText(SENSOR);

    
    //  Would like to use strcat() but it's giving me an error.  Why?
    //  strcat(UserSensor,BinToText(SENSOR));


    InitADC();


    InitLCD();
    ClearLCDScreen();
    InitLCD();
    ClearLCDScreen();
    
    while(1)
    {

        char * UserSensor = BinToText(SENSOR);
        WriteStringToLCD(UserSensor);
        WriteStringToLCD(" ");
        ADCON0 = 0x01;
        delay(EDelay);             // Delay the required length of acquisition (Fosc/2)
        ADCON0 = 0x03;          // Sets the GO/!DONE bit so that ADC begins

        while(ADCON0 == 0x03);  // Poll for GO/!DONE bit to be cleared
       
    }
     
        // Written strings must be ordered by lines in 16 character segments


}

