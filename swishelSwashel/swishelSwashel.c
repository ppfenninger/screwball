#include "elecanisms.h"

#define SERVO_MIN_WIDTH     900e-6
#define SERVO_MAX_WIDTH     2.1e-3

int16_t main(void) {
    init_elecanisms();
    uint8_t *RPOR, *RPINR;
    uint16_t direction, servoHigh, servoLow, servo_offset, servo_multiplier, buttonUp;
    
    //Set up the servo - Pin D0
    servo_offset = (uint16_t)(FCY * SERVO_MIN_WIDTH);
    servo_multiplier = (uint16_t)(FCY * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH));

    servoHigh = 42000;
    servoLow = 22000;
    direction = 1; 

    D0_DIR = OUT;      // configure D13 to be a digital output
    D0 = 0;            // set D13 low

    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;

    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D0_RP] = OC1_RP;  // connect the OC1 module output to pin D10
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC1CON1 = 0x1C0F;
    OC1CON2 = 0x008B;

    OC1RS = servo_offset + servoLow*servo_multiplier;
    OC1R = 1;
    OC1TMR = 0;

    T1CON = 0x0010;     // configure Timer1 to have a period of 20ms
    PR1 = 0x9C3F;
    TMR1 = 0;
    T1CONbits.TON = 1;


    //Set up timer for servo - Timer 2
    T2CON = 0X0030;         // set Timer2 period of 0.5 seconds
    PR2 = 0x1E847;
    TMR2 = 0;               // set initial timer count to 0
    IFS0bits.T2IF = 0;      // lower Timer2 interrupt flag
    T2CONbits.TON = 1;      // turn on Timer2

    //Button setup 
    D1 = 0; //starts low

    //Set up timer for button - Timer 3
    T3CON = 0X0030;         // set Timer2 period of 0.5 seconds
    PR3 = 0x1E847;
    TMR3 = 0;               // set initial timer count to 0
    IFS0bits.T3IF = 0;      // lower Timer2 interrupt flag
    T3CONbits.TON = 1;      // turn on Timer2\

    //Set up Solenoid
    D2 = 0; // solenoid input

    T4CON = 0x0030;         
    PR4 = 0x1869;

    TMR4 = 0;               // set Timer1 count to 0
    IFS0bits.T4IF = 0;      // lower Timer1 interrupt flag
    T4CONbits.TON = 1;      // turn on Timer1

    // initialize the motor
    D3_DIR = OUT;      // configure D1 to be a digital output
    D3 = 0;            // set D1 low
    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D3_RP] = OC3_RP;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC4CON1 = 0x1C06;   // configure OC1 module to use the peripheral clock (i.e., FCY, OCTSEL<2:0> = 0b111) and  to operate in edge-aligned PWM mode (OCM<2:0> = 0b110)
    OC4CON2 = 0x001F;   // configure OC1 module to syncrhonize to itself (i.e., OCTRIG = 0 and SYNCSEL<4:0> = 0b11111)

    OC4RS = (uint16_t)(FCY / 1e4 - 1.);     // configure period register to get a frequency of 1kHz

    OCRvalue = 10*OC4RS/100;  // configure duty cycle to 1% (i.e., period / 10)r
    OC4R = 0;               //both are stopped
    OC4TMR = 0;         // set OC1 timer count to

    while(1) {

        if(D1 && buttonUp && IFS0bits.T3IF){
            IFS0bits.T3IF = 0;
            OC1RS = 0;
            buttonUp = 0;
            OC1RS = 0;
            LED1 = 1;
        }
        else if(direction && IFS0bits.T2IF && IFS0bits.T3IF){
            direction = 0;
            OC1RS = servo_offset + servoHigh*servo_multiplier;
            IFS0bits.T2IF = 0; 
            LED1 = 0;
        }
        else if(IFS0bits.T2IF && IFS0bits.T3IF){
            direction = 1; 
            OC1RS = servo_offset + servoLow*servo_multiplier;
            IFS0bits.T2IF = 0; 
            LED1 = 0; 
        }

        if(!D1){
            buttonUp = 1; 
        }

        if(D2){
            OC4RS = 6*OCRvalue;
        }

    }
}