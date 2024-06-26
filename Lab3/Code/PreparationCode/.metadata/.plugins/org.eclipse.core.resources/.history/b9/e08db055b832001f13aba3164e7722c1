#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  int lastValueDiff = 0;
  sysConfig();
  lcd_init();
  lcd_clear();

  while(1){
    switch(state){
      case state0:
        enterLPM(lpm_mode);
        break;

      case state1:
        PrepareLCDState1();
        enableTimerA1();
        while(state == state1){
            lastValueDiff = DispFreqFromInput(lastValueDiff);
        }
        disableTimerA1();
        lcd_clear();
        break;
		 
      case state2:
        lcd_clear();
        CountSec();
        lcd_clear();
        break;

      case state3:
         EnableTimerState3();
         while(state == state3){
             genBuzz();
         }
         DisableTimerState3();
         break;
                
      case state4:
        lcd_clear();
        printBs();
        lcd_clear();
        break;
		
    }
  }
}
  
  
  
  
  
  
