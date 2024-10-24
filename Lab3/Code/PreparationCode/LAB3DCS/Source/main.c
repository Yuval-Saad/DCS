#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;



void main(void){

    state = state0;  // start in idle state on RESET
    lpm_mode = mode0;     // start in idle state on RESET

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
          WriteWord();
          lcd_clear();
          break;

		 
      case state2:
          Merge();
          lcd_clear();
          break;

      case state3:
          LEDsByDMA();
          break;
                
      case state4:
          MirrorByDMA();
          break;
		
    }
  }
}
  
  
  
  
  
  
