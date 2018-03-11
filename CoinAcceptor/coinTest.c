#include "elecanisms.h"

int16_t main(void) {
    init_elecanisms();
    uint8_t
    uint8_t goneUp;

    LED1 = 0;
    goneUp = 1;

	while(1) {
		if (D0 == 0 && goneUp == 1){
			LED1 = !LED1; 
			goneUp = 0; 
		}
		else if (D0 == 1){
			goneUp = 1;
		}
	}
}