#include <TM1638.h>
#include "mainSettings.h"
#include "kitchenTimer.h"   // Class declaration

#ifdef TRACE_ON
#define TRACE_CLOCK 
#define DEBUG_SETTING_1 
//#define DEBUG_SCALE_TABLE 
#endif

#define DEFAULT_INTERVAL 600   // !0 Minutes
#define TIMER_MAX_INTERVAL 603800  // Seconds of 7 days
#define CONTROL_VALUE_MAX  convertTimeToControlvalue(TIMER_MAX_INTERVAL)
#define CONTROL_VALUE_DEFAULT_UP  convertTimeToControlvalue(600)   // 10 Minutes as start value when turning upwards 
#define CONTROL_VALUE_DEFAULT_DN  convertTimeToControlvalue(180)   // 3 Minutes as start value when turning downwards 
#define UI_FALLBACK_INTERVAL 10   // seconds, ui will fall back to idle wihtout interaction
#define PRESS_DURATION_FOR_RESET 2500  // milliseconds you must hold select to disable a timer
#define PRESS_DURATION_SHORT 300  // milliseconds while a press counts as short 

const long timer_interval_preset[]={180,330,600,1200}; // 3 min, 5.5 min, 10 min, 20 min

#define MOCKUP_TIMER 0


enum UI_MODES {
  IDLE_MODE, // show time, check for alarms, check age of rds time 
  PRESELECT_MODE,
  SET_MODE
};
UI_MODES ui_mode = IDLE_MODE; 


byte ui_focussed_timer_index=MOCKUP_TIMER;  // until we have multi timer support we test everything with 3
bool ui_value_changed=false;
long ui_preselect_value=NO_TIME_SELECTION;

TM1638 ledAndKeymodule(4, 3, 2);  // the led+keys module is input and output, so core must own it

KitchenTimer myKitchenTimerList[TIMER_COUNT];

/* *************** IDLE_MODE ***************** */
void enter_IDLE_MODE(){
  #ifdef TRACE_CLOCK
    Serial.println(F("#IDLE"));
  #endif
  ui_mode=IDLE_MODE;
  output_clearAllSequence ();
  input_pauseUntilRelease();
}

void process_IDLE_MODE()
{
  KitchenTimer *focussed_timer;


  for(ui_focussed_timer_index=0;ui_focussed_timer_index<TIMER_COUNT;ui_focussed_timer_index++)
  {  
    focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
    if(input_timerButtonGotReleased(ui_focussed_timer_index) )
    {
      if(focussed_timer->hasAlert()){
        focussed_timer->acknowledgeAlert(); 
      } else {
        enter_SET_MODE_without_preselection();
        return;      
      }
    } 
  }

  /* Automated acknowledgement */
  for(int i=0;i<TIMER_COUNT;i++)
  {
    if(myKitchenTimerList[i].hasAlert()
       && (myKitchenTimerList[i].getTimeLeft()< -15) ) // TODO: Make this longer, when not in IDLE
           myKitchenTimerList[i].acknowledgeAlert();
  }

  output_renderIdleScene(myKitchenTimerList);
}


/* *************** PRESELECT_MODE ***************** */
void enter_PRESELECT_MODE(){
  #ifdef TRACE_CLOCK
    Serial.println(F("#PRESELECT_MODE"));
  #endif
  ui_mode=PRESELECT_MODE;
}

void process_PRESELECT_MODE()
{

}

/* *************** SET_MODE ***************** */


/* depending on the timer status, we simlulate a preselection when timer was disabled  */ 
void enter_SET_MODE_without_preselection()
{
  #ifdef TRACE_CLOCK
    Serial.println(F("#SET_MODE_with_timer_defaults"));
  #endif
  
  KitchenTimer *focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];

  input_setEncoderRange(0,CONTROL_VALUE_MAX,1,false);   

  /* simulate presection when timer was disabled */
  if(focussed_timer->isDisabled()) 
  {   
            input_setEncoderValue(convertTimeToControlvalue(timer_interval_preset[ui_focussed_timer_index]));     
            ui_value_changed=true;         
  } else ui_value_changed=false;  // Trigger capture of value with first encoder turn
  enter_SET_MODE();
}

/* prepare the set mode: 
 *  
 */
void enter_SET_MODE()
{
  #ifdef TRACE_CLOCK
    Serial.println(F("#SET_MODE"));
  #endif
  if(!ui_value_changed)
  {
    input_setEncoderRange(-200,200,1,false);   // set encoder to track an initial movement 
    input_setEncoderValue(0);  
  }
  ui_mode=SET_MODE;
  output_clearAllSequence ();
  input_pauseUntilRelease();
}

