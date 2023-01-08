#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
//#pragma config BOREN = ON       // Brown-out Reset Enable bit
//#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)
#define _XTAL_FREQ 4000000
int player1=0;
int player2=0;

void UART_Write(unsigned char data)  // Output on Terminal
{
    while(!TXSTAbits.TRMT);
    TXREG = data;              //write to TXREG will send data 
}

void check(){
    if(player1>player2){
        if(CCPR1L<0x11)
            CCPR1L++;
        }

    else if(player2>player1){
        if(CCPR1L>0x07)
            CCPR1L--;
    }
}

void MyusartRead()
{
    unsigned char data;
    data=RCREG;
    UART_Write(data);
    if(data=='s'){
        player1++;
    }
        
    else if(data == 'k'){
        player2++;
    }
    return;
}



void servo_motor_setups(){
    //setups for interrupt
    ADCON1 = 0x0F;
    INTCONbits.INT0IE = 0;//Disable external interrupt
    INTCONbits.INT0IF = 0;//Clear external interrupt flag
    INTCONbits.GIE = 0;//Disable global interrupt
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;
    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 us
    OSCCONbits.IRCF = 0b001; 
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    // RD7/P1D, RD5/P1B -> Output
    TRISD = 0;
    LATD = 0;
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    PR2 = 0x9b;
    //reset 2 motors
   // CCP1CONbits.P1M = 0b01;//P1D
    //__delay_ms(1000);
   // CCPR1L = 2400/32/4;
   // CCP1CONbits.DC1B = (2400/32)%4;
    //__delay_ms(1000);
    
    CCP1CONbits.P1M = 0b11;//P1B
    //__delay_ms(1000);
    CCPR1L = 1450/32/4;
    CCP1CONbits.DC1B = (1450/32)%4;
    //__delay_ms(1000);
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
    while(INTCONbits.TMR0IF != 1){
        while(!PIR1bits.RCIF) ;
        MyusartRead();
        check();
        PIR1bits.RCIF = 0;
    }
    T0CONbits.TMR0ON = 0;//stop timer0
    /*
    for(int j = 1450; j <= 2400; j+=100){//2400
        CCPR1L = j/32/4;
        CCP1CONbits.DC1B = (j/32)%4;
    }
     */
    CCPR2L = 500/32/4;
    CCP2CONbits.DC2B = (500/32)%4;
}


void main(void) 
{
    
//uart
    TRISCbits.TRISC6 = 1;            
    TRISCbits.TRISC7 = 1;            
    
    //  Setting baud rate = 150
    TXSTAbits.SYNC = 0;           
    BAUDCONbits.BRG16 = 0;          
    TXSTAbits.BRGH = 1;
    SPBRG = 51;      
    
   //   Serial enable
    RCSTAbits.SPEN = 1;              
    PIR1bits.TXIF = 0;
    PIR1bits.RCIF = 0;
    TXSTAbits.TXEN = 1;           
    RCSTAbits.CREN = 1;             
    PIE1bits.TXIE = 0;       
    IPR1bits.TXIP = 0;             
    PIE1bits.RCIE = 0;              
    IPR1bits.RCIP = 0;   
    
//server
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 탎
    OSCCONbits.IRCF = 0b001;
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    //ENABLE INTERUPT
    RCONbits.IPEN=0;
    INTCONbits.INT0IE=1;
    INTCONbits.INT0IF=0;
    INTCONbits.GIE=1;
    
    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /** 
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9b + 1) * 4 * 8탎 * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9b;
    
    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (0x03*4 + 0b11) * 8탎 * 4
     * ~= 500탎
     */
    
    CCPR1L = 0x0c;
    CCP1CONbits.DC1B = 0b00;
    servo_motor_setups();
    timer0_setups();
    servo_start();
    while(1){
        //while(!PIR1bits.RCIF) ;
        //MyusartRead();
        //check();
        //PIR1bits.RCIF = 0;
    }
    return;
}
