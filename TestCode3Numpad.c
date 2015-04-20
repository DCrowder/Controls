//      Author:     David Crowder
//      Name:       TestCod3Numpad.c
//      Purpose:    Checks the value of the number pad and displays the number
//                  to the screen.

//      MCU header info
#include <PIC16f887.h>
#include <htc.h>
#include <pic.h>


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

//Enable bit delay time
#define EDelay 304 //304 cycles = 38 usec

// Numpad Ports
#define Col1 RE0
#define Col2 RE1
#define Col3 RE2

#define Row1 PORTBbits.RB0
#define Row2 PORTBbits.RB1
#define Row3 PORTBbits.RB2
#define Row4 PORTBbits.RB3

// Numpad Directions
#define Col1_dir TRISE0
#define Col2_dir TRISE1
#define Col3_dir TRISE2

#define Row1_dir TRISB0
#define Row2_dir TRISB1
#define Row3_dir TRISB2
#define Row4_dir TRISB3

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
   delay(120000);                   // Determined by 15 ms and 8 Mhz clk--p11 of SPLC780 datasheet
   WriteCommandToLCD(0x30);

   delay(32800);                    // Determined by 4.1 ms--32800
   WriteCommandToLCD(0x30);

   delay(800);                      // 100 usec--800
   WriteCommandToLCD(0x30);

   delay(150000);                   // 15 msec--120000
  /////////////////////////////////////////////////////

   WriteCommandToLCD(0x38);                //function set
   delay(35000);
   WriteCommandToLCD(0b00001100);          //display on/off control
   delay(35000);                           //display on,cursor off,blink off
   WriteCommandToLCD(0x01);                //clear display
   delay(35000);
   WriteCommandToLCD(0x06);                //entry mode, set increment
   delay(35000);

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
	delay(35000);              // Delay for cursor to return at zero position
}

char * IntToText(int n)
{
  static int bin[5];
  bin[3] = n%10;                      // ones value
  bin[2] = (n%100 - n%10)/10;         // tens value = tens and ones value minus the ones value
  bin[1] = (n%1000 - n%100)/100;      // whole number minus the tens and ones values
  bin[0] = '\0';                      // null terminator
  // convert all three values into ASCII digits

  static char ascii[5];
  ascii[0] = (char)(bin[1] + '0');    // ASCII form of binary value
  ascii[1] = (char)(bin[2] + '0');
  ascii[2] = (char)(bin[3] + '0');
  ascii[3] = bin[0]; // null terminator


     // return needed value
     return ascii;
}

unsigned char ReadNumpad(void)
{
    unsigned char numpadEntry = 0x80;

    // The way Rows and Columns are stored:
    //  (1)(CCC)(RRRR) -- 1, C columns, R rows

    // Row values
    unsigned char Row1_bin = 0b10000001;
    unsigned char Row2_bin = 0b10000010;
    unsigned char Row3_bin = 0b10000100;
    unsigned char Row4_bin = 0b10001000;

    // Column values
    unsigned char Col1_bin = 0b10010000;
    unsigned char Col2_bin = 0b10100000;
    unsigned char Col3_bin = 0b11000000;

    // 2a: Col1
    Col1 = 1;
    Col2 = 0;
    Col3 = 0;
    delay(6000);

    WriteStringToLCD("Column 1: *");
    WriteDataToLCD(Row1);
    WriteDataToLCD(Row2);
    WriteDataToLCD(Row3);
    WriteDataToLCD(Row4);
    WriteStringToLCD("*");

    if((Row1||Row2||Row3||Row4) == 1)
    {
        numpadEntry |= Col1_bin;

        if(Row1 == 1)
        {
            numpadEntry |= Row1_bin;
        }
        if(Row2 == 1)
        {
            numpadEntry |= Row2_bin;
        }
        if(Row3 == 1)
        {
            numpadEntry |= Row3_bin;
        }
        if(Row4 == 1)
        {
            numpadEntry |= Row4_bin;
        }

    }

    // 2b: Col2
    Col1 = 0;
    Col2 = 1;
    Col3 = 0;
    delay(6000);
    WriteStringToLCD("Column 2: *");
    WriteDataToLCD(Row1);
    WriteDataToLCD(Row2);
    WriteDataToLCD(Row3);
    WriteDataToLCD(Row4);
    WriteStringToLCD("*");

    if((Row1||Row2||Row3||Row4) == 1)
    {
        numpadEntry |= Col2_bin;

        if(Row1 == 1)
        {
            numpadEntry |= Row1_bin;
        }
        if(Row2 == 1)
        {
            numpadEntry |= Row2_bin;
        }
        if(Row3 == 1)
        {
            numpadEntry |= Row3_bin;
        }
        if(Row4 == 1)
        {
            numpadEntry |= Row4_bin;
        }

    }

    // 2c: Col3
    Col1 = 0;
    Col2 = 0;
    Col3 = 1;
    delay(6000);

    WriteStringToLCD("Column 3: *");
    WriteDataToLCD(Row1);
    WriteDataToLCD(Row2);
    WriteDataToLCD(Row3);
    WriteDataToLCD(Row4);
    WriteStringToLCD("*");

    if((Row1||Row2||Row3||Row4) == 1)
    {
        numpadEntry |= Col3_bin;

        if(Row1 == 1)
        {
            numpadEntry |= Row1_bin;
        }
        if(Row2 == 1)
        {
            numpadEntry |= Row2_bin;
        }
        if(Row3 == 1)
        {
            numpadEntry |= Row3_bin;
        }
        if(Row4 == 1)
        {
            numpadEntry |= Row4_bin;
        }
    }

    WriteDataToLCD(numpadEntry);
    return numpadEntry;
}

