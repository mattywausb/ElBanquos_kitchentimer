#include "mainSettings.h"
#include "kitchenTimer.h"

#include <LedControl.h>

#ifdef TRACE_ON
#define TRACE_OUTPUT_SEQUENCE
//#define TRACE_OUTPUT_HIGH
#endif

#define LED7SEG_DATA_PIN 12
#define LED7SEG_LOAD_PIN 11
#define LED7SEG_CLK_PIN 10
#define LED7SEG_DEVICE_COUNT 1
#define LED7SEG_ROTATION180_FLAG 1

LedControl led7seg=LedControl(LED7SEG_DATA_PIN,LED7SEG_CLK_PIN,LED7SEG_LOAD_PIN,LED7SEG_DEVICE_COUNT,LED7SEG_ROTATION180_FLAG);

#define DISPLAY_ACTIVE true
#define DISPLAY_INTENSITY 1

#define BLINKCYCLE_FOCUS 1
#define BLINKCYCLE_ALERT 2
#define BLINKCYCLE_OVER 3
#define BLINKCYCLE_HOLD 4
#define BLINKCYCLE_RUNNING 5
#define BLINKCYCLE_ALERT_FLIP 6

#define TIMER_CALM_DOWN_TIME -120


#define BLINKSTATE(blinkcycle) bitRead(output_blinkCycleFlags,blinkcycle)

byte output_blinkCycleFlags=0;

byte output_ledPattern=0;



/*  ************************ simple Operations **************************
 *  *********************************************************************
 */

void output_clearDisplay()
{
  led7seg.clearDisplay(0);
}


/*  ************************  scenes  ***********************************
 *  *********************************************************************
 */

/* ************ Idle scene *************************** */

void output_renderIdleScene(KitchenTimer  myKitchenTimerList[]) {

  determineBlinkCycles(); 
  output_ledPattern=0;
  
  for(int i=0;i<TIMER_COUNT;i++)
  {
    renderTimerCompact(myKitchenTimerList[i],i);
    setLedByTimerStatus(i,myKitchenTimerList[i]);  
  }
  dev_led_rg_show();
}

/* ************ Preselect scene *************************** */
void output_renderPreselectScene(KitchenTimer myKitchenTimerList[],long selected_time)
{
  determineBlinkCycles(); 
  renderTimeLong(selected_time);
  for(int i=0;i<TIMER_COUNT;i++)
  {
    setLedByTimerStatus(i,myKitchenTimerList[i]);  
  } 
  dev_led_rg_show();
}


/* ************ Set scene *************************** */

void output_renderSetScene(KitchenTimer myKitchenTimerList[],long selected_time, byte targetTimer) {
  determineBlinkCycles(); 
  renderTimeLong(selected_time);
  
  for(int i=0;i<TIMER_COUNT;i++)
  {
    if(i!=targetTimer) setLedByTimerStatus(i,myKitchenTimerList[i]);
    else {
      if((BLINKSTATE(BLINKCYCLE_FOCUS))) 
      {
        if(myKitchenTimerList[i].isOver()) dev_led_rg_set(i,LEDRG_GREEN);
        else setLedByTimerStatus(i,myKitchenTimerList[i]);
      }
      else dev_led_rg_set(i,0);
    }
  }
  dev_led_rg_show();
}

void output_renderSetScene_withLastTime (KitchenTimer myKitchenTimerList[],long selected_time, byte targetTimer) 
{
  #ifdef TRACE_OUTPUT_HIGH
   // Serial.println(F("renderSetScene_withLastTime"));
  #endif
  if(selected_time!=0) 
  {
    output_renderSetScene(myKitchenTimerList,selected_time,targetTimer); 
    led7seg.setChar(0,7,65,false); // Character A  
  } else led7seg.setString(0,7,"A -h----",B00000100);
}




/*  ************************  Sequences *********************************
 *  *********************************************************************
 */
/* ************ clear All *************************** */

