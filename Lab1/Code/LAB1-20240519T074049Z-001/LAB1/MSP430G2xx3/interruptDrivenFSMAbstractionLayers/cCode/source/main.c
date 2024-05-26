#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  
  while(1){
	switch(state){
	  case state0:
		printSWs2LEDs();
                enterLPM(lpm_mode);
		break;
		 
	  case state1:
		disable_interrupts();
		incLEDs(1);
		delay(LEDs_SHOW_RATE);	// delay of 62.5 [ms]
		enable_interrupts();
		break;
		 
	  case state2:
		disable_interrupts();
		incLEDs(-1);
		delay(LEDs_SHOW_RATE);		// delay of 62.5 [ms]
		enable_interrupts();
		break;
		
	}
  }
}
  
  
  
  
  
  