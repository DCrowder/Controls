//      Author:     David Crowder
//      Name:       FinalCode.c
//      Purpose:    


//      MCU header info
#include <PIC16f887.h>
#include <htc.h>
#include <pic.h>
#include <stdio.h>
#include <math.h>

// Configuration Bits
__CONFIG(FOSC_INTRC_NOCLKOUT & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_OFF);
__CONFIG(BOR4V_BOR21V & WRT_OFF);

//.................LCD Information...................../

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

// Delay time for Enable pin: defined by the LCD module datasheet.
#define EDelay 304 //304 cycles = 38 usec

//.............End of LCD Information............./

// ADC Module Information
#define PORT_SEN_DIR TRISA0
#define ANALOG_SEN ANS0
#define SENSORH ADRESH
#define SENSORL ADRESL

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
  /////////////////////////////////////////////////////////////

   WriteCommandToLCD(0x38);          //function set
   WriteCommandToLCD(0x0C);          //display on/off control
                                     //display on,cursor off,blink off
   WriteCommandToLCD(0x01);          //clear display
   WriteCommandToLCD(0x06);          //entry mode, set increment
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

// Column directions
#define DIRCOL1 TRISE0
#define DIRCOL2 TRISE1
#define DIRCOL3 TRISE2

// Row directions
#define DIRROW1 TRISB0
#define DIRROW2 TRISB1
#define DIRROW3 TRISB2
#define DIRROW4 TRISB3

char * IntToText(int n)
{
    // IntToText(n) reads an integer and creates a 5 character array
    // that corresponds to a four digit ASCII format of the integer.

    // Modulus (%) takes the integer n and can find the remainder
    // of n divided by 10, 100, and 1000.  This is used to find
    // the single digit integer of n for the thousands, hundreds, tens,
    // and ones place.

    static int bin[5];
    bin[0] = (n - n%1000)/1000;         // thousands value
    bin[1] = (n%1000 - n%100)/100;      // hundreds value
    bin[2] = (n%100 - n%10)/10;         // tens value = tens and ones value minus the ones value
    bin[3] = n%10;                      // ones value
    bin[4] = '\0';                      // null terminator
    
    // convert the four values into ASCII digits
    // must be typecasted because the array is converted from int to char.
    static char ascii[5];
    ascii[0] = (char)(bin[0] + '0');    // ASCII form of binary value
    ascii[1] = (char)(bin[1] + '0');    // swapping order so that
    ascii[2] = (char)(bin[2] + '0');
    ascii[3] = (char)(bin[3] + '0');
    ascii[4] = bin[4]; // null terminator


  // return 4 digit ASCII array of integer n with closing null terminator
  return ascii;
}

void ColumnsOutput()
{
    // Set direction of column to an output = 0
    DIRCOL1 = 0;
    DIRCOL2 = 0;
    DIRCOL3 = 0;
}

void RowsInput()
{
    // Set direction of row to an input = 1
    DIRROW1 = 1;
    DIRROW2 = 1;
    DIRROW3 = 1;
    DIRROW4 = 1;
}

void ColumnsInput()
{
    // Set direction of column to an input = 1
    DIRCOL1 = 1;
    DIRCOL2 = 1;
    DIRCOL3 = 1;
}
void RowsOutput()
{
    // Set direction of row to an output = 0
    DIRROW1 = 0;
    DIRROW2 = 0;
    DIRROW3 = 0;
    DIRROW4 = 0;
}