void output_startupSequence()
{
  char s[9]; // Stringbuffer
  String compileDate=__DATE__;
  String compileTime=__TIME__;
  output_clearAllSequence ();

  led7seg.setString(0,7,compileDate.substring(4,6)+compileDate.substring(0,3)+compileDate.substring(9,11),B00100000);
  for(int i=0;i<TIMER_COUNT+2;i++) {
    if(i<TIMER_COUNT)dev_led_rg_set(i, LEDRG_GREEN);
    if(i>0 && i<TIMER_COUNT+1) dev_led_rg_set(i-1,LEDRG_RED);
    if(i>1) dev_led_rg_set(i-2,0);
    dev_led_rg_show();
    delay(250);
  }
  led7seg.setString(0,7,compileTime,B00000000);
  for(int i=0;i<TIMER_COUNT+2;i++) {
    if(i<TIMER_COUNT)dev_led_rg_set(i, LEDRG_GREEN);
    if(i>0 && i<TIMER_COUNT+1) dev_led_rg_set(i-1,LEDRG_RED);
    if(i>1) dev_led_rg_set(i-2,0);
    dev_led_rg_show();
    delay(250);
  }
  output_clearAllSequence ();
}

void output_clearAllSequence ()
{
  output_clearDisplay();
  for(int i=0;i<TIMER_COUNT;i++) dev_led_rg_set(i,0);
  dev_led_rg_show();
}

void output_startTimerSequence(KitchenTimer myKitchenTimerList[],byte ui_focussed_timer_index)
{

  output_clearDisplay();
  led7seg.setString(0,ui_focussed_timer_index*2+1,"Go",0);
  setLedByTimerStatus(ui_focussed_timer_index,myKitchenTimerList[ui_focussed_timer_index]);  
  dev_led_rg_show(); 
  delay(1000);    
  
  // IMPROVE manage animation of all other timers
}

void output_holdSequence(KitchenTimer myKitchenTimerList[],byte ui_focussed_timer_index)
{

  output_clearDisplay();
  led7seg.setString(0,ui_focussed_timer_index*2+1,"PA",0); delay(1000);   
    // IMPROVE manage animation of all other timers 
}


void  output_blockedSequence(KitchenTimer myKitchenTimerList[],byte ui_focussed_timer_index)
{
  output_clearDisplay();
  for(int i=0;i<5;i++)
  { //26
  led7seg.setChar(0,ui_focussed_timer_index*2,32,false);
  led7seg.setChar(0,ui_focussed_timer_index*2+1,26,false);
  delay(100); 
  led7seg.setChar(0,ui_focussed_timer_index*2,26,false);
  led7seg.setChar(0,ui_focussed_timer_index*2+1,32,false);
  delay(100); 
  }
  // IMPROVE manage animation of all other timers 
}


void output_resetSequence(KitchenTimer myKitchenTimerList[],byte ui_focussed_timer_index)
{

  output_clearDisplay();
  led7seg.setString(0,ui_focussed_timer_index*2+1,"OF",0); 
  setLedByTimerStatus(ui_focussed_timer_index,myKitchenTimerList[ui_focussed_timer_index]);  
  dev_led_rg_show(); 
  delay(1000);   
  // IMPROVE manage animation of all other timers 
}

/*  ************************  Helpers  **********************************
 *  *********************************************************************
 */


void determineBlinkCycles()
{
   byte frame=(millis()/63)%32;  /* Create 16fps framenumber, 1 bit in pattern = 1/16 second, 8 bit= 1/2 second */
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_FOCUS,(0x33333333>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_ALERT,(0x55555555>>frame)&0x0001); 
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_ALERT_FLIP,(0xFF00FF00>>frame)&0x0001); 
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_OVER, (0xfff00000>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_HOLD, (0xfffffcfc>>frame)&0x0001);
   bitWrite(output_blinkCycleFlags,BLINKCYCLE_RUNNING, (0xfffffffe>>frame)&0x0001);
}


void renderTimerCompact(KitchenTimer myKitchenTimer, byte timerSlot) {


   long currentTime=abs(myKitchenTimer.getTimeLeft());
   bool setDot=BLINKSTATE(BLINKCYCLE_RUNNING); /* dot will blink with running cycle, when shown */

   /* Disabled  static */
   if(myKitchenTimer.isDisabled())  // Inactive channels just show a dot
   {
      led7seg.setString(0,timerSlot*2+1,"  ",0);
      return;   
   } 

   /* on hold blinking */
   if(!myKitchenTimer.isOver() && myKitchenTimer.isOnHold() &&!BLINKSTATE(BLINKCYCLE_HOLD))
   {
      led7seg.setString(0,timerSlot*2+1,"  ",0);
      return;   
   }

   /* Alert and Over */
   if( myKitchenTimer.isOver()) 
   {
      if(myKitchenTimer.hasAlert()) {
        if(BLINKSTATE(BLINKCYCLE_ALERT_FLIP))
        
          if( BLINKSTATE(BLINKCYCLE_ALERT))
            {
              led7seg.setString(0,timerSlot*2+1,"*_",0);    
              return;      
            } else {
              led7seg.setString(0,timerSlot*2+1,"_*",0); 
              return;
            }
      }
      else  // not in alert state, just over
      if ( myKitchenTimer.getTimeLeft()<TIMER_CALM_DOWN_TIME || BLINKSTATE(BLINKCYCLE_OVER) ) 
      {

       led7seg.setString(0,timerSlot*2+1,"__",B01);   
       return;        
      }
   }
   /* when here, we are allowed to show the time */
   renderTimeCompact(timerSlot,abs(myKitchenTimer.getTimeLeft()),setDot);
}

