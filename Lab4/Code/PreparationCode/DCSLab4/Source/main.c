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

    counter = 0;


  while(1){
    switch(state){
      case state0:
          enableUARTRecevie();
          enterLPM(lpm_mode);
          break;
      case state1:
          PlayRGB();
          break;
      case state2:
          countUp();
          break;
      case state3:
          buzzGen();
          break;
      case state4:
          getX();
          break;
      case state5:
          dispalyPoten();
          break;
      case state7:
          PBstr();
          break;
    }
  }
}
  
  
  
  
  
  
