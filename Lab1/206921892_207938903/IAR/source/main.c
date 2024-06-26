#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum CountType tp;
enum SYSmode lpm_mode;

void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  tp = up;
  sysConfig();
  unsigned char lastCountState1 = 0;
  unsigned char lastCountState2 = 1;
  
  while(1){
    int i;
	switch(state){
	  case state0:
	      clrLEDs();
          enterLPM(lpm_mode);
		break;
		 
	  case state1:
		disablePBInterrupts(~PB0); // Nobody can interrupt instead of PB0 for changing the type of the count
                tp = up;
                SetLEDs(lastCountState1);
                for (i=20;i>0;i--){
                  if (tp == up){
                    incLEDs(1);
                    delay(LEDs_SHOW_RATE); // delay of 0.5 [s]
                  }
                  else{
                    incLEDs((char)-1);
                    delay(LEDs_SHOW_RATE); // delay of 0.5 [s]
                  }
                }
                lastCountState1 = GetLEDs();
                state = state0;
		enablePBInterrupts(0xF0); // Enable all interrupts from PBS 
		break;
		 
	  case state2:
		disable_interrupts();
                SetLEDs(lastCountState2);
                for (i = 14; i>0 ; i--){
                  delay(LEDs_SHOW_RATE);    // delay of 0.5 [s]
                  shiftLEDcyclic(1);
                }
                lastCountState2 = GetLEDs();
                state = state0;
		enable_interrupts();
		break;

          case state3:
            clrLEDs();
            while(state==state3){
              SignalUp();
              delay(UP_SIGNAL_RATE);       //187.5 [us]
              SignalDown();
              delay(DOWN_SIGNAL_RATE);     //62.5 [us]
            }
             break;
                
          case state4:
            state=state0;
            break;
		
	}
  }
}
  
  
  
  
  
  
