#include "mainSettings.h"

#include <TM1638.h>

#define DISPLAY_ACTIVE true
#define DISPLAY_INTENSITY 3

TM1638 *ledModule;

/* ************ Idle scene *************************** */



/* ************ Demo scene *************************** */

void output_renderDemoScene (byte buttonPattern) {
   byte cycle=(millis()/100)%8; // cycle from 0 to 8, stepping in 1/10s
   byte prevCycle=(cycle-1)%8;

   if(prevCycle!=cycle) {
            ledModule->clearDisplayDigit(prevCycle,false);
            ledModule->setDisplayDigit(0,cycle,false); 
    }

    if(buttonPattern==0)   ledModule->setLEDs(B10000000>>cycle);
    else ledModule->setLEDs(buttonPattern);
}


/* ************ clear All *************************** */

void output_clearAllSequence ()
{
   ledModule->clearDisplay();
   ledModule->setLEDs(0);
  
}




/* *********   output_setup    *******************************+
 */
void output_setup(TM1638 *ledKeyModToUse) {
  ledModule=ledKeyModToUse;
  ledModule->setupDisplay(DISPLAY_ACTIVE, DISPLAY_INTENSITY);
}
