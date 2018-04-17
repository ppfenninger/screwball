#include "elecanisms.h"

int16_t main(void) {
    init_elecanisms();
    uint16_t OCRvalue;
    uint8_t goingUP, sum, *RPOR, *RPINR;

    D4_DIR = OUT;
    D5_DIR = OUT;
    D6_DIR = OUT;
    D3_DIR = OUT;
    D8_DIR = OUT;
    goingUP = 1; 
    D4 = 0;
    D5 = 0;
    D6 = 0;
    D3 = 0;
    D8 = 0;

    T1CON = 0x0030;         // set Timer1 period to 0.5s
    PR1 = 0x1869;

    TMR1 = 0;               // set Timer1 count to 0
    IFS0bits.T1IF = 0;      // lower Timer1 interrupt flag
    T1CONbits.TON = 1;      // turn on Timer1

   	// initialize the motor
   	D1_DIR = OUT;      // configure D1 to be a digital output
    D1 = 0;            // set D1 low
    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D1_RP] = OC1_RP;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    OC1CON1 = 0x1C06;   // configure OC1 module to use the peripheral clock (i.e., FCY, OCTSEL<2:0> = 0b111) and  to operate in edge-aligned PWM mode (OCM<2:0> = 0b110)
    OC1CON2 = 0x001F;   // configure OC1 module to syncrhonize to itself (i.e., OCTRIG = 0 and SYNCSEL<4:0> = 0b11111)

    OC1RS = (uint16_t)(FCY / 1e4 - 1.);     // configure period register to get a frequency of 1kHz

    OCRvalue = 10*OC1RS/100;  // configure duty cycle to 1% (i.e., period / 10)r
    OC1R = 0;               //both are stopped
    OC1TMR = 0;         // set OC1 timer count to 

	while(1) {
		if (D0 == 1){
			LED1 = 1; 
			if(D8){
				OC1R = 10*OCRvalue;
			}
			else if(D3){
				OC1R = 9*OCRvalue;
			}
			else if(D6){
				OC1R = 8*OCRvalue;
			}
			else if(D5) {
				OC1R = 7*OCRvalue;
			}
			else if(D4){
				OC1R = 6*OCRvalue;
			}
			else{
				OC1R = 5*OCRvalue; 
			}
			
		}
		else {
			OC1R = 0;
			LED1 = 0;  
			if(IFS0bits.T1IF){
				IFS0bits.T1IF = 0;
				if (goingUP){
					if(D8){
						goingUP = 0;
						LED1 = 1;
						LED2 = 0;
					}
					else if(D3){
						D8 = 1; 
					}
					else if(D6){
						D3 = 1;
					}
					else if(D5) {
						D6 = 1;
					}
					else if(D4){
						D5 = 1;
					}
					else{
						D4 = 1; 
					}

				}
				else if(goingUP == 0){
					
					if(D4 == 0){
						goingUP = 1;  
						LED2 = 1;
						LED1 = 0;
					}
					else if(D5 == 0){
						D4 = 0; 
					}
					else if(D6 == 0){
						D5 = 0; 
					}
					else if(D3 == 0){
						D6 = 0;
					}
					else if(D8 == 0) {
						D3 = 0;;
					}
					else{
						D8 = 0;
					}
				}
			}
		}
	}
}