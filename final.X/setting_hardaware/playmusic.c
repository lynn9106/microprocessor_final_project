
#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit 
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON   // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

#define _XTAL_FREQ  16000000
/* frequency list */
#define NOTE_REST   0.0
#define NOTE_C1		32.7032 
#define NOTE_CS1	34.6478 
#define NOTE_D1		36.7081 
#define NOTE_DS1	38.8909 
#define NOTE_E1		41.2034 
#define NOTE_F1		43.6535 
#define NOTE_FS1	46.2493
#define NOTE_G1		48.9994
#define NOTE_GS1	51.9130
#define NOTE_A1		55.0000
#define NOTE_AS1	58.2705
#define NOTE_B1		61.7354

#define NOTE_C2		(NOTE_C1*2)
#define NOTE_CS2	(NOTE_CS1*2)
#define NOTE_D2		(NOTE_D1*2)
#define NOTE_DS2	(NOTE_DS1*2)
#define NOTE_E2		(NOTE_E1*2)
#define NOTE_F2		(NOTE_F1*2)
#define NOTE_FS2	(NOTE_FS1*2)
#define NOTE_G2		(NOTE_G1*2)
#define NOTE_GS2	(NOTE_GS1*2)
#define NOTE_A2		(NOTE_A1*2)
#define NOTE_AS2	(NOTE_AS1*2)
#define NOTE_B2		(NOTE_B1*2)

#define NOTE_C3		(NOTE_C2*2)
#define NOTE_CS3	(NOTE_CS2*2)
#define NOTE_D3		(NOTE_D2*2)
#define NOTE_DS3	(NOTE_DS2*2)
#define NOTE_E3		(NOTE_E2*2)
#define NOTE_F3		(NOTE_F2*2)
#define NOTE_FS3	(NOTE_FS2*2)
#define NOTE_G3		(NOTE_G2*2)
#define NOTE_GS3	(NOTE_GS2*2)
#define NOTE_A3		(NOTE_A2*2)
#define NOTE_AS3	(NOTE_AS2*2)
#define NOTE_B3		(NOTE_B2*2)

#define NOTE_C4		(NOTE_C3*2)
#define NOTE_CS4	(NOTE_CS3*2)
#define NOTE_D4		(NOTE_D3*2)
#define NOTE_DS4	(NOTE_DS3*2)
#define NOTE_E4		(NOTE_E3*2)
#define NOTE_F4		(NOTE_F3*2)
#define NOTE_FS4	(NOTE_FS3*2)
#define NOTE_G4		(NOTE_G3*2)
#define NOTE_GS4	(NOTE_GS3*2)
#define NOTE_A4		(NOTE_A3*2)
#define NOTE_AS4	(NOTE_AS3*2)
#define NOTE_B4		(NOTE_B3*2)

#define NOTE_C5		(NOTE_C4*2)
#define NOTE_CS5	(NOTE_CS4*2)
#define NOTE_D5		(NOTE_D4*2)
#define NOTE_DS5	(NOTE_DS4*2)
#define NOTE_E5		(NOTE_E4*2)
#define NOTE_F5		(NOTE_F4*2)
#define NOTE_FS5	(NOTE_FS4*2)
#define NOTE_G5		(NOTE_G4*2)
#define NOTE_GS5	(NOTE_GS4*2)
#define NOTE_A5		(NOTE_A4*2)
#define NOTE_AS5	(NOTE_AS4*2)
#define NOTE_B5		(NOTE_B4*2)

#define NOTE_C6		(NOTE_C5*2)
#define NOTE_CS6	(NOTE_CS5*2)
#define NOTE_D6		(NOTE_D5*2)
#define NOTE_DS6	(NOTE_DS5*2)
#define NOTE_E6		(NOTE_E5*2)
#define NOTE_F6		(NOTE_F5*2)
#define NOTE_FS6	(NOTE_FS5*2)
#define NOTE_G6		(NOTE_G5*2)
#define NOTE_GS6	(NOTE_GS5*2)
#define NOTE_A6		(NOTE_A5*2)
#define NOTE_AS6	(NOTE_AS5*2)
#define NOTE_B6		(NOTE_B5*2)

#define PWM_TRIS TRISBbits.RB1 //buzzer pin RB1
#define PWM_PIN  LATBbits.LATB1




void MakePWM(float freq, uint8_t duty, uint16_t ms);
void PlayMusic(void);
void PlayMusic1(void);

