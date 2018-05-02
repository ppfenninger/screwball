#include "elecanisms.h"

int16_t main(void) {
    init_elecanisms();

    D1_DIR = OUT;
    __asm__("nop");
    D2_DIR = OUT;
    __asm__("nop");

    D1 = 0; 
    D2 = 0; 
    D12 = 0; 

	while(1) {



		if (D12){
			D1 = 1; 
			D2 = 1; 
		}
		else{
			D1 = 0;
			D2 = 0; 
		}
	}
}