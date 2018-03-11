
#include "elecanisms.h"

int16_t main(void) {
    init_elecanisms();
    uint8_t goneUp;

    LED1 = 0;
    LED2 = 0;
    LED3 = 0;

	while(1) {
		if (D1 == 1){
			LED1 = 1; 
		}
		else{
			LED1 = 0;
		}
		if (D2 == 1){
			LED2 = 1; 
		}
		else{
			LED2 = 0;
		}
		if (D3 == 1){
			LED3 = 1; 
		}
		else{
			LED3 = 0;
		}
		if (D4 == 1){
			D0 = 1; 
		}
		else{
			D0 = 0;
		}
	}
}