int music_signal = 0;

void __interrupt() buttom(void)             // High priority interrupt
{
    PlayMusic();
    PlayMusic1();
    /*
    if (music_signal == 0){
        music_signal = 1;
        PlayMusic();
    }
    else if (music_signal == 1){
        music_signal = 0;
        PlayMusic1();
    }  
     */
    INTCONbits.INT0IF = 0;
}

void main(void) {

    OSCCON = 0x72; //16MHz, Internal OSC
    RCONbits.IPEN = 1;
    INTCONbits.GIE = 1;
    INTCON = 0b11010000;
    PWM_TRIS = 0; //RB1 output
    PWM_PIN = 0; //RB1 low

    T1CONbits.RD16 = 1; //16bit mode    
    T1CONbits.TMR1CS = 0; //internal clock source, 16/4 = 4MHz
    T1CONbits.T1CKPS = 2; //1:4, 1MHz = 1us

    //PlayMusic(); //play notes
    //PlayMusic1();
    while (1) {
    }

    return;
}

void MakePWM(float freq, uint8_t duty, uint16_t ms) {

    if (freq != NOTE_REST) {
        uint16_t usPeriod = 1 / freq * 1000000;
        uint16_t usHighPeriod = (uint32_t) usPeriod * (uint32_t) duty / 100;
        uint16_t usLowPeriod = usPeriod - usHighPeriod;

        int loopNum = freq * ms / 1000;

        for (int i = 0; i < loopNum; i++) {
            PWM_PIN = 1; //pwm high
            TMR1 = 0;
            T1CONbits.TMR1ON = 1;
            while (TMR1 < usHighPeriod); //1us TMR1 counter

            PWM_PIN = 0; //pwm low
            TMR1 = 0;
            while (TMR1 < usLowPeriod); ////1us TMR1 counter
            T1CONbits.TMR1ON = 0;
        }
    } else {
        PWM_PIN = 0; //PB1 low
        TMR1 = 0;
        T1CONbits.TMR1ON = 1; //TMR1 on

        for (int i = 0; i < ms; i++) { //wait (ms) millis
            while (TMR1 < 1000);
            TMR1 = 0;
        }

        T1CONbits.TMR1ON = 0; //TMR1 off        
    }
}

void PlayMusic(void) {
    /* C:?, D:?, E:?, F:?, G:?, A:?, B:? */
    static uint16_t melody[] = {
        NOTE_D5,
        NOTE_D5, NOTE_E5, NOTE_D5, NOTE_B4, NOTE_G4, NOTE_B4, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_B4, 
        NOTE_G5, NOTE_E5, NOTE_D5, NOTE_B4, NOTE_G5, 
        NOTE_G5, NOTE_A5, NOTE_G5, NOTE_E5, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_E5,
        NOTE_C6, NOTE_A5, NOTE_G5, NOTE_E5,
        NOTE_G5, NOTE_E5, NOTE_D5, NOTE_B4,
        NOTE_G5, NOTE_E5, NOTE_D5, NOTE_B4,
        NOTE_G5, NOTE_E5, NOTE_D5, NOTE_B4,
        NOTE_D5, NOTE_E5, NOTE_F5,
        NOTE_G5,
    };

    static uint8_t duration[] = {
        16,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 8,
        8, 8, 8, 16, 16, 
        16, 16, 16, 16, 16, 16, 16, 16, 16, 8,
        8, 8, 8, 8,
        8, 8, 8, 8,
        8, 8, 8, 8,
        8, 8, 8, 8,
        8, 4, 8,
        1,
    };

    for (int i = 0; i < sizeof (duration); i++) {
        uint16_t noteDuration = 2000 / duration[i];
        MakePWM(melody[i], 50, noteDuration);
    }
}

void PlayMusic1(void) {
    /* C:?, D:?, E:?, F:?, G:?, A:?, B:? */
    static uint16_t melody[] = {
        NOTE_G5, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_B5, NOTE_G5, NOTE_D5, NOTE_G5,
    };

    static uint8_t duration[] = {
        16, 16, 16, 8, 16, 8, 16, 16, 
    };

    for (int i = 0; i < sizeof (duration); i++) {
        uint16_t noteDuration = 2000 / duration[i];
        MakePWM(melody[i], 50, noteDuration);
    }
}

//16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
// NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS6, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_AS5, NOTE_B5, NOTE_REST,
//NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_REST,