int bitcount (unsigned int n)
{
   int count = 0;
   while (n) {
      count += n & 0x1u;
      n >>= 1;
   }
   return count;
}

unsigned char numpadNum (unsigned char numpadEntry)
{
    //row 1
    unsigned char num1_bin =    0b10010001;
    unsigned char num2_bin =    0b10100001;
    unsigned char num3_bin =    0b11000001;

    //row 2
    unsigned char num4_bin =    0b10010010;
    unsigned char num5_bin =    0b10100010;
    unsigned char num6_bin =    0b11000010;

    //row 3
    unsigned char num7_bin =    0b10010100;
    unsigned char num8_bin =    0b10100100;
    unsigned char num9_bin =    0b11000100;

    //row 4
    unsigned char numastk_bin = 0b10011000;
    unsigned char num0_bin =    0b10101000;
    unsigned char numpnd_bin =  0b11001000;

    unsigned char number = '!';

    //row 1
    if(numpadEntry == num1_bin)
    {
        number = '1';
    }
    if(numpadEntry == num2_bin)
    {
        number = '2';
    }
    if(numpadEntry == num3_bin)
    {
        number = '3';
    }

    //row 2
    if(numpadEntry == num4_bin)
    {
        number = '4';
    }
    if(numpadEntry == num5_bin)
    {
        number = '5';
    }
    if(numpadEntry == num6_bin)
    {
        number = '6';
    }

    //row 3
    if(numpadEntry == num7_bin)
    {
        number = '7';
    }
    if(numpadEntry == num8_bin)
    {
        number = '8';
    }
    if(numpadEntry == num9_bin)
    {
        number = '9';
    }

    //row 4
    if(numpadEntry == numastk_bin)
    {
        number = '*';
    }
    if(numpadEntry == num0_bin)
    {
        number = '0';
    }
    if(numpadEntry == numpnd_bin)
    {
        number = '#';
    }
            WriteDataToLCD(number);
            return number;
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

int IsValid(char n)
{
     int count = 0;
     for (int i=0; i<8; i++) // Here we want an incremental bitwise shift through the numbers.
     {
          if (n%2 == 1)     // this checks to see if the number is odd.  With bitwise shifting each cycle, we should see every bit.
          {
               count++;          // increments count, signifying that there is a 1 in that bit space.
          }
          n<<1;               // Bitwise shift to left.
                              // Not sure that this syntax is correct.  Verify.
     }

     if (count == 3)
     {
          return 1;          // Bit 7 and 2 bits must be set from row and column.
     }
     else
     {
          return 0;
     }
}

char NumAssign(char NumFlag)
{

          char ColMask = 0x10001111;     // 1 = don't care, 0 = check this value
          char RowMask = 0x11110000;

          char Col1Check = 0x10011111;
          char Col2Check = 0x10101111;
          char Col3Check = 0x11001111;

          char Row1Check = 0x11110001;
          char Row2Check = 0x11110010;
          char Row3Check = 0x11110100;
          char Row4Check = 0x11111000;

          char CheckCol = NumFlag | ColMask;
          char CheckRow = NumFlag | RowMask;

          char number = '$';
          
//  Assign number if Col1 is detected
if (CheckCol == Col1Check)
{
     if (CheckRow == Row1Check)
     {
          number = '1';
     }
     else if (CheckRow == Row2Check)
     {
          number = '4';
     }
     else if (CheckRow == Row3Check)
     {
          number = '7';
     }
     else if (CheckRow == Row4Check)
     {
          number = '*';
     }
}

//  Assign number if Col2 is detected
else if (CheckCol == Col2Check)
{
    if (CheckRow == Row1Check)
    {
         number = '2';
    }
    else if (CheckRow == Row2Check)
    {
         number = '5';
    }
    else if (CheckRow == Row3Check)
    {
         number = '8';
    }
    else if (CheckRow == Row4Check)
    {
         number = '0';
    }
}

//  Assign number if Col3 is detected
else if (CheckCol == Col3Check)
{
    if (CheckRow == Row1Check)
    {
         number = '3';
    }
    else if (CheckRow == Row2Check)
    {
         number = '6';
    }
    else if (CheckRow == Row3Check)
    {
         number = '9';
    }
    else if (CheckRow == Row4Check)
    {
         number = '#';
    }
}

//  End of the number assignment
          
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

        InitLCD();
        ClearLCDScreen();
        InitLCD();
        ClearLCDScreen();

    while(1)
    {
    
    char number[] = "$";
    // ASCII value of the character entered will be stored here.
    // Invalid character so that program can't flow without an actual input.

    char NumFlag = 0x00000000;
    // Bit 7 signals whether or not any switch has
    // been pressed for the cycle.
    // 1 = switch pressed, 0 = no switch pressed.
    // Bits 6-4 are columns, bits 3-0 are rows.
    // 1 = Column or row detected, 0 = No column or row detected.

    int NumFlagText0 = IntToText(NumFlag);    // Verify this is actually 0

    //Columns are outputs, rows are inputs
    ColumnsOutput();
    RowsInput();
    
    //Turn on Columns
    COL1 = 1;
    COL2 = 1;
    COL3 = 1;

    //Default Rows to 0
    ROW1 = 0;
    ROW2 = 0;
    ROW3 = 0;
    ROW4 = 0;

    delay(100000); // Delay to give time for MCU to update

        if (ROW1 == 1)
        {
               NumFlag |= 0x10000001;
               // OR operator used because we don't want
               // to disturb any earlier edits to the flag.
        }
        if (ROW2 == 1)
        {
               NumFlag |= 0x10000010;
        }
        if (ROW3 == 1)
        {
               NumFlag |= 0x10000100;
        }
        if (ROW4 == 1)
        {
               NumFlag |= 0x10001000;
        }

    int NumFlagText1 = IntToText(NumFlag);

    
    //Columns are inputs, rows are outputs
    ColumnsInput();
    RowsOutput();

     //Turn on Rows
     ROW1 = 1;
     ROW2 = 1;
     ROW3 = 1;
     ROW4 = 1;

     //Default Columns to 0
     COL1 = 0;
     COL2 = 0;
     COL3 = 0;

     delay(100000); // Delay to give time for MCU to update
     
        if (COL1 == 1)
        {
               NumFlag |= 0x10010000;
        }
        if (COL2 == 1)
        {
               NumFlag |= 0x10100000;
        }
        if (COL3 == 1)
        {
               NumFlag |= 0x11000000;
        }

     int NumFlagText2 = IntToText(NumFlag);


     // int validFlag = IsValid(NumFlag);

     //if(validFlag == 1)     // only runs if the NumFlag is valid.
     //{
        number[0] = NumAssign(NumFlag);
     //}
        ClearLCDScreen();
        WriteStringToLCD(NumFlagText0);     // Initial value
        WriteStringToLCD(" ");
        WriteStringToLCD(NumFlagText1);     // After Rows checked
        WriteStringToLCD(" ");
        WriteStringToLCD(NumFlagText2);     // After Cols checked
        WriteStringToLCD(" ");
        WriteStringToLCD(number);           // ASCII value of number
     
    }



    while(1);

}