void process_SET_MODE()
{
   static bool select_short_press=false;
   
   KitchenTimer *focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
   /* UI Fallback, after brief time of no interaction */
   if(input_getSecondsSinceLastEvent()>UI_FALLBACK_INTERVAL) 
   {
     enter_IDLE_MODE();
     return;    
   }

   /* Button of the timer */

   if( input_timerButtonIsPressed(ui_focussed_timer_index)
   && input_getCurrentPressDuration()>PRESS_DURATION_FOR_RESET)
   {
       focussed_timer->disable();
       output_resetSequence(myKitchenTimerList,ui_focussed_timer_index);
       enter_IDLE_MODE();
       return;       
   }
   
   if(input_timerButtonGotReleased(ui_focussed_timer_index)&& input_getCurrentPressDuration()<PRESS_DURATION_FOR_RESET ) {
     if(ui_value_changed) {   
       focussed_timer->setInterval(convertControlvalueToTime(input_getEncoderValue()));
       focussed_timer->startCounting();
     }
     enter_IDLE_MODE();
     return;    
   } 

  /* Select Button */
   if( input_selectIsPressed() && input_getCurrentPressDuration()>PRESS_DURATION_SHORT)
   {
    output_renderSetScene_withLastTime(myKitchenTimerList,focussed_timer->getLastSetTime(),ui_focussed_timer_index); 
    return;
   }

  if( input_selectGotReleased() && input_getCurrentPressDuration()<PRESS_DURATION_SHORT)
  {
    if(input_getLastPressDuration()<PRESS_DURATION_SHORT) 
    {
    
      if(ui_value_changed)  
      { 
        focussed_timer->setInterval(convertControlvalueToTime(input_getEncoderValue()));
        focussed_timer->startCounting();
        enter_IDLE_MODE();
        return;
      }
      
      if(focussed_timer->isOver())  
      {
        focussed_timer->disable();
        enter_IDLE_MODE();
        return;
       }
  
      if(focussed_timer->isRunning()) 
      {
        focussed_timer->stopCounting();
        output_holdSequence(myKitchenTimerList,ui_focussed_timer_index);
        enter_IDLE_MODE();
        return;   
      }
      
      if(focussed_timer->isOnHold()) 
      {
        focussed_timer->startCounting();      
        output_resumeSequence(myKitchenTimerList,ui_focussed_timer_index);
        enter_IDLE_MODE();
        return;   
      } 
    }
  }

   /* Button  of other timer */
  for(int other_timer_index=0;other_timer_index<TIMER_COUNT;other_timer_index++)
  {
    if(other_timer_index==ui_focussed_timer_index) continue;
    
    if(input_timerButtonGotPressed(other_timer_index)) {
     if(myKitchenTimerList[other_timer_index].hasAlert()){
        myKitchenTimerList[other_timer_index].acknowledgeAlert(); 
     } else {
      if(ui_value_changed)  // user put some effort in selecting a time
      {
        if(myKitchenTimerList[other_timer_index].isOver() ||
           myKitchenTimerList[other_timer_index].isDisabled())  // Other time has no duty
           {
            ui_focussed_timer_index=other_timer_index;
            focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
            input_pauseUntilRelease();
           } else {
            output_blockedSequence(myKitchenTimerList,other_timer_index); 
           }       
      } else {
        ui_focussed_timer_index=other_timer_index;  // Switch focus to other timer
        enter_SET_MODE_without_preselection();
        return;             
      }
     } 
    }
  }


  /* React to encoder changes  */
  int initial_encoderValue=0;
  
  if(input_hasEncoderChangeEvent()) {
    input_getEncoderValue(); // acknowledge the processing of the change to the input module
    if(!ui_value_changed) 
    {
      if(focussed_timer->isRunning() || focussed_timer->isOnHold()) 
      {
        initial_encoderValue=convertTimeToControlvalue(focussed_timer->getTimeLeft());                  
      } else {  // there is no timer value we can start from 
        if(input_getEncoderValue()>0) initial_encoderValue=CONTROL_VALUE_DEFAULT_UP;
        else                          initial_encoderValue=CONTROL_VALUE_DEFAULT_DN;
      }
      input_setEncoderRange(0,CONTROL_VALUE_MAX,1,false);   // would change the Value, so we must do this after checking the value
      input_setEncoderValue(initial_encoderValue);     
      ui_value_changed=true;  
    }
  }

  /* Display the value, original or selected */
  if(ui_value_changed) output_renderSetScene(myKitchenTimerList,convertControlvalueToTime(input_getEncoderValue()),ui_focussed_timer_index);
  else output_renderSetScene(myKitchenTimerList,focussed_timer->getTimeLeft(),ui_focussed_timer_index);
}



