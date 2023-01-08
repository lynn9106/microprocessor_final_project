#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
#pragma config BOREN = ON       // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)
#define _XTAL_FREQ 4000000
void main(void) {
    //configure OSC and port
    OSCCONbits.IRCF = 0b001; //125KHz

    //output led
    TRISDbits.RD0=0;
    TRISDbits.RD1=0;
    TRISDbits.RD2=0;
    TRISDbits.RD3=0;
    LATD=0b0000;
    
    while(!INTCONbits.TMR0IF){
        LATD=0b0001;
        __delay_ms(10);
        LATD=0b0010;
        __delay_ms(10);
        LATD=0b0100;
        __delay_ms(10);
        LATD=0b1000;
        __delay_ms(10);
        
        LATD=0b0100;
        __delay_ms(10);
        LATD=0b0010;
        __delay_ms(10);
    }
    while(1){
        LATD=0b1111;
        __delay_ms(10);
        LATD=0b0000;
        __delay_ms(10);
    }
    
    return;
}
