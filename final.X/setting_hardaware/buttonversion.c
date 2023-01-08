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
#define HAND_INIT 500
#define HAND_END 1450
#define BASE_INIT 1450
#define player1hit 500
#define player2hit 2400
#define _XTAL_FREQ 4000000

int i, direction;
int A[3] = {0x04, 0x0b, 0x12};
int B[3] = {0b00, 0b01, 0b11};
int player1=0;
int player2=0;
void check();
void __interrupt() COMP_ISR(void){
    __delay_ms(10);
    INTCONbits.INT0IE = 0;
    INTCON3bits.INT1IE = 0;
    if(INTCONbits.INT0IF == 1){
        //LATD = 1;
        player1++;
    }
    if(INTCON3bits.INT1IF == 1){
        //LATD = 2;
        player2++;
    }
    check();
    /*
    for(int j = 500; j <= 2400; j+=100){//2400
        CCPR1L = j/32/4;
        CCP1CONbits.DC1B = (j/32)%4;
    }
     * */
    INTCONbits.INT0IE = 1;
    INTCON3bits.INT1IE = 1;
    INTCONbits.INT0IF = 0;//clear external interrupt flag
    INTCON3bits.INT1IF = 0;
}
void check(){
    if(player1>player2){
        if(CCPR1L<2200/32/4)
            CCPR1L++;
    }
    else if(player2>player1){
        if(CCPR1L>700/32/4)
            CCPR1L--;
    }
}
void servo_motor_setups(){
    //setups for interrupt
    ADCON1 = 0x0F;
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;
    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 us
    OSCCONbits.IRCF = 0b001; 
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    CCP2CONbits.CCP2M = 0b1100;
    // CCP1 -> Output
    TRISC = 0;
    LATC = 0x01;
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    PR2 = 0x9b;
    //reset 2 motors
    CCPR1L = BASE_INIT/32/4;
    CCP1CONbits.DC1B = (BASE_INIT/32)%4;
     
    CCPR2L = HAND_INIT/32/4;
    CCP2CONbits.DC2B = (HAND_INIT/32)%4;
}


void timer0_setups(){
    //Timer0 setups
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.T0SE = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS = 0b111;//1:256
    TMR0H = 0xF8;//15 sec
    TMR0L = 0xD8;
    INTCONbits.TMR0IF = 0;//clear timer flag
}

void servo_start(){
    T0CONbits.TMR0ON = 1;//start timer0
    LATC = 0;
    while(INTCONbits.TMR0IF != 1){
        LATD=0b0001;
        __delay_ms(5);
        LATD=0b0010;
        __delay_ms(5);
        LATD=0b0100;
        __delay_ms(5);
        LATD=0b1000;
        __delay_ms(5);
        
        LATD=0b0100;
        __delay_ms(5);
        LATD=0b0010;
        __delay_ms(5);
    }
    T0CONbits.TMR0ON = 0;//stop timer0
    INTCONbits.INT0IE = 0;
    INTCON3bits.INT1IE = 0;
    LATC = 1;
    /*
    for(int j = 1450; j <= 2400; j+=100){//2400
        CCPR1L = j/32/4;
        CCP1CONbits.DC1B = (j/32)%4;
    }
     */
    __delay_ms(10);
    int j = CCPR1L*4*32;
    if(player1>player2){
        while(j <= player2hit){//2400
            __delay_ms(1);
            CCPR1L = j/32/4;
            CCP1CONbits.DC1B = (j/32)%4;
            j += 100;
        }
    }
    else if(player1<player2){
        while(j >= player1hit){//2400
            __delay_ms(1);
            CCPR1L = j/32/4;
            CCP1CONbits.DC1B = (j/32)%4;
            j -= 100;
        }
    }
    else{
        player1+=player2;
        if(player1%2==1){
            while(j <= player2hit){//2400
                __delay_ms(1);
                CCPR1L = j/32/4;
                CCP1CONbits.DC1B = (j/32)%4;
                j += 100;
            }
        }
        else{
           while(j >= player1hit){//2400
               __delay_ms(1);
                CCPR1L = j/32/4;
                CCP1CONbits.DC1B = (j/32)%4;
                j -= 100;
            }
        }
    }
    TMR0H = 0xFF;//15 sec
    TMR0L = 0xC2;
    INTCONbits.TMR0IF = 0;//clear timer flag
    T0CONbits.TMR0ON = 1;//start timer0
    while(INTCONbits.TMR0IF != 1);
    T0CONbits.TMR0ON = 0;
    CCPR2L = HAND_END/32/4;
    CCP2CONbits.DC2B = (HAND_END/32)%4;
}
void main(void)
{
    //INTCONbits.GIE = 1;
    //INTCON3bits.INT2IE = 1;
    ADCON1 = 0x0F;
    INTCON3bits.INT2IF = 0;
    while(1){
        player1 = 0;
        player2 = 0;
        while(INTCON3bits.INT2IF != 1);
        INTCON3bits.INT2IE = 0;
        INTCON3bits.INT2IF = 0;
        i = 0;
        direction = 0;
        //setups for interrupt
        ADCON1 = 0x0F;
        INTCONbits.INT0IE = 1;//Enable external interrupt
        INTCONbits.INT0IF = 0;//Clear external interrupt flag
        INTCON3bits.INT1IE = 1;//Enable external interrupt
        INTCON3bits.INT1IF = 0;//Clear external interrupt flag
        INTCONbits.GIE = 1;//Enable global interrupt
        // Timer2 -> On, prescaler -> 4
        T2CONbits.TMR2ON = 0b1;
        T2CONbits.T2CKPS = 0b01;

        // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 ?
        OSCCONbits.IRCF = 0b001;

        // PWM mode, P1A, P1C active-high; P1B, P1D active-high
        CCP1CONbits.CCP1M = 0b1100;

        // CCP1/RC2 -> Output
        //TRISC = 0;
        //LATC = 0;
        TRISD = 0;
        LATD = 0;
        // Set up PR2, CCP to decide PWM period and Duty Cycle
        PR2 = 0x9b;
        //CCPR1L = A[0];
        //CCP1CONbits.DC1B = B[0];//(3*4+4)*8*4=512?

        servo_motor_setups();
        timer0_setups();
        servo_start();
        INTCON3bits.INT2IF = 0;
        while(INTCON3bits.INT2IF == 0){
            LATD=0b1111;
            __delay_ms(10);
            LATD=0b0000;
            __delay_ms(10);
        }
    }
    
    return;
}
#pragma interrupt COMP_ISR
