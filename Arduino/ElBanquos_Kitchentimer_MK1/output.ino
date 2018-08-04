#include "mainSettings.h"
#include "kitchenTimer.h"

#include <TM1638.h>

#ifdef TRACE_ON
#define TRACE_OUTPUT_SEQUENCE
#endif

#define DISPLAY_ACTIVE true
#define DISPLAY_INTENSITY 2

#define BLINKCYCLE_FOCUS 1
#define BLINKCYCLE_ALERT 2
#define BLINKCYCLE_OVER 3
#define BLINKCYCLE_HOLD 4
#define BLINKCYCLE_RUNNING 5


#define BLINKSTATE(blinkcycle) bitRead(output_blinkCycleFlags,blinkcycle)

byte output_blinkCycleFlags=0;

byte output_ledPattern=0;

TM1638 *ledModule;


/*  ************************  scenes  ***********************************
 *  *********************************************************************
 */

/* ************ Idle scene *************************** */

void output_renderIdleScene(KitchenTimer  myKitchenTimerList[]) {

  determineBlinkCycles(); 
  output_ledPattern=0;
  
  for(int i=0;i<TIMER_COUNT;i++)
  {
    renderTimerCompact(myKitchenTimerList[i],i*2);
    setLedBitByTimerStatus(myKitchenTimerList[i],i*2+1);  
  }
 
  ledModule->setLEDs(output_ledPattern);
}

/* ************ Set scene *************************** */

