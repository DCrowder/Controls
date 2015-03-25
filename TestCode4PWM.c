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

#include <pic.h> /* This header file contains symbolic register addresses
that may be used to perform input/output operations
and to access other resources on the PIC 16F877 microcontroller*/

__CONFIG(FOSC_INTRC_NOCLKOUT & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_OFF);
__CONFIG(BOR4V_BOR21V & WRT_OFF);

#define NDELAY 530 /* This delay should be set to a low value (say 4) for trace simulation
debugging, but then to a higher value, say 30000, after the program is
debugged. The precise value depends upon the clock frequency. */

#define PushButton RB0 // Redefine RB0 more meaningfully to fit this application as ?PushButton?

#define LED RD0 // Redefine RD0 more meaningfully to fit this application as ?LED?

// Numpad Ports
#define Col1 RE0
#define Col2 RE1
#define Col3 RE2

#define Row1 RB0
#define Row2 RB1
#define Row3 RB2
#define Row4 RB3

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


// PWM Ports
#define PWM CCP2 // Redefine CCP2 as PWM


void main(void)
{

        unsigned int    i,j;
        unsigned char   dc ;

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

        for(;;)                         // forever
                {
                /*
                 * PWM resolution is 10 bits
                 * don't use last 2 less significant bits CCPxCON,
                 * so only CCPRxL have to be touched to change duty cycle
                 */
                for(dc = 0 ; dc < 128 ; dc++)
                        {
                        CCPR1L = dc ;
                        CCPR2L = 128 - dc ;
                         for(i = 0; i < NDELAY; i++) //Loop
                                {
                                }
                        }
                for(dc = 127 ; dc > 0 ; dc--)
                        {
                        CCPR1L = dc ;
                        CCPR2L = 128 - dc ;
                         for(j = 0; j < NDELAY; j++) //Loop
                                {
                                }
                        }
                }
    
}
