#include "elecanisms.h"

int16_t main(void) {
    init_elecanisms();

    LED1 = 0;
    D3_DIR = OUT;

	while(1) {
		if (SW1 == 0){
			D3 = 1;  
			LED1 = 1;
		}
		else{
			D3 = 0;
			LED1 = 0;
		}
	}
}