void output_renderSetScene(KitchenTimer myKitchenTimerList[],long selected_time, byte targetTimer) {
  byte cycle=(millis()/100)%8; // cycle from 0 to 8, stepping in 1/10s
  byte startSegment=targetTimer*2;
  
  determineBlinkCycles(); 
  renderTimeLong(selected_time);
  
  for(int i=0;i<TIMER_COUNT;i++)
  {
    setLedBitByTimerStatus(myKitchenTimerList[i],i*2+1);  
  }
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

/*  ************************  Sequences *********************************
 *  *********************************************************************
 */
/* ************ clear All *************************** */

void output_clearAllSequence ()
{
  ledModule->clearDisplay();
  ledModule->setLEDs(0);
  #ifdef TRACE_OUTPUT_SEQUENCE
    Serial.println(F("!ouput_clearAllSequence"));  
  #endif
  
}

void output_resumeSequence(KitchenTimer myKitchenTimerList[],byte ui_focussed_timer_index)
{

  ledModule->clearDisplay();
  ledModule->setDisplayToString("Go",0,ui_focussed_timer_index*2); delay(1000);    
  
  // IMPROVE manage animation of all other timers
}

void output_holdSequence(KitchenTimer myKitchenTimerList[],byte ui_focussed_timer_index)
{

  ledModule->clearDisplay();
  ledModule->setDisplayToString("PA",0,ui_focussed_timer_index*2); delay(1000);   
    // IMPROVE manage animation of all other timers 
}


void  output_blockedSequence(KitchenTimer myKitchenTimerList[],byte ui_focussed_timer_index)
{
  ledModule->clearDisplay();
  for(int i=0;i<5;i++)
  {
  ledModule->setDisplayToString("? ",0,ui_focussed_timer_index*2); delay(100);   
  ledModule->setDisplayToString(" ?",0,ui_focussed_timer_index*2); delay(100); 
  }
  // IMPROVE manage animation of all other timers 
}


void output_resetSequence(KitchenTimer myKitchenTimerList[],byte ui_focussed_timer_index)
{

  ledModule->clearDisplay();
  ledModule->setDisplayToString("OF",0,ui_focussed_timer_index*2); delay(1000);   
    // IMPROVE manage animation of all other timers 
}

/*  ************************  Helpers  **********************************
 *  *********************************************************************
 */


void determineBlinkCycles()
{
   byte frame=(millis()/63)%32;  /* Create 16fps framenumber, 1 bit in pattern = 1/16 second, 8 bit= 1/2 second */
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_FOCUS,(0x33333333>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_ALERT,(0xF555f555>>frame)&0x0001); 
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_OVER, (0xfff00000>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_HOLD, (0xfffffcfc>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_RUNNING, (0xfffffffe>>frame)&0x0001);
}


void renderTimerCompact(KitchenTimer myKitchenTimer, byte startSegment) {


   long currentTime=abs(myKitchenTimer.getTimeLeft());
   bool setDot=BLINKSTATE(BLINKCYCLE_RUNNING); /* dot will blink with running cycle, when shown */

   /* Disabled  static */
   if(myKitchenTimer.isDisabled())  // Inactive channels just show a dot
   {
      ledModule->clearDisplayDigit(startSegment,true);
      ledModule->clearDisplayDigit(startSegment+1,false);
      return;   
   } 

   /* on hold blinking */
   if(!myKitchenTimer.isOver() && myKitchenTimer.isOnHold() &&!BLINKSTATE(BLINKCYCLE_HOLD))
   {
      ledModule->clearDisplayDigit(startSegment,false);
      ledModule->clearDisplayDigit(startSegment+1,false);
      return;   
   }

   /* Alert and Over */
   if( myKitchenTimer.isOver()) {
     if(myKitchenTimer.hasAlert())
     {  if  ( BLINKSTATE(BLINKCYCLE_ALERT))
        {
          ledModule->setDisplayToString("*_",0,startSegment);    
          return;      
        } else {
          ledModule->setDisplayToString("_*",0,startSegment); 
          return;
        }
     } else {
        ledModule->setDisplayToString("__",B10000000,startSegment);   
        return;
     }
   }
   /* when here, we are allowed to show the time */
   renderTimeCompact(abs(myKitchenTimer.getTimeLeft()),setDot,startSegment);
}

void renderTimeCompact(long currentTime,bool dotState, byte startSegment)
{
   char s[8];
 
   if(currentTime>=86400)  // day display mode
   { 
      sprintf(s, "%dd", currentTime/86400);
      ledModule->setDisplayToString(s,0,startSegment);    
      return;  
   }
   
   if(currentTime>=36000)  // 10 Hour display mode
   { 
      sprintf(s, "H%d", currentTime/36000);
      ledModule->setDisplayToString(s,0,startSegment);    
      return;  
   }
   
   if(currentTime>=3600)  // Hour display mode
   { 
      sprintf(s, "%dh", currentTime/3600);
      ledModule->setDisplayToString(s,0,startSegment);    
      return;  
   }
   
   if(currentTime>=600)  // Minute display mode
   { 
      ledModule->setDisplayDigit(currentTime/600,startSegment,false);
      ledModule->setDisplayDigit((currentTime/60)%10,startSegment+1,dotState);
      return;  
   }

   if(currentTime>59) {  //Minute + 10 second display mode
    ledModule->setDisplayDigit(currentTime/60,startSegment, dotState);
    ledModule->setDisplayDigit((currentTime%60)/10,startSegment+1,false);
    return;
   }
   
   // last seconds
   sprintf(s, "%2d", abs(currentTime));
   ledModule->setDisplayToString(s,0,startSegment);
}



void renderTimeLong(long currentTime)
{
   char s[9];
   currentTime=abs(currentTime);
   if(currentTime<86400)  {// below 24 Hour display mode hh"H"mm.ss
      sprintf(s, " %2ldh%02ld%02ld", (currentTime/3600)%24,(currentTime/60)%60,currentTime%60);
      ledModule->setDisplayToString(s,B00000100,0);    
        return;  
   }

   sprintf(s, "%ldd%2dh%02d'", (currentTime/86400)%24,(currentTime/3600)%24,(currentTime/60)%60);
   ledModule->setDisplayToString(s,B00000010,0);    
   return;  

}


/* Prepere the bits for the led bar accordingly */
void setLedBitByTimerStatus(KitchenTimer theKitchenTimer, byte ledIndex) {

   if(theKitchenTimer.isDisabled()    
      ||(theKitchenTimer.isOnHold() &&!BLINKSTATE(BLINKCYCLE_HOLD))
      )
   {
       bitClear(output_ledPattern,ledIndex);
      return;   
   }

   if( theKitchenTimer.isOver()) 
   {
     if( !theKitchenTimer.hasAlert())
     {
      bitClear(output_ledPattern,ledIndex);
      return;
     }
     if(BLINKSTATE(BLINKCYCLE_ALERT)) {
         bitClear(output_ledPattern,ledIndex);   
        return;      
     }
   }   
   /* since no rule switches of the led we will light it up */
   bitSet(output_ledPattern,ledIndex);
}

/*  ************************  setup  ************************************
 *  *********************************************************************
 */

void output_setup(TM1638 *ledKeyModToUse) {
  ledModule=ledKeyModToUse;
  ledModule->setupDisplay(DISPLAY_ACTIVE, DISPLAY_INTENSITY);
}