char assignNumpad()
{
    // assignNumpad() reads the ports corresponding to the rows and columns
    // of the number pad and stores it in memory.


    // keys is an array with an additional row so '$' can be stored.
    // '$' is the default character used because $ should never display to
    // the screen.
    // The rest of the values are stored so the key value can be pulled
    // when a button is pressed.
    
    unsigned char keys[5][3] =
    {'1','2','3',
    '4','5','6',
    '7','8','9',
    '*','0','#',
    '$','\0','\0'};

    // Rows 1-4 have flags to store the current row values
    int R1Flag = 0;
    int R2Flag = 0;
    int R3Flag = 0;
    int R4Flag = 0;

    // Columns 1-3 have flags to store the current column values
    int C1Flag = 0;
    int C2Flag = 0;
    int C3Flag = 0;

    
    // Default invalid character stored as char called digit
    char digit = '$';

    // Columns are outputs, rows are inputs
    ColumnsOutput();
    RowsInput();

    // Turn on columns so that the rows will go high when a button is pressed
    COL1 = 1;
    COL2 = 1;
    COL3 = 1;

    // Assign current row values to each flag
    R1Flag = ROW1;
    R2Flag = ROW2;
    R3Flag = ROW3;
    R4Flag = ROW4;

    //Columns are inputs, rows are outputs
    ColumnsInput();
    RowsOutput();

    //Turn on rows so that the columns will go high when a button is pressed
    ROW1 = 1;
    ROW2 = 1;
    ROW3 = 1;
    ROW4 = 1;

    // Assign current column values to each flag
    C1Flag = COL1;
    C2Flag = COL2;
    C3Flag = COL3;

    // Invalid array number: $ by default.  Will be changed later.
    int RowFlag = 4;
    int ColFlag = 0;


    // Sweep Rows by checking if a flag is HIGH.
    // If a flag is high, the RowFlag is assigned with the row number detected.
    if (R1Flag)
    {
           RowFlag = 0;
    }
    if (R2Flag)
    {
           RowFlag = 1;
    }
    if (R3Flag)
    {
           RowFlag = 2;
    }
    if (R4Flag)
    {
           RowFlag = 3;
    }

    // Sweep Columns by checking if a flag is HIGH.
    // If a flag is high, the ColFlag is assigned with the column number detected.
    if (C1Flag)
    {
           ColFlag = 0;
    }
    if (C2Flag)
    {
           ColFlag = 1;
    }
    if (C3Flag)
    {
           ColFlag = 2;
    }

    // Assign the char digit with the ASCII values found in the keys array.
    digit = keys[RowFlag][ColFlag];

    // Return the assigned char
    return digit;


}

void InitADC(void)
{
    // All information for ADC initialization can be found on
    // page 100 of the PIC16F887 datasheet.

    PORT_SEN_DIR = 1;       // Sets the sensor port as an input
    ANALOG_SEN = 1;         // Sets the sensor pin as analog
    
    // A/D Control Registers
    ADCON0 = 0x01;          // Analog channel 1 (AN0) ADCON0<5:2> = CHS<3:0> 0000
                            // Fosc/2 as conversion clock bits ADCON0<7:6> = ADCS<2:0> = 00
                            // ADC Enable bit turned on  ADCON0<0> = ADON = 0 (p.104)
    
    ADCON1 = 0xFF;          // left justified, Vref pins active (p.105)

    delay(EDelay);          // Delay the required length of acquisition (Fosc/2)
                            // EDelay is close enough to the necessary delay value
                            // and does not hinder the program.
                            // The sensor has been tested successfully with this delay.
    
    ADCON0 = 0x03;          // Sets the GO/!DONE bit so that ADC begins (p. 105)

    while(ADCON0 == 0x03);  // Poll for GO/!DONE bit to be cleared
}

