#include "mainSettings.h"
#include "kitchenTimer.h"

#include <TM1638.h>

#define DISPLAY_ACTIVE true
#define DISPLAY_INTENSITY 3

#define BLINKCYCLE_1xSEC 1
#define BLINKCYCLE_2xSEC 2
#define BLINKCYCLE_FOCUS 3
#define BLINKSTATE(blinkcycle) bitRead(output_blinkCylcleFlags,blinkcycle)
byte output_blinkCylcleFlags=0;

TM1638 *ledModule;

/* ************ Idle scene *************************** */

void output_renderIdleScene(KitchenTimer myKitchenTimer) {

  determineBlinkCycles(); 
  // TODO: Wrap a loop around this to iterate over timers 
  renderCompactTimer(myKitchenTimer,6);  
  // TODO: renderTimerLed(myKitchenTimer);
}

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

/* **************** internal helper funtions **************/

voic determineBlinkCycles()
{
   unsigned long frameTime=millis();  /* Freeze time for consistent blink picture */
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_FOCUS,(frameTime/70)%2;
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_2xSEC,(frameTime/250)%2;
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_1xSEC,(frameTime/500)%2;
}

void renderIdleSceneTimer(KitchenTimer myKitchenTimer, byte startSegment,byte blinkCycleFlags) {

   char s[8];
   long currentTime=abs(myKitchenTimer.getTimeLeft());

   bool withDot=false;

   if(currentTime>0 || BLINKSTATE(BLINKCYCLE_2xSEC))

     if(currentTime>=600) {

     if(currentTime>59) {
      ledModule->setDisplayDigit(currentTime/60,startSegment, currentTime<0||BLINKSTATE(BLINKCYCLE_2xSEC));
      ledModule->setDisplayDigit((currentTime%60)/10,startSegment+1,false);
      return;
     }
    {
        sprintf(s, "%2d", abs(currentTime));
        ledModule->setDisplayToString(s,0,startSegment);
   } else {
      ledModule->clearDisplayDigit(startSegment,false);
      ledModule->clearDisplayDigit(startSegment+1,false);
   }
}


/* *********   output_setup    *******************************+
 */
void output_setup(TM1638 *ledKeyModToUse) {
  ledModule=ledKeyModToUse;
  ledModule->setupDisplay(DISPLAY_ACTIVE, DISPLAY_INTENSITY);
}
