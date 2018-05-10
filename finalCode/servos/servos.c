/*
** This microcontoller controls the start and stop conditions of our .
** This microcontroller requires 4 break beam sensors, 1 coin acceptor, 1 button, and 2 servos
** To singal to the other microcontrollers that the game is on, this microcontroller sets 2 pins high (one for each side of the game)
*/

#include "elecanisms.h"
#define SERVO_MIN_WIDTH     900e-6
#define SERVO_MAX_WIDTH     2.1e-3

int16_t main(void) {
    init_elecanisms();
    WORD32 servo_temp;
    uint16_t servo_multiplier, servo_offset, servoStartMagnet, servoStartRocket, a0_analog, a1_analog, potRange, servoRange, range, a1_old, a0_old, gameOn, servoValue; 
    uint8_t *RPOR, *RPINR;

    gameOn = 0; 

    servo_offset = (uint16_t)(FCY * SERVO_MIN_WIDTH);
    servo_multiplier = (uint16_t)(FCY * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH));
    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;

    gameOn = 0; 

    //Ball Pass Code
    potRange = 1023;
    servoRange = 65535;
    range = servoRange/potRange - 1;
    a0_analog = 1; 
    a1_analog = 1; 
    a0_old = 0;
    a1_old = 0; 

    T1CON = 0x0010;     // configure Timer1 to have a period of 20ms
    PR1 = 0x9C3F;
    TMR1 = 0;
    T1CONbits.TON = 1;

    D7_DIR = OUT; //rocketPass servo
    __asm__("nop");
     __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D7_RP] = OC1_RP;  // connect the OC1 module output to pin D10
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC1CON1 = 0x1C0F;
    OC1CON2 = 0x008B;
    servoStartRocket = 22768;
    OC1RS = servo_offset + servoStartRocket*servo_multiplier;
    OC1R = 1;
    OC1TMR = 0;
    
    // A0_AN_DIR = IN;
    // __asm__("nop");
    // A1_AN_DIR = IN; 
    //  __asm__("nop");


    D8_DIR = OUT; //magnetPass servo
    __asm__("nop");
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D8_RP] = OC2_RP;  // connect the OC1 module output to pin D10
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC2CON1 = 0x1C0F;
    OC2CON2 = 0x008B;
    servoStartMagnet = 22768;  
    OC2RS = servo_offset + servoStartMagnet*servo_multiplier;
    OC2R = 1;
    OC2TMR = 0;

    A0_DIR = IN;
    A1_DIR = IN;

    D12_DIR = IN; //right in
    D13_DIR = IN; //left in

    D12 = 0;
    D13 = 0;

    T2CON = 0x0030;         // set Timer1 period to 0.5s
    PR2 = 0x7A11;
    TMR2 = 0;               // set Timer1 count to 0
    IFS0bits.T2IF = 0;      // lower Timer1 interrupt flag
    T2CONbits.TON = 1;      // turn on Timer1

    



	while(1) {
        a0_analog = read_analog(A0_AN);
        a1_analog = read_analog(A1_AN);
        OC1RS = a0_analog*range*servo_multiplier + servo_offset;
        OC2RS = a1_analog*range*servo_multiplier + servo_offset;

        servoValue = a0_analog*range;
        servo_temp.ul = (uint32_t)servoValue * (uint32_t)servo_multiplier; 
        OC1RS = servo_temp.w[1] + servo_offset;

        // if(D12 && D13){
        //     gameOn = 1; 
        // }
        // else if (!D12 && !D13){
        //     gameOn = 0; 
        // }

        // if(D12 && gameOn){

        //     a0_analog = read_analog(A0_AN);
        //     a1_analog = read_analog(A1_AN);

        //     if(IFS0bits.T2IF){
        //         IFS0bits.T2IF = 0;
        //         if(a0_analog - a0_old < 3){
        //             OC1RS = a0_analog*range*servo_multiplier + servo_offset;
        //             LED1 = 1; 
        //         }
        //         else if(a0_old - a0_analog < 3){
        //             OC1RS = a0_analog*range*servo_multiplier + servo_offset;
        //             LED1 = 1; }
        //         else{
        //             LED1 = 0; 
        //         }

        //         if(a1_analog - a1_old < 3){
        //             OC2RS = a1_analog*range*servo_multiplier + servo_offset;
        //             LED2 = 1; 
        //         }
        //         else if(a1_old - a1_analog < 3){
        //             OC2RS = a0_analog*range*servo_multiplier + servo_offset;
        //             LED2 = 1; }
        //         else{
        //             LED2 = 0; 
        //         }

        //         a0_old = a0_analog;
        //         a1_old = a1_analog;
        //     } 
        // }

        // if(a0_old - a0_analog > 3){
        //     a0_old = a0_analog;
        //     OC1RS = a0_analog*range*servo_multiplier + servo_offset;}
        // else if(a0_analog - a0_old > 3){
        //     a0_old = a0_analog;
        //     OC1RS = a0_analog*range*servo_multiplier + servo_offset;}

        // if(a1_old - a1_analog > 3){
        //     a1_old = a1_analog;
        //     OC2RS = a1_analog*range*servo_multiplier + servo_offset;}
        // else if(a1_analog - a1_old > 3){
        //     a1_old = a1_analog;
        //     OC2RS = a0_analog*range*servo_multiplier + servo_offset;}


            // if(a0_analog > a0_old > 30){
            //    OC3RS = a0_analog*range*servo_multiplier + servo_offset; //magnet Pass
            //    a0_old = a0_analog; 
            // }
            // else if(a0_old - a0_analog > 30){
            //     OC3RS = a0_analog*range*servo_multiplier + servo_offset; //magnet Pass
            //     a0_old = a0_analog; 
            // }

            // if(a1_analog - a1_old > 30){
            //    OC2RS = a1_analog*range*servo_multiplier + servo_offset; //rocket Pass
            //    a1_old = a1_analog; 
            // }
            // else if(a1_old - a1_analog > 30){
            //     OC2RS = a1_analog*range*servo_multiplier + servo_offset; //rocket Pass
            //     a1_old = a1_analog; 
            // }
            


	}
}
