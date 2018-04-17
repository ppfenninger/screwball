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
    uint8_t gameOn, gameStart, win, winRight, winLeft, timeOver, ballExitLeft, ballExitRight, buttonUp, D4up, D5up, *RPOR, *RPINR;
    uint16_t servo_multiplier, servo_offset, servo1Open, servo1Close, servo2Open, servo2Close, servo3Open, servo3Close, servo4Open, servo4Close;
    WORD32 servo_temp;

    // These pins go high when a coin has been inserted and the start button has been pressed.
    D1_DIR = OUT;
    __asm__("nop");
    D2_DIR = OUT; 
    D1 = 0; // player 1's side of the game
    __asm__("nop");
    D2 = 0; // player 2s side of the game
    __asm__("nop");

    // Four break beam sensors - 2 for winning conditions - 2 for loseing conditions
    D0 = 0; //win right
    D3 = 0; //win left
    D4 = 0;	//exit right
    D5 = 0; //exit left 

    //START OF SERVO SETUP	
    D6_DIR = OUT; //right ball return
    __asm__("nop");
    D7_DIR = OUT; //left ball return
    __asm__("nop");
    D8_DIR = OUT; //right ball start
    __asm__("nop");
    D9_DIR = OUT; // left ball start
    __asm__("nop");

    servo_offset = (uint16_t)(FCY * SERVO_MIN_WIDTH);
    servo_multiplier = (uint16_t)(FCY * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH));

    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;

    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D8_RP] = OC1_RP;  // connect the OC1 module output to pin D6
    RPOR[D9_RP] = OC2_RP;  // connect the OC2 module output to pin D7
    RPOR[D6_RP] = OC3_RP;  // connect the OC3 module output to pin D8
    RPOR[D7_RP] = OC4_RP;  // connect the OC4 module output to pin D9
    __builtin_write_OSCCONL(OSCCON | 0x40);

    OC1CON1 = 0x1C0F;   // configure OC1 module to use the peripheral
    OC1CON2 = 0x008B; 

    OC2CON1 = 0x1C0F;   // configure OC2 module to use the peripheral
    OC2CON2 = 0x008B;

    OC3CON1 = 0x1C0F;   // configure OC3 module to use the peripheral
    OC3CON2 = 0x008B;

    OC4CON1 = 0x1C0F;   // configure OC4 module to use the peripheral
    OC4CON2 = 0x008B;

    servo_temp.ul = 0x8000 * (uint32_t)servo_multiplier;

    // 4 servos - 2 for staring and 2 for ending 
    //values for the servoes where they are closed
    servo1Open = 32768;
    servo2Open = 32768;
    servo3Open = 32768;
    servo4Open = 32768; 

    //values for the servos where they are open
    servo1Close = 22768;
    servo2Close = 22768;
    servo3Close = 22768;
    servo4Close = 22768;

    OC1RS = servo_offset + servo1Close*servo_multiplier;
    __asm__("nop");
    OC2RS = servo_offset + servo2Close*servo_multiplier;
    __asm__("nop");
    OC3RS = servo_offset + servo3Close*servo_multiplier;
    __asm__("nop");
    OC4RS = servo_offset + servo4Close*servo_multiplier;
    __asm__("nop");

    OC1R = 1;
    __asm__("nop");
    OC2R = 1;
    __asm__("nop");
    OC3R = 1;
    __asm__("nop");
    OC4R = 1;
    __asm__("nop");

    OC1TMR = 0;
	OC2TMR = 0;
	OC3TMR = 0;
	OC4TMR = 0;

	T1CON = 0x0010;     // configure Timer1 to have a period of 20ms
    PR1 = 0x9C3F;
	TMR1 = 0;
    T1CONbits.TON = 1;
    //END OF SERVO SETUP

    //Button setup
    D10 = 0; //Button starts low
    D11_DIR = OUT; //LED in button
    __asm__("nop");
    D11 = 0; //LET in button 

    //Coin Receptor Setup
    D12 = 0; //no coin has been inserted when this is one

    // This pin tells us when time has run out
    D13 = 0;

    gameOn = 0; // the game starts off
    timeOver = 0;
    winRight = 0;
    winLeft = 0;
    D4up = 1;
    D5up = 1;
    LED1 = 0;
    LED2 = 0;
    LED3 = 0; 
    ballExitLeft = 0;
    ballExitRight = 0; 

	while(1) {

		//Checking for wins
		if(!D0){ //right side of the game has won
			D1 = 0; //turns off the right side of the game
			__asm__("nop");
			winRight = 1;
			OC1RS = servo_offset + servo1Close*servo_multiplier; //closes the right ball returns
			__asm__("nop");
		}
		if(!D3){
			D2 = 0; //turns off the left side of the game
			__asm__("nop");
			winLeft = 1;
			OC2RS = servo_offset + servo2Close*servo_multiplier; //closes the left ball returns
			__asm__("nop"); 
		}

		if(winLeft && winRight){
			win = 1;
		}

		//What happens if you time out
		if(D13 && gameOn){ //time has run out
			timeOver = 1; 
		}

		if (timeOver){ //the time has turned off
			OC1RS = servo_offset + servo1Close*servo_multiplier; //closes the right ball returns
			__asm__("nop");
			OC2RS = servo_offset + servo2Close*servo_multiplier; //closes the left ball returns 
			__asm__("nop"); 
		}

		//make sure that both balls are back before turning off the game
		if((!D4 && D4up) && (timeOver || winRight)){//right ball return
			ballExitRight =  1;
			D4up = 0; 
			D1 = 0; //turns off the right side of the game
		}
		else if(D4) {
			D4up = 1; 

		}
		if ((!D5 && D5up) && (timeOver || winLeft)){
			ballExitLeft  = 1;
			D5up = 0; 
			D2 = 0; //turns off the left side of the game 
		}
		else if (D5){ //debouncing
			D5up = 1;
		}

		//both balls have been returned and the game is over -- reset everything
		if((ballExitLeft && ballExitRight) && (timeOver || win)){
			if(win){
				D1 = 1;
			}
			else{
				D2 = 1;
			}
			gameOn = 0; 
			winRight = 0;
			winLeft = 0;
			win = 0;
			D11 = 0; //turns off the start button
			timeOver = 0;
			ballExitLeft = 0;
			ballExitRight = 0;
			LED3 = 0;  
		}

		//Starting the game
		if (D12 && !gameOn){ // the game has started --reset the win/lose lights?
			gameStart = 1;
			D11 = 1; //light up the start button
			__asm__("nop");
			D1 = 0;
			D2 = 0;
			LED3 = 1;
		}

		if ((D10 == 1) && gameStart){ // the player has pressed the start button, so the game actual has started
			D1 = 1; //right side of the game is on
			__asm__("nop");
			D2 = 1; //left side of the game is on
			__asm__("nop");
			gameOn = 1;
			buttonUp = 0;

			OC1RS = servo_offset + servo1Open*servo_multiplier; //opens the right ball returns
			__asm__("nop");
			OC2RS = servo_offset + servo2Open*servo_multiplier; //opens the left ball returns 
			__asm__("nop");
		}

		if ((D10 == 1) && gameOn){ // the player has pressed the start button, so the game actual has started
			LED1 = 1; 
			if(D1){
				OC3RS = servo_offset + servo3Open*servo_multiplier; //opens the right ball start
				__asm__("nop");
			}
			if(D2){
				OC4RS = servo_offset + servo4Open*servo_multiplier; //opens the left ball start
				__asm__("nop");
			}
			gameStart = 0; 
		}

		if (D10 == 0){
			LED1 = 0; 
			OC3RS = servo_offset + servo3Close*servo_multiplier; //opens the right ball start
			__asm__("nop");
			OC4RS = servo_offset + servo4Close*servo_multiplier; //opens the left ball start
			__asm__("nop");
		}

	}
}