#include "elecanisms.h"
#include <stdlib.h>

#define SERVO_MIN_WIDTH     900e-6
#define SERVO_MAX_WIDTH     2.1e-3

int16_t main(void) {
    init_elecanisms();
    uint8_t *RPOR, *RPINR;
    uint16_t servo_offset, servo_multiplier, servo1, servo2, ison;
    WORD32 servo_temp;

    LED1 = 0;

    servo_offset = (uint16_t)(FCY * SERVO_MIN_WIDTH);
    servo_multiplier = (uint16_t)(FCY * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH));
    servo1 = 32568;
    servo2 = 32788;

    // Configure pin D13 and D12 to produce hobby servo control signals
    // using the OC1 and OC2 modules, respectively.
    D13_DIR = OUT;      // configure D13 to be a digital output
    D13 = 0;            // set D13 low

    D12_DIR = OUT;      // configure D12 to be a digital output
    D12 = 0;            // set D12 low

    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;

    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D13_RP] = OC1_RP;  // connect the OC1 module output to pin D13
    RPOR[D12_RP] = OC2_RP;  // connect the OC2 module output to pin D12
    __builtin_write_OSCCONL(OSCCON | 0x40);

    OC1CON1 = 0x1C0F;   // configure OC1 module to use the peripheral
                        //   clock (i.e., FCY, OCTSEL<2:0> = 0b111),
                        //   TRIGSTAT = 1, and to operate in center-aligned 
                        //   PWM mode (OCM<2:0> = 0b111)
    OC1CON2 = 0x008B;   // configure OC1 module to trigger from Timer1
                        //   (OCTRIG = 1 and SYNCSEL<4:0> = 0b01011)

    // set OC1 pulse width to 1.5ms (i.e. halfway between 0.9ms and 2.1ms)
    servo_temp.ul = 0x8000 * (uint32_t)servo_multiplier;
    OC1RS = servo_offset + servo1*servo_multiplier;
    OC1R = 1;
    OC1TMR = 0;

    OC2CON1 = 0x1C0F;   // configure OC2 module to use the peripheral
                        //   clock (i.e., FCY, OCTSEL<2:0> = 0b111),
                        //   TRIGSTAT = 1, and to operate in center-aligned 
                        //   PWM mode (OCM<2:0> = 0b111)
    OC2CON2 = 0x008B;   // configure OC2 module to trigger from Timer1
                        //   (OCTRIG = 1 and SYNCSEL<4:0> = 0b01011)

    // set OC2 pulse width to 1.5ms (i.e. halfway between 0.9ms and 2.1ms)
    servo_temp.ul = 0x8000 * (uint32_t)servo_multiplier;
    OC2RS = servo_offset + servo2*servo_multiplier;
    OC2R = 1;
    OC2TMR = 0;

    T1CON = 0x0010;     // configure Timer1 to have a period of 20ms
    PR1 = 0x9C3F;

    TMR1 = 0;
    T1CONbits.TON = 1;
  

	while(1) {
		if (D0 == 0){
			LED1 = 1; 
			ison = 1; 
		}
		while(ison){
			if (D1 == 1 && servo1 < 32768){
				servo1 = servo1 + 1; 
				LED2 = 1; 
			}
			else if (D2 == 1 && servo1 > 32368){
				servo1 = servo1 - 1;
				LED2 = 0; 
			}

			if (D3 == 1 && servo2 < 32988){
				servo2 = servo + 1;
				LED3 = 1;
			}
			else if (D4 == 1 && servo2 > 32588){
				servo2 = servo - 1;
				LED3 = 0; 
			}
			OC1RS = servo1*servo_multiplier + servo_offset;
			OC2RS = servo2*servo_multiplier + servo_offset;
		}
	}
}