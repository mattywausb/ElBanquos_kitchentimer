#include "mainSettings.h"
#include "LedControl.h"

#ifdef TRACE_ON
#define TRACE_OUTPUT_SEQUENCE
#define TRACE_OUTPUT_HIGH
#endif

#define DISPLAY_ACTIVE true
#define DISPLAY_INTENSITY 1

#define BLINKCYCLE_FOCUS 1
#define BLINKCYCLE_ALERT 2
#define BLINKCYCLE_OVER 3
#define BLINKCYCLE_HOLD 4
#define BLINKCYCLE_RUNNING 5

#define TIMER_CALM_DOWN_TIME -120

#define LED7SEG_DATA_PIN 8
#define LED7SEG_LOAD_PIN 7
#define LED7SEG_CLK_PIN 6

LedControl led7seg=LedControl(LED7SEG_DATA_PIN,LED7SEG_CLK_PIN,LED7SEG_LOAD_PIN,1);

byte output_blinkCycleFlags=0;


/*  ************************ simple Operations **************************
 *  *********************************************************************
 */

void output_clearDisplay()
{
  led7seg.clearDisplay(0);
}


/*  ************************  internal  ************************************
 *  *********************************************************************
 */

void renderTimeCompact(byte timerSlot,long currentTime,bool dotState)
{
   char s[8];  // s is the stringbuffer for easy formatted rendering
   byte startSegment=(timerSlot*2+1);
   
   if(currentTime>=86400)  // day display mode
   { 
      sprintf(s, "%dd", currentTime/86400);
      led7seg.setString(0,startSegment,s,dotState?B01000000:0);    
      return;  
   }
   
   if(currentTime>=36000)  // 10 Hour display mode
   { 
      sprintf(s, "H%d", currentTime/36000);
      led7seg.setString(0,startSegment,s,dotState?B01000000:0);    
      return;  
   }
   
   if(currentTime>=3600)  // Hour display mode
   { 
      sprintf(s, "%dh", currentTime/3600);
      led7seg.setString(0,startSegment,s,dotState?B01000000:0);     
      return;  
   }
   
   if(currentTime>=600)  // Minute display mode
   { 
      led7seg.setDigit(0,startSegment,currentTime/600,false);
      led7seg.setDigit(0,startSegment-1,(currentTime/60)%10,dotState);
      return;  
   }

   if(currentTime>59) {  //Minute + 10 second display mode
    led7seg.setDigit(0,startSegment,currentTime/60, dotState);
    led7seg.setDigit(0,startSegment-1,(currentTime%60)/10,false);
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
      led7seg.setString(0,6,s,B00001000);    
        return;  
   }

   // more then 24 Hour display mode d"d"hh"h"mm
   sprintf(s, "%ldd%2ldh%02ld", (currentTime/86400)%24,(currentTime/3600)%24,(currentTime/60)%60);
   led7seg.setString(0,6,s,B00000010);    
   return;  

}

/*  ************************  setup  ************************************
 *  *********************************************************************
 */

void output_setup() {
  led7seg.shutdown(0,false); // wake up 7seg display
  led7seg.setIntensity(0,8);
  led7seg.clearDisplay(0);
}