/* ******************   Helpers    *************************     
 * *********************************************************
 */

/* determine the default preseecltion values, depending on timer state and number 
 *  
 */


long convertControlvalueToTime(long value)
{
/* this is generated with a google sheet, dont ask for explanation */  
if(value==NO_TIME_SELECTION) return NO_TIME_SELECTION;  
if (value<30) return value;  // First step 1 s  
if (value<48) return 30+(value-30)*5; // Until h 2min Step 0,0833333333333333 min
if (value<66) return 120+(value-48)*10; // Until h 5min Step 0,166666666666667 min
if (value<106) return 300+(value-66)*15;  // Until h 15min Step 0,25 min
if (value<136) return 900+(value-106)*30; // Until h 30min Step 0,5 min
if (value<166) return 1800+(value-136)*60;  // Until 1h min Step 1 min
if (value<190) return 3600+(value-166)*300; // Until 3h 0min Step 5 min
if (value<274) return 10800+(value-190)*900;  // Until 24h 0min Step 15 min
if (value<322) return 86400+(value-274)*3600; // Until 48h min Step 60 min
if (value<378) return 172800+(value-322)*10800; // Until 168h min Step 180 min
}

long convertTimeToControlvalue(long totalSeconds)
{
/* this is generated with a google sheet, dont ask for explanation */
if(totalSeconds<30) return totalSeconds;
if(totalSeconds<120) return (totalSeconds-30)/5+30;
if(totalSeconds<300) return (totalSeconds-120)/10+48;
if(totalSeconds<900) return (totalSeconds-300)/15+66;
if(totalSeconds<1800) return (totalSeconds-900)/30+106;
if(totalSeconds<3600) return (totalSeconds-1800)/60+136;
if(totalSeconds<10800) return (totalSeconds-3600)/300+166;
if(totalSeconds<86400) return (totalSeconds-10800)/900+190;
if(totalSeconds<172800) return (totalSeconds-86400)/3600+274;
if(totalSeconds<604800) return (totalSeconds-172800)/10800+322;
}

 #ifdef TRACE_ON 
void trace_printTime(long timeValue)
{
    Serial.print((timeValue/86400));
    Serial.print(F("d "));
    Serial.print((timeValue/3600)%24);
    Serial.print(F(":"));
    Serial.print((timeValue/60)%60);
    Serial.print(F(":"));
    Serial.print(timeValue%60);  
}
#endif

/* *********************    Main Setup   **********************
 * ************************************************************
 */

void setup() {

  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
  
  sound_setup();
  output_setup(&ledAndKeymodule);
  input_setup(&ledAndKeymodule); 

  input_setEncoderRange(0,1,1,false);


  #ifdef DEBUG_SCALE_TABLE
  /* Test of stepconversion */
  long timevalue;
  long controlvalue;
  
  for(long i=0;i<312;i++) {
    timevalue=convertControlvalueToTime(i);
    controlvalue=convertTimeToControlvalue(timevalue);
    Serial.print(i);
    Serial.print(F(" - "));
    Serial.print(controlvalue);
    Serial.print(F("\t = "));
    trace_printTime(timevalue);
    Serial.print(F("\t > "));
    trace_printTime(convertControlvalueToTime(controlvalue));
    Serial.println();
  }
  #endif

}

/* ********************    Main Loop     **********************
 * ************************************************************
 */

void loop() {
   /* Inputs */
  input_switches_scan_tick();

    /* UI logic */
    switch (ui_mode) {
          case IDLE_MODE:                 process_IDLE_MODE(); break;
          case PRESELECT_MODE:            process_PRESELECT_MODE(); break;
          case SET_MODE:                  process_SET_MODE(); break;

    }

  /* Manage sound */
  for(int i=0;i<TIMER_COUNT;i++) {
    if(myKitchenTimerList[i].hasAlert()) {
      sound_playAlarmForTimer(i);
    } else {
      sound_stopAlarmForTimer(i);
    }    
  }
  sound_tick();
}