void PromptLCD(int stage)
{
    ClearLCDScreen();       // Clear the screen before any stage messages

    //  The LCD displays incorrectly without sending commands.
    //  Each line is labeled by order (1-5).
    //  Each line is 16 characters long except for line 5.
    //  Line 5 has 8 characters that do not display to the LCD screen.

    //  The lines display in 1, 3, 5, 2, and 4.
    //  Imagine the display reads A-Z, 0-9, and then repeats.

    //-----------Sample LCD-------------//
    //      ABCDEFGHIJKLMNOP    Line 1  //
    //      EFGHIJKLMNOPQRST    Line 2  //
    //      QRSTUVWXYZ012345    Line 3  //
    //      UVWXYZ0123456789    Line 4  //
    //      6789ABCD            Line 5  //
    //----------------------------------//


    // Check patient's
    // leg & footplate
    // straps.  Press
    // # to continue."

    if (stage == 1)     // Check patient
    {
        WriteStringToLCD(" Check patient's");       // 1st line
        WriteStringToLCD(" straps.  Press ");       // 3rd line
        WriteStringToLCD("        ");               // 5th line
        WriteStringToLCD(" leg & footplate");       // 2nd line
        WriteStringToLCD(" # to continue. ");       // 4th line

    }

    // Check ratchets:
    // Motor side: down
    // Other side: up
    // # to continue.

    if (stage == 2)     // Check ratchet
    {
        WriteStringToLCD(" Check ratchets:");       // 1st line
        WriteStringToLCD("Other side: up  ");       // 3rd line
        WriteStringToLCD("        ");               // 5th line
        WriteStringToLCD("Motor side: down");       // 2nd line
        WriteStringToLCD(" # to continue. ");       // 4th line
    }

    // Enter Patient
    // Weight: _                      (blinking cursor)
    // Enter: # Back *

    // The "Weight: " string is last, therefore in code we can display
    // the weight without sending an additional command.

    if (stage == 3)     // Enter patient weight
    {
        WriteStringToLCD(" Enter  Patient ");       // 1st line
        WriteStringToLCD("Enter: # Back: *");       // 3rd line
        WriteStringToLCD("        ");               // 5th line
        WriteStringToLCD("   Weight: ");            // 2nd line
    }

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

       char number[4] = "000";
       double numberInt = 0;
       char digit[2] = "$";

        TRISE = 0;
        TRISB = 1;

        TRISC = 0 ;                     // set PORTC as output
        PORTC = 0 ;                     // clear PORTC

        /*
         * configure CCP module as 4000 Hz PWM output
         */
        PR2 = 0b01111100 ;
        T2CON = 0b00000101 ;
        CCP1CON = 0b00001100 ;
        CCP2CON = 0b00111100 ;


       unsigned char   dc ;

       dc = 0;  // set duty cycle to 0

       CCPR2L = dc;               // store dc value
       CCPR1L = 128 - dc ;

        

       InitLCD();
       ClearLCDScreen();
       InitLCD();
       ClearLCDScreen();

       
       PromptLCD(1);    // stage = 1
       
       while (digit[0] != '#')
       {
           digit[0] = assignNumpad();
       }

       digit[0] = '$';
       delay(10000);

       PromptLCD(2);                // stage = 2;
       
       while (digit[0] != '#')
       {
           digit[0] = assignNumpad();
       }

       digit[0] = '$';
       delay(10000);

       PromptLCD(3);               // stage = 3
       WriteCommandToLCD(0x0F);    //  Turn Cursor on

       int count = 0;

       while(digit[0] != '#' || count < 3)
       {
            digit[0] = assignNumpad();
            delay(1000000);

                if (digit[0] != '$')    // button has been pressed
                {
                    if (digit[0] == '*' && count > 0) // only works if count is greater than 0
                    {
                        WriteCommandToLCD(0x10);    // C and L bits: move cursor left
                        WriteStringToLCD(" ");      // Erase former digit
                        WriteCommandToLCD(0x10);    // C and L bits: move cursor left
                        count--;                    // Go back a step in array
                    }
                    else if (digit[0] != '#' && count < 3 && digit[0] != '*')   // only if operator doesn't want to continue
                    {
                        number[count] = digit[0];   // place the digit into the number array
                        WriteStringToLCD(digit);    // display 
                        count++;
                    }
                    delay(1000);       // Delay after action has been done
                }
                
       }

       // Adds each digit to a new array from ASCII form.
       //

       for(int i = 0; i < 3; i++)
       {

           numberInt += (pow(10,2-i))*(number[i] - '0');
            
       }

       double forceMax = 0;        // maximum force value for weight
       double threshold = 0;       // maximum voltage value for weight

       if (numberInt >= 150)
       {
           forceMax = 15;
       }
       else
       {
           forceMax = numberInt/10;
       }

        int a = 17;     // slope of force to voltage equation
        int b = 435;    // offset of force to voltage equation

       threshold = a*forceMax + b;      // force to voltage equation




       //  Initialize the ADC
       InitADC();


        //  Combines the ADC value into a single 2-byte variable
        //  ADRESH/SENSORH is shifted 8 bits to the right (making it 2^(8,9))
        //  ADRESH/SENSORH is OR-ed with ADRESL/SENSORL to get the 10 bit value
        int short SENSOR = (SENSORH<<8) | SENSORL;

        int displaySensor = 0;

        char UserForceDec[2] = "0";
        char UserForceOne[2] = "0";
        char UserForceTen[2] = "0";

        float thresholdTen = threshold*10;
        float forceMaxTenFloat = (thresholdTen - 10*b)/a;
        char forceMaxTenChar = (char) forceMaxTenFloat;

        char * UserForceMax = IntToText(forceMaxTenChar);

        char UserForceMaxDec[2] = "0";
        char UserForceMaxOne[2] = "0";
        char UserForceMaxTen[2] = "0";


        UserForceMaxDec[0] = forceMaxTenChar%10 + '0';  // quick fix to decimal not displaying right
        UserForceMaxOne[0] = UserForceMax[2];
        UserForceMaxTen[0] = UserForceMax[1];

        ClearLCDScreen();       // clear screen for next stage



        while(1)
        {
            ADCON0 = 0x01;
            delay(EDelay);          // Delay the required length of acquisition (Fosc/2)
            ADCON0 = 0x03;          // Sets the GO/!DONE bit so that ADC begins

            while(ADCON0 == 0x03);  // Poll for GO/!DONE bit to be cleared


            SENSOR = (SENSORH<<8) | SENSORL;

            if (assignNumpad() == '#')
            {
                if (SENSOR < threshold && dc <= 127)
                {
                    if (dc == 0)        // initial dc start up
                    {
                        dc = 50;
                    }
                    else
                    {
                        dc += 1;    // add one to the duty cycle
                    }
                }
                else if (SENSOR >= threshold)
                {
                    dc -= 5;
                }

            }
            else
            {
                dc = 0;         // only run if # is pressed
            }

                CCPR2L = dc ;               // store dc value
                CCPR1L = 128 - dc ;

            //  IntToText converts an integer to ASCII

            float sensorTen = SENSOR*10;
            float forceTenFloat = (sensorTen - 10*b)/a;
            char forceTenChar = (char) forceTenFloat;

            if (displaySensor%50 == 0)
            {
                WriteCommandToLCD(0x02);                    // return cursor to home
                WriteStringToLCD("Hold # to start.");       // line 1

                // line 3: --####----####--
                WriteStringToLCD("  ");

                int * UserSensor = IntToText(SENSOR);
                WriteStringToLCD(UserSensor);   // display current sensor value text

                WriteStringToLCD("    ");

                if (SENSOR >= 435)
                {
                    char * UserForce = IntToText(forceTenFloat);

                    UserForceDec[0] = forceTenChar%10 + '0';  // quick fix to decimal not displaying right
                    UserForceOne[0] = UserForce[2];
                    UserForceTen[0] = UserForce[1];

                    WriteStringToLCD(UserForceTen);

                    WriteStringToLCD(UserForceOne);
                    WriteStringToLCD(".");       // currently can't get proper value
                    WriteStringToLCD(UserForceDec);
                    WriteStringToLCD("  ");
//
                }                
                else
                {
                    WriteStringToLCD("####");
                    WriteStringToLCD("  ");
                }
                
           }
                // end of line 3


                WriteStringToLCD("        ");               // line 5
                WriteStringToLCD(" Sensor: Force: ");      // line 2




                // line 4: --####----####--
                char * UserThreshold = IntToText(threshold); // threshold text
                WriteStringToLCD("  ");
                WriteStringToLCD(UserThreshold);    // display threshold value text
                WriteStringToLCD("    ");

                // Example: 15.0
                if (UserForceMaxTen[0] == '1')
                {
                    WriteStringToLCD(UserForceMaxTen);
                }
                else
                {
                    WriteStringToLCD(" ");
                }
                WriteStringToLCD(UserForceMaxOne);
                WriteStringToLCD(".");       // currently can't get proper value
                WriteStringToLCD(UserForceMaxDec);
                
                WriteStringToLCD("  ");
                // end of line 4

            }

            displaySensor++;

        while(1);
        

        }
