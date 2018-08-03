#include "mainSettings.h"
#include "kitchenTimer.h"

#include <TM1638.h>

#define DISPLAY_ACTIVE true
#define DISPLAY_INTENSITY 3

#define BLINKCYCLE_FOCUS 1
#define BLINKCYCLE_1xSEC 2
#define BLINKCYCLE_2xSEC 3
#define BLINKCYCLE_1x2SEC 4
#define BLINKCYCLE_HOLD 5
#define BLINKCYCLE_15to5 6

#define BLINKSTATE(blinkcycle) bitRead(output_blinkCycleFlags,blinkcycle)

byte output_blinkCycleFlags=0;

byte output_ledPattern=0;

TM1638 *ledModule;

/* ************ Idle scene *************************** */

void output_renderIdleScene(KitchenTimer myKitchenTimer) {

  determineBlinkCycles(); 
  // TODO: Wrap a loop around this to iterate over timers 
  renderTimerCompact(myKitchenTimer,6);  
  setLedBitByTimerStatus(myKitchenTimer,7);
  ledModule->setLEDs(output_ledPattern);
}

/* ************ Set scene *************************** */

void output_renderSetScene(KitchenTimer myKitchenTimer,long selected_time, byte targetTimer) {
  byte cycle=(millis()/100)%8; // cycle from 0 to 8, stepping in 1/10s
  byte startSegment=targetTimer*2;
  determineBlinkCycles(); 

  if(selected_time!=NO_TIME_SELECTION) renderTimeCompact(selected_time, startSegment);
  else renderTimerCompact(myKitchenTimer,startSegment);
  
  // for all not targets  setLedBitByTimerStatus(myKitchenTimer,6);
   
  
  // TODO: Wrap a loop around this to iterate over timers 
  bitWrite(output_ledPattern,startSegment+1,BLINKSTATE(BLINKCYCLE_FOCUS)); 
  ledModule->setLEDs(output_ledPattern);
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

void determineBlinkCycles()
{
   byte frame=(millis()/125)%16;  /* Create 8fps framenumber, 1 bit in pattern = 1/8 second */
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_FOCUS,(0x5555>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_2xSEC,(0x3333>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_1xSEC,(0x0f0f>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_1x2SEC,(0x00ff>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_HOLD,(0xffee>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_15to5,(0x0fff>>frame)&0x0001);  
}


void renderTimerCompact(KitchenTimer myKitchenTimer, byte startSegment) {


   long currentTime=abs(myKitchenTimer.getTimeLeft());

   /* manage state specifc blinking */
   if(!myKitchenTimer.isActive()   // 
      ||(!myKitchenTimer.isOver() && myKitchenTimer.isOnHold() &&!BLINKSTATE(BLINKCYCLE_HOLD))
      )
   {
      ledModule->clearDisplayDigit(startSegment,false);
      ledModule->clearDisplayDigit(startSegment+1,false);
      return;   
   }

   if( myKitchenTimer.isOver()) {
     if(myKitchenTimer.hasAlert()  && BLINKSTATE(BLINKCYCLE_2xSEC)) {
        ledModule->setDisplayToString("--",0,startSegment);    
        return;      
     }
     if(!myKitchenTimer.hasAlert()&& ! BLINKSTATE(BLINKCYCLE_1x2SEC) ) 
     {
      ledModule->clearDisplayDigit(startSegment,false);
      ledModule->clearDisplayDigit(startSegment+1,false);
      return;
      }
   }
   /* when here, we are allowed to show the time */
   renderTimeCompact(abs(myKitchenTimer.getTimeLeft()),startSegment);
}

void renderTimeCompact(long currentTime,byte startSegment)
{
   char s[8];
   if(currentTime>=36000)  // 10 Hour display mode
   { 
      ledModule->setDisplayToString("Lh",0,startSegment);    
      return;  
   }
   
   if(currentTime>=3600)  // Hour display mode
   { 
      sprintf(s, "h%d", currentTime/3600);
      ledModule->setDisplayToString(s,0,startSegment);    
      return;  
   }
   
   if(currentTime>=600)  // Minute display mode
   { 
      ledModule->setDisplayDigit(currentTime/600,startSegment,false);
      ledModule->setDisplayDigit((currentTime/60)%10,startSegment+1,true);
      return;  
   }

   if(currentTime>59) {
    ledModule->setDisplayDigit(currentTime/60,startSegment, true);
    ledModule->setDisplayDigit((currentTime%60)/10,startSegment+1,false);
    return;
   }
   
   // last seconds
   sprintf(s, "%2d", abs(currentTime));
   ledModule->setDisplayToString(s,0,startSegment);
}

/* Prepere the bits for the led bar accordingly */
void setLedBitByTimerStatus(KitchenTimer theKitchenTimer, byte ledIndex) {

   if(!myKitchenTimer.isActive()   
      ||(!theKitchenTimer.isOver() && theKitchenTimer.isOnHold() &&!BLINKSTATE(BLINKCYCLE_HOLD))
      )
   {
       bitClear(output_ledPattern,ledIndex);
      return;   
   }

   if( theKitchenTimer.isOver()) 
   {
     if(theKitchenTimer.hasAlert()  && BLINKSTATE(BLINKCYCLE_2xSEC)) {
         bitClear(output_ledPattern,ledIndex);   
        return;      
     }
     if(!theKitchenTimer.hasAlert()&& ! (BLINKSTATE(BLINKCYCLE_1x2SEC)) ) 
     {
      bitClear(output_ledPattern,ledIndex);
      return;
      }
   }   
   bitSet(output_ledPattern,ledIndex);
}


/* *********   output_setup    *******************************+
 */
void output_setup(TM1638 *ledKeyModToUse) {
  ledModule=ledKeyModToUse;
  ledModule->setupDisplay(DISPLAY_ACTIVE, DISPLAY_INTENSITY);
}
