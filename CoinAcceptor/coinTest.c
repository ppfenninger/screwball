#include "elecanisms.h"

int16_t main(void) {
    init_elecanisms();
	D1_DIR = OUT; 
	__asm__("nop");
    D1_DIR = OUT;
    __asm__("nop");
    D2_DIR = OUT;
    __asm__("nop");
    D3_DIR = OUT;
    __asm__("nop");
    D4_DIR = OUT;
    __asm__("nop");
    D6_DIR = OUT;
    __asm__("nop");
    D7_DIR = OUT;
    __asm__("nop");
    D8_DIR = OUT;
    __asm__("nop");
    D9_DIR = OUT;
    __asm__("nop");
    D10_DIR = OUT;
    __asm__("nop");
    D11_DIR = OUT;
    __asm__("nop");
    D12_DIR = OUT;
    __asm__("nop");
    D13_DIR = OUT;
    __asm__("nop");


    D0 = 1; 
    __asm__("nop");
    D1 = 1; 
    __asm__("nop");
    D2 = 1; 
    __asm__("nop");
    D3 = 1; 
    __asm__("nop");
    D4 = 1;
    __asm__("nop");
    D5 = 1; 
    __asm__("nop");
    D6 = 1; 
    __asm__("nop");
    D7 = 1; 
    __asm__("nop");
    D8 = 1; 
    __asm__("nop");
    D9 = 1;
    __asm__("nop");
    D10 = 1;
    __asm__("nop");
    D11 = 1; 
    __asm__("nop");
    D12 = 1; 
    __asm__("nop");
    D13 = 1; 
    __asm__("nop");


	while(1) {
	}
}