/* 
 * File:   newmain.c
 * Author: David Crowder
 *
 * Created on November 9, 2014, 6:20 PM
 */

#include <PIC16f887.h>

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

#include <htc.h>

__CONFIG(FOSC_INTRC_NOCLKOUT & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_OFF);
__CONFIG(BOR4V_BOR21V & WRT_OFF);



#include <pic.h> /* This header file contains symbolic register addresses
that may be used to perform input/output operations
and to access other resources on the PIC 16F877 microcontroller*/

#define NDELAY 53000 /* This delay should be set to a low value (say 4) for trace simulation
debugging, but then to a higher value, say 30000, after the program is
debugged. The precise value depends upon the clock frequency. */

#define PushButton RB0 // Redefine RB0 more meaningfully to fit this application as ?PushButton?

#define LED RD0 // Redefine RD0 more meaningfully to fit this application as ?LED?


void main()
{
unsigned int i,j; //?unsigned int? specifies one or more 16-bit unsigned integer variables, which may
//hold values between 0 and 65535. Note that two precious RAM registers are required for
//each unsigned int variable that you define. If you want to count higher than 65535,
//you must use a ?long int? 32-bit variable specification, which requires 4 precious RAM
//registers. If you know that your variable need only take on values from 0 to 255, use
//?unsigned char? 8-bit unsigned integer specification. If you need a variable that
//holds signed values between -128 and +127, then use the ?char? signed 8-bit integer
//specification. Either of these 8-bit specifications use only one RAM register.

TRISB0 = 1; ANS12 = 0; //Make RB0 an input.
//Since RB0 is also analog input AN12, the ANS12 bit must be cleared as well as TRISB0 bit set.
TRISD0 = 0; //Make RD0 an output.
LED = 0; //Turn OFF the LED on RD0)
while(1) //Do this for loop forever!
{
while (PushButton == 1); // Wait here until switch is closed (RB0 goes LOW).
// Note equality comparison is denoted by
// two repeated equal signs! A single equal
// sign implies assignment of a value to a variable, and would not be correct here.
// This also works: ?while(PushButton);? Since a non-zero value is ?True? and a zero value is ?False?
for(i = 0; i < 8; i++)
{

for(j = 0; j < NDELAY; j++) //Loop for half a second
{
LED = 1;
}

for(j = 0; j < NDELAY; j++) //Loop for half a second
{
LED = 0;
}
}
}
}