void renderTimeCompact(byte timerSlot,long currentTime,bool dotState)
{
   char s[8];  // s is the stringbuffer for easy formatted rendering
   byte startSegment=(timerSlot*2+1);
   
   if(currentTime>=86400)  // day display mode
   { 
      sprintf(s, "%dd", currentTime/86400);
      led7seg.setString(0,startSegment,s,dotState?B00000000:0);    
      return;  
   }
   
   if(currentTime>=36000)  // 10 Hour display mode
   { 
      sprintf(s, "H%d", currentTime/36000);
      led7seg.setString(0,startSegment,s,dotState?B00000000:0);    
      return;  
   }
   
   if(currentTime>=3600)  // Hour display mode
   { 
      sprintf(s, "%dh", currentTime/3600);
      led7seg.setString(0,startSegment,s,dotState?B00000000:0);     
      return;  
   }
   
   if(currentTime>=600)  // Minute display mode
   { 
      led7seg.setDigit(0,startSegment,currentTime/600,dotState);
      led7seg.setDigit(0,startSegment-1,(currentTime/60)%10,false);
      return;  
   }

   if(currentTime>59) {  //Minute + 10 second display mode
    led7seg.setDigit(0,startSegment,currentTime/60, 0);
    led7seg.setDigit(0,startSegment-1,(currentTime%60)/10,dotState);
    return;
   }
   
   // last seconds
   sprintf(s, "%2d", abs(currentTime));
   led7seg.setString(0,startSegment,s,0);
}


void renderTimeLong(long currentTime)
{
   char s[9];
   currentTime=abs(currentTime);
   if(currentTime<86400)  {// below 24 Hour display mode hh"h"mm.ss
      sprintf(s, "%2ldh%02ld%02ld", (currentTime/3600)%24,(currentTime/60)%60,currentTime%60);
      led7seg.setString(0,6,s,B00010100);    
        return;  
   }

   // more then 24 Hour display mode d"d"hh"h"mm
   sprintf(s, "%ldd%2ldh%02ld", (currentTime/86400)%24,(currentTime/3600)%24,(currentTime/60)%60);
   led7seg.setString(0,6,s,B00000100);    
   return;  

}

void clearTimeCompact(byte timerSlot)
{
  led7seg.setChar(0,timerSlot*2+1,32,false); //Space char
  led7seg.setChar(0,timerSlot*2,32,false); //Space char
}

/* Prepere the bits for the led bar accordingly */
void setLedByTimerStatus(byte timerIndex,KitchenTimer theKitchenTimer ) {

   if(theKitchenTimer.isDisabled()   )
   {
          dev_led_rg_set(timerIndex, LEDRG_GREEN);
          return;    
   }

    // Probably running so set to red

   if(theKitchenTimer.isOnHold() &&!BLINKSTATE(BLINKCYCLE_HOLD)) // On Hold blink
   {
       dev_led_rg_set(timerIndex, 0);
       return;
   }

   if(theKitchenTimer.hasAlert()) // Alert Blink
   {
    if(BLINKSTATE(BLINKCYCLE_ALERT)) dev_led_rg_set(timerIndex, 0);
    else  dev_led_rg_set(timerIndex, LEDRG_RED);
    return;
   }
    
   if(theKitchenTimer.isOver()) 
   {
       dev_led_rg_set(timerIndex, 0);
       return;
   }

   dev_led_rg_set(timerIndex, LEDRG_RED); // No rule interfered so we are running
}

/*  ************************  setup  ************************************
 *  *********************************************************************
 */


void output_setup() {
  led7seg.shutdown(0,false); // wake up 7seg display
  led7seg.setIntensity(0,8);
  led7seg.clearDisplay(0);
  dev_led_rg_setup();
  output_startupSequence();
}
