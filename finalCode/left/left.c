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
    uint16_t servo_multiplier, servo_offset, servoLowRocket, servoStartMagnet, servoStartRocket, OCRvalueLever, OCRvalueWeeble, goingUP, gameOn, a0_analog, a1_analog, potRange, servoRange, range, a1_old, a0_old; 
    uint8_t *RPOR, *RPINR;

    gameOn = 0; 

    servo_offset = (uint16_t)(FCY * SERVO_MIN_WIDTH);
    servo_multiplier = (uint16_t)(FCY * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH));
    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;

    D0_DIR = OUT;
    __asm__("nop");
    D1_DIR = OUT;
    __asm__("nop");
    D2_DIR = OUT;
    __asm__("nop");
    D3_DIR = OUT;
    __asm__("nop");
    D4_DIR = OUT;
    __asm__("nop");

    D0 = 0;
    __asm__("nop");
    D1 = 0;
    __asm__("nop");
    D2 = 0;
    __asm__("nop");
    D3 = 0;
    __asm__("nop");
    D4 = 0; 
    __asm__("nop");


    D5_DIR = IN; //weeble wobble switch
    D5 = 0; 

    D6_DIR = OUT; //weeble wooble solenoid
    __asm__("nop");
    D6_DIR = OUT;      // configure D1 to be a digital output
    D6 = 0;            // set D1 low
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D6_RP] = OC1_RP;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC1CON1 = 0x1C06;  
    OC1CON2 = 0x001F;   

    OC1RS = (uint16_t)(FCY / 1e4 - 1.);

    OCRvalueWeeble = 15*OC1RS/100; 
    OC1R = 0; 
    OC1TMR = 0;  

    //Ball Pass Code
    potRange = 1023;
    servoRange = 65535;
    range = servoRange/potRange;
    a0_analog = 1; 
    a1_analog = 1; 
    a0_old = 0;
    a1_old = 0; 


    D7_DIR = OUT; //rocketPass servo
    __asm__("nop");
     __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D7_RP] = OC2_RP;  // connect the OC1 module output to pin D10
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC2CON1 = 0x1C0F;
    OC2CON2 = 0x008B;
    servoStartRocket = 22768;
    OC2RS = servo_offset + servoStartRocket*servo_multiplier;
    OC2R = 1;
    OC2TMR = 0;
    T2CON = 0x0010;     // configure Timer1 to have a period of 20ms
    PR2 = 0x9C3F;
    TMR2 = 0;
    T2CONbits.TON = 1;
    


    D8_DIR = OUT; //magnetPass servo
    __asm__("nop");
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D8_RP] = OC3_RP;  // connect the OC1 module output to pin D10
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC3CON1 = 0x1C0F;
    OC3CON2 = 0x008B;
    servoStartMagnet = 22768;  
    OC3RS = servo_offset + servoStartMagnet*servo_multiplier;
    OC3R = 1;
    OC3TMR = 0;
    T3CON = 0x0010;     // configure Timer3 to have a period of 20ms
    PR3 = 0x9C3F;
    TMR3 = 0;
    T3CONbits.TON = 1; 
    
    //Levertoss - code
    D9_DIR = IN; //levertoss switch
    D9 = 0; 
	D10_DIR = OUT; // lever toss solenoid

	T1CON = 0x0030;         // set Timer1 period to 0.5s
    PR1 = 0x1869;
	TMR1 = 0;               // set Timer1 count to 0
    IFS0bits.T1IF = 0;      // lower Timer1 interrupt flag
    T1CONbits.TON = 1;      // turn on Timer1

    __asm__("nop");
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D10_RP] = OC4_RP;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC4CON1 = 0x1C06;  
    OC4CON2 = 0x001F;   
    OC4RS = (uint16_t)(FCY / 1e4 - 1.);
    OCRvalueLever = 5*OC4RS/100; 
    OC4R = 0; 
    OC4TMR = 0;  

    D12_DIR = IN; //right in
    D13_DIR = IN; //left in

    D12 = 0;
    D13 = 0;

    



	while(1) {
		if(D12 && D13){
			gameOn = 1;}
		else if(!D12 && !D13){
			gameOn = 0;} //CHANGE BACK TO 0

		if(gameOn && D12){ //and D12
		//Lever toss code
			if (D9 == 1){
				if(D4){
					OC4R = 10*OCRvalueLever;}
				else if(D3){
					OC4R = 9*OCRvalueLever;}
				else if(D2){
					OC4R = 8*OCRvalueLever;}
				else if(D1) {
					OC4R = 7*OCRvalueLever;}
				else if(D0){
					OC4R = 6*OCRvalueLever;}
				else{
					OC4R = 5*OCRvalueLever;}}
			else {
				OC4R = 0;
				if(IFS0bits.T1IF){
					IFS0bits.T1IF = 0;
					if (goingUP){
						if(D4){
							goingUP = 0;
							LED1 = 1;
							LED2 = 0;}
						else if(D3){
							D4 = 1; }
						else if(D2){
							D3 = 1;}
						else if(D1) {
							D2 = 1;}
						else if(D0){
							D1 = 1;}
						else{
							D0 = 1; }}
					else if(goingUP == 0){
						if(D0 == 0){
							goingUP = 1;  
							LED2 = 1;
							LED1 = 0;}
						else if(D1 == 0){
							D0 = 0;}
						else if(D2 == 0){
							D1 = 0;}
						else if(D3 == 0){
							D2 = 0;}
						else if(D4 == 0) {
							D3 = 0;}
						else{
							D4 = 0;}}}}}
		if(gameOn && D13){ //Add D13
			//weeble-Wobble Code
			if(D5){
				OC1R = OCRvalueWeeble;}
			else{
				OC1R = 0;}

            a0_analog = read_analog(A0_AN);
            a1_analog = read_analog(A1_AN);

            
            if(a0_analog > 500){
                LED1 = 1;
                LED2 = 0;}
            else{
                LED1 = 0;
                LED2 = 1;}

            if(a0_analog > a0_old > 30){
               OC3RS = a0_analog*range*servo_multiplier + servo_offset; //magnet Pass
               a0_old = a0_analog; 
            }
            else if(a0_old - a0_analog > 30){
                OC3RS = a0_analog*range*servo_multiplier + servo_offset; //magnet Pass
                a0_old = a0_analog; 
            }

            if(a1_analog - a1_old > 30){
               OC2RS = a1_analog*range*servo_multiplier + servo_offset; //rocket Pass
               a1_old = a1_analog; 
            }
            else if(a1_old - a1_analog > 30){
                OC2RS = a1_analog*range*servo_multiplier + servo_offset; //rocket Pass
                a1_old = a1_analog; 
            }
            



		}
	}
}