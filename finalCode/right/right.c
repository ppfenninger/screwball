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
    WORD32 servo_temp1;
    uint16_t servo_multiplier, servo_offset, servoLowRocket, servoCurrentSwish, servoLowSwish, servoHighSwish, servoStartMoon, OCRvaluePopper, OCRvalueSwish, goingUP, gameOn, direction, servoValue1, potRange, servoRange, range, a0_analog; 
    uint8_t *RPOR, *RPINR;

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

    goingUP = 1;
    T2CON = 0x0030;         // set Timer1 period to 0.5s
    PR2 = 0x1869;

    TMR2 = 0;               // set Timer1 count to 0
    IFS0bits.T2IF = 0;      // lower Timer1 interrupt flag
    T2CONbits.TON = 1;      // turn on Timer1  


    //Popper Code 
    D5_DIR = IN; //popper switch
    D5 = 0; 

    D6_DIR = OUT; //popper solenoid
    __asm__("nop");     
    D6 = 0;          
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D6_RP] = OC1_RP;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC1CON1 = 0x1C06;  
    OC1CON2 = 0x001F;   
    OC1RS = (uint16_t)(FCY / 1e4 - 1.);
    OCRvaluePopper = 5*OC1RS/100; 
    OC1R = 0; 
    OC1TMR = 0; 


    // Swishel Swashel Code
    direction = 0; 
    D7_DIR = IN; //swishel swashel rotate stop button
    D7 = 0;
    D8_DIR = IN; //swishel swashel pop button
    D8 = 0; 

    D9_DIR = OUT; //swishel swashel servo
    __asm__("nop");
     __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D9_RP] = OC2_RP;  // connect the OC1 module output to pin D10
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC2CON1 = 0x1C0F;
    OC2CON2 = 0x008B;
    servoLowSwish = 22768;
    servoHighSwish = 32768;
    servoCurrentSwish = 22768; 
    OC2RS = servo_offset + servoCurrentSwish*servo_multiplier;
    OC2R = 1;
    OC2TMR = 0;

    //swishel swashel solenoid
    D10_DIR = OUT;
    __asm__("nop");
    D10 = 0; 
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D10_RP] = OC3_RP;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC3CON1 = 0x1C06;  
    OC3CON2 = 0x001F;   
    OC3RS = (uint16_t)(FCY / 1e4 - 1.);
    OCRvalueSwish = 75*OC3RS/100; 
    OC3R = 0; 
    OC3TMR = 0;  

    //Set up timer for servo - Timer 3
    T3CON = 0x0030;         
    PR3 = 0x1770;
    TMR3 = 0;               // set initial timer count to 0
    IFS0bits.T3IF = 0;      // lower Timer3 interrupt flag
    T3CONbits.TON = 1;      // turn on Timer3


    //CatchMoon - code
	D11_DIR = OUT;
    __asm__("nop");

    potRange = 1023; 
    servoRange = 33000;
    range = servoRange/potRange;

    servoStartMoon = 22768;
     __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D11_RP] = OC4_RP;  // connect the OC1 module output to pin D10
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC4CON1 = 0x1C0F;
    OC4CON2 = 0x008B;
    OC4RS = servo_offset + servoStartMoon*servo_multiplier;
    OC4R = 1;
    OC4TMR = 0;
    T1CON = 0x0010;     // configure Timer1 to have a period of 20ms
    PR1 = 0x9C3F;
    TMR1 = 0;
    T1CONbits.TON = 1;

    D12_DIR = IN; //right in
    D13_DIR = IN; //left in

    D12 = 0;
    D13 = 0;

    gameOn = 0; 


	while(1) {
		if(D12 && D13){
			gameOn = 1;}
		else if(!D12 && !D13){
			gameOn = 0;
            OC2RS = servo_offset + servoLowSwish*servo_multiplier;
            D0 = 0;
            __asm__("nop");
            D1 = 0;
            __asm__("nop"); 
            D2 = 0;
            __asm__("nop");
            D3 = 0;
            __asm__("nop");
            D4 = 0;
            __asm__("nop");} 

		if(gameOn && D13){ //add d12
		//popper code
			if (D5){
				if(D4){
					OC1R = 12*OCRvaluePopper;}
				else if(D3){
					OC1R = 11*OCRvaluePopper;}
				else if(D2){
					OC1R = 10*OCRvaluePopper;}
				else if(D1) {
					OC1R = 9*OCRvaluePopper;}
				else if(D0){
					OC1R = 8*OCRvaluePopper;}
				else{
					OC1R = 7*OCRvaluePopper;}}
			else {
				OC1R = 0;
                LED1 = 1; 
				if(IFS0bits.T2IF){
					IFS0bits.T2IF = 0;
					if (goingUP){
						if(D4){
							goingUP = 0;
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
							D4 = 0;}}}}
            //Swishel Swashel Code
            if(D7){
                OC2RS = servo_offset + servoCurrentSwish*servo_multiplier;}
            else if(servoCurrentSwish > servoLowSwish && IFS0bits.T3IF && !direction){
                servoCurrentSwish = servoCurrentSwish - 10; 
                OC2RS = servo_offset + servoCurrentSwish*servo_multiplier;
                IFS0bits.T3IF = 0; }
            else if(servoCurrentSwish < servoHighSwish && IFS0bits.T3IF && direction){
                servoCurrentSwish = servoCurrentSwish + 10;
                OC2RS = servo_offset + servoCurrentSwish*servo_multiplier;
                IFS0bits.T3IF = 0; }
            else if(servoCurrentSwish <= servoLowSwish && IFS0bits.T3IF){
                servoCurrentSwish = servoCurrentSwish + 10;
                OC2RS = servo_offset + servoCurrentSwish*servo_multiplier;
                direction = 1;
                IFS0bits.T3IF = 0; }
            else if(servoCurrentSwish >= servoHighSwish && IFS0bits.T3IF){
                servoCurrentSwish = servoCurrentSwish - 10;
                OC2RS = servo_offset + servoCurrentSwish*servo_multiplier;
                direction = 0;
                IFS0bits.T3IF = 0;}

            if(D8){
                OC3R = OCRvalueSwish;
            }
            else{
                OC3R = 0; 
            }}
		if(gameOn && D12){
			a0_analog = read_analog(A0_AN);

            servoValue1 = a0_analog*range;
            servo_temp1.ul = (uint32_t)servoValue1 * (uint32_t)servo_multiplier; 
            OC4RS = servo_temp1.w[1] + servo_offset;

		}
	}
}