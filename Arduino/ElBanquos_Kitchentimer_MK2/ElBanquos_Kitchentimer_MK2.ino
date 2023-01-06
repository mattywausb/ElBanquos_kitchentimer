#include <LedControl.h>


#include "mainSettings.h"
#include "kitchenTimer.h"   // Class declaration

#ifdef TRACE_ON
#define TRACE_MODE 
#define TRACE_MAIN_EVENTS
//#define DEBUG_SCALE_TABLE 
#endif

#define DEFAULT_INTERVAL 600   // 10 Minutes
#define TIMER_MAX_INTERVAL 603800  // Seconds of 7 days
#define CONTROL_VALUE_MAX  convertTimeToControlvalue(TIMER_MAX_INTERVAL)
#define DEFAULT_INTERVAL_UP  600   // 10 Minutes as start value when turning upwards 
#define DEFAULT_INTERVAL_DN  600   // 10 Minutes as start value when turning downwards
#define UI_FALLBACK_INTERVAL 10   // seconds, ui will fall back to idle wihtout interaction
#define PRESS_DURATION_FOR_RESET 3000  // milliseconds you must hold select to disable a timer
#define PRESS_DURATION_SHORT 500  // milliseconds while a press counts as short 
#define ALERT_DURATION 45 // Seconds the alert will go without manual ackknowledgement
#define MAX_OVER_TIME 5400 // Seconds the over time mode will be kept befor complet disabling the timer
#define MIN_OVER_TIME 180 // Seconds the over time mode will be kept at least

const long timer_interval_preset[]={1800,1200,375,180}; // 30 min, 20 min, 6'15 min, 3 min (order is right to left)


enum UI_MODES {
  IDLE_MODE, 
  PRESELECT_MODE,
  DISPLAY_MODE,
  SET_MODE
};
UI_MODES ui_mode = IDLE_MODE; 


byte ui_focussed_timer_index=0;  // this  is only relevant in DISPLAY and SET mode and will determined before entering these
byte ui_partner_timer_index=INDEX_FOR_UNDEFINED_TIMER;  // index of the timer, we refer when dialing in a delta time set

bool ui_button_press_from_previous_mode=false;

KitchenTimer myKitchenTimerList[TIMER_COUNT];

/* *************** IDLE_MODE ***************** 
*/
void enter_IDLE_MODE(){
  #ifdef TRACE_MODE
    Serial.println(F("#IDLE"));
  #endif
  ui_mode=IDLE_MODE;
  output_clearDisplay();
  input_IgnoreUntilRelease();
  ui_partner_timer_index=INDEX_FOR_UNDEFINED_TIMER;
}

void process_IDLE_MODE()
{
  KitchenTimer *focussed_timer;

  /* Timer Buttons */
  for(ui_focussed_timer_index=0;ui_focussed_timer_index<TIMER_COUNT;ui_focussed_timer_index++)
  {  
    focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
    if(input_timerButtonGotPressed(ui_focussed_timer_index) )
    {
      if(focussed_timer->hasAlert()){
        focussed_timer->acknowledgeAlert(); 
        #ifdef TRACE_MAIN_EVENTS
            Serial.println(F("TRACE_MAIN_EVENTS>alert acknowledged"));
        #endif

      } else {
        if(focussed_timer->isDisabled()) enter_SET_MODE( timer_interval_preset[ui_focussed_timer_index] );  
        else enter_DISPLAY_MODE();
        return;      
      }
    } 
  }

  /* Encoder Movement */
  if(input_hasEncoderChangeEvent()) 
  {
    enter_PRESELECT_MODE();
    return;
  }

  /* Automated acknowledgement */
  for(int i=0;i<TIMER_COUNT;i++)
  {
    if(myKitchenTimerList[i].hasAlert()
       && (myKitchenTimerList[i].getTimeLeft()< -ALERT_DURATION) ) // TODO: Make this longer, when not in IDLE
           {myKitchenTimerList[i].acknowledgeAlert();
           #ifdef TRACE_MAIN_EVENTS
            Serial.println(F("TRACE_MAIN_EVENTS>alert exceeded max duration"));
          #endif
           }
  }

  /* Automated disabling after twice of runtime */
  for(int i=0;i<TIMER_COUNT;i++)
  {
    if(myKitchenTimerList[i].isOver() && -myKitchenTimerList[i].getTimeLeft()>MIN_OVER_TIME)
    {
      if (((-myKitchenTimerList[i].getTimeLeft())>MAX_OVER_TIME)
      || (-myKitchenTimerList[i].getTimeLeft()>(myKitchenTimerList[i].getLastSetTime()*2))) myKitchenTimerList[i].disable();
    }
  }

  output_renderIdleScene(myKitchenTimerList);
}


/* *************** PRESELECT_MODE ***************** 
*/
void enter_PRESELECT_MODE(){
  #ifdef TRACE_MODE
    Serial.println(F("#PRESELECT_MODE"));
  #endif
    ui_mode=PRESELECT_MODE;
    input_setEncoderRange(0,CONTROL_VALUE_MAX,1,false);  
    input_setEncoderValue(convertTimeToControlvalue(DEFAULT_INTERVAL));
    output_clearDisplay();
    output_renderPreselectScene(myKitchenTimerList,convertControlvalueToTime(input_getEncoderValue()));    
}

void process_PRESELECT_MODE()
{
  KitchenTimer *focussed_timer;

   /* UI Fallback, after brief time of no interaction */
   if(input_getSecondsSinceLastEvent()>UI_FALLBACK_INTERVAL) 
   {
     enter_IDLE_MODE();
     return;    
   }

  /* Start timer by hitting timer button */
  for(ui_focussed_timer_index=0;ui_focussed_timer_index<TIMER_COUNT;ui_focussed_timer_index++)
  {  
    focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
    if(input_timerButtonGotReleased(ui_focussed_timer_index) )
    {
      if(focussed_timer->hasAlert()){
        focussed_timer->acknowledgeAlert();
        #ifdef TRACE_MAIN_EVENTS
            Serial.println(F("TRACE_MAIN_EVENTS>alert acknowledged"));
        #endif

      } else {
        if(focussed_timer->isOver() ||
           focussed_timer->isDisabled())  // timer is free to use
           {
              focussed_timer->setInterval(convertControlvalueToTime(input_getEncoderValue()));
              focussed_timer->startCounting();
              output_startTimerSequence(myKitchenTimerList,ui_focussed_timer_index);
              #ifdef TRACE_MAIN_EVENTS
                  Serial.print(F("TRACE_MAIN_EVENTS>Start Timer "));Serial.print(ui_focussed_timer_index);
                  Serial.print(F("Interval is "));Serial.println(focussed_timer->getLastStartTime());
              #endif
              enter_IDLE_MODE();
              return;
           } else {
            output_blockedSequence(myKitchenTimerList,ui_focussed_timer_index); 
           }                        
      }
      return;  
    } 
  }

  /* Dial Button long */
  if( input_dialIsPressed() && input_getCurrentPressDuration()>PRESS_DURATION_SHORT)
  {
    enter_IDLE_MODE(); 
    return;
  }

  
  output_renderPreselectScene(myKitchenTimerList,convertControlvalueToTime(input_getEncoderValue()));   
}


/* *************** DISPLAY_MODE ***************** 
*/
void enter_DISPLAY_MODE(){
  #ifdef TRACE_MODE
    Serial.println(F("#DISPLAY_MODE"));
    Serial.print(F("ui_focussed_timer_index="));Serial.println(ui_focussed_timer_index);
    Serial.print(F("time left: "));Serial.println(myKitchenTimerList[ui_focussed_timer_index].getTimeLeft());
  #endif

  input_setEncoderRange(-200,200,1,false);   // set encoder to track an initial movement 
  input_setEncoderValue(0);  
    
  ui_mode=DISPLAY_MODE;
  ui_button_press_from_previous_mode=true;
  output_clearAllSequence ();
  output_renderSetScene(myKitchenTimerList,myKitchenTimerList[ui_focussed_timer_index].getTimeLeft(),ui_focussed_timer_index,INDEX_FOR_UNDEFINED_TIMER);
}

void process_DISPLAY_MODE() {
 

   KitchenTimer *focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
   
   /* UI Fallback, after brief time of no interaction */
   if(input_getSecondsSinceLastEvent()>UI_FALLBACK_INTERVAL) 
   {
     enter_IDLE_MODE();
     return;    
   }

   /* Long press of current timer = Switch timer off */
   if( input_timerButtonIsPressed(ui_focussed_timer_index)
   && input_getCurrentPressDuration()>PRESS_DURATION_FOR_RESET)
   {
       focussed_timer->disable();
       output_resetSequence(myKitchenTimerList,ui_focussed_timer_index);
       enter_IDLE_MODE();
       return;       
   }

   /* Start partnered setting when current button is hold and another valid timer button got pressed */
  if( input_timerButtonIsPressed(ui_focussed_timer_index)) {
    for(byte other_timer_index=0;other_timer_index<TIMER_COUNT;other_timer_index++) {
      if(other_timer_index==ui_focussed_timer_index) continue;
      if(input_timerButtonGotPressed(other_timer_index) && 
        (myKitchenTimerList[other_timer_index].isOver() || myKitchenTimerList[other_timer_index].isDisabled()) ) { // Valid Target
          ui_partner_timer_index=ui_focussed_timer_index;
          ui_focussed_timer_index=other_timer_index;
          #ifdef TRACE_MAIN_EVENTS
                  Serial.println(F("TRACE_MAIN_EVENTS>Request for partnered setting "));
          #endif
          enter_SET_MODE( myKitchenTimerList[ui_partner_timer_index].getTimeLeft()/2 );
          return;
      }
    }
  }

   /* Catch and ignore the first release of the button, since it triggered the display mode  and should not trigger upcoming events */
   if( ui_button_press_from_previous_mode && !input_timerButtonIsPressed(ui_focussed_timer_index))
   {
    ui_button_press_from_previous_mode=false;
    return;
   }
   
   /* Got back to idle when focussed timer button was pressed short */
   if(input_timerButtonGotReleased(ui_focussed_timer_index)&& input_getCurrentPressDuration()<PRESS_DURATION_FOR_RESET ) {
     enter_IDLE_MODE();
     return;    
   } 

  /* Display last set time of timer when dial is pressed long */
   if( input_dialIsPressed() && input_getCurrentPressDuration()>PRESS_DURATION_SHORT)
   {
    output_renderSetScene_withLastTime(myKitchenTimerList,focussed_timer->getLastSetTime(),ui_focussed_timer_index); 
    return;
   }

  /* Display current time when dial is released again */
  if( input_dialGotReleased() && input_getLastPressDuration()>PRESS_DURATION_SHORT) {
      output_clearDisplay();
      return;  // Just to omit short button release reaction, Display will come up in next cycle
  } 

  /* pause/resume running timer with dial button short, go back to idle if timer is already over */
  if( input_dialGotReleased() )
  {
      
    if(focussed_timer->isOver())  
    {
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
      output_startTimerSequence(myKitchenTimerList,ui_focussed_timer_index);
      enter_IDLE_MODE();
      return;   
    } 
  }
  
   /* Ackowledge alert of any alerting timer or switch focus to other timer with timer buttons */
  for(int other_timer_index=0;other_timer_index<TIMER_COUNT;other_timer_index++)
  {
    if(other_timer_index==ui_focussed_timer_index) continue;
    
    if(input_timerButtonGotPressed(other_timer_index)) {
     if(myKitchenTimerList[other_timer_index].hasAlert()){
        myKitchenTimerList[other_timer_index].acknowledgeAlert(); 
        return;
     } else {  // Switch focus to other timer
        ui_focussed_timer_index=other_timer_index;  
        #ifdef TRACE_MODE
          Serial.print(F("TRACE_MAIN_EVENTS>switched to timer ")); Serial.print(ui_focussed_timer_index);
          Serial.print(F(" time left: "));Serial.println(myKitchenTimerList[ui_focussed_timer_index].getTimeLeft());
      #endif
        focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
                input_IgnoreUntilRelease();
        if(focussed_timer->isDisabled()) 
        {
          enter_IDLE_MODE();  
          return;
        }

     }
     } 
   }
  
  /* Switch to setting the current shown, when dial is turned  */
  
  if(input_hasEncoderChangeEvent()) {
    input_getEncoderValue(); // acknowledge the processing of the change to the input module
    if(focussed_timer->isRunning() || focussed_timer->isOnHold()) 
    {
      enter_SET_MODE(focussed_timer->getTimeLeft());    
      return ;             
    } else {  // there is no timer value we can start from o lets take the preset
      if(input_getEncoderValue()>0) enter_SET_MODE(timer_interval_preset[ui_focussed_timer_index]);
      else                          enter_SET_MODE(timer_interval_preset[ui_focussed_timer_index]);  // There might be a use of different values depending on the turning direction
      return;
    }
  }

  /* Provide output */
  output_renderSetScene(myKitchenTimerList,focussed_timer->getTimeLeft(),ui_focussed_timer_index,INDEX_FOR_UNDEFINED_TIMER);
  
}

/* *************** SET_MODE ***************** 
*/

void enter_SET_MODE(long preselected_time)
{
  #ifdef TRACE_MODE
    Serial.println(F("#SET_MODE"));
    Serial.print(F("preselected_time ="));Serial.println(preselected_time);
    Serial.print(F("ui_focussed_timer_index="));Serial.println(ui_focussed_timer_index);
    Serial.print(F("ui_partner_timer_index ="));Serial.println(ui_partner_timer_index);
  #endif
  ui_mode=SET_MODE;
  input_setEncoderRange(0,CONTROL_VALUE_MAX,1,false);   
  input_setEncoderValue(convertTimeToControlvalue(preselected_time));  
  output_clearAllSequence ();
  input_IgnoreUntilRelease();
}

void process_SET_MODE()
{
   
   KitchenTimer *focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
   
   /* UI Fallback, after brief time of no interaction */
   if(input_getSecondsSinceLastEvent()>UI_FALLBACK_INTERVAL) 
   {
     enter_IDLE_MODE();
     return;    
   }

   /* Escape set mode by long press of dial */
   if( input_dialIsPressed() && input_getCurrentPressDuration()>PRESS_DURATION_SHORT)
   {
        enter_IDLE_MODE();
        return;
   }

  /* ignore irritating edgcase of long press/release of dial */
  if( input_dialGotReleased() && input_getLastPressDuration()>PRESS_DURATION_SHORT)  return;

   /* Abort timer by pressing the button of the timer very long */
   if( input_timerButtonIsPressed(ui_focussed_timer_index)
   && input_getCurrentPressDuration()>PRESS_DURATION_FOR_RESET)
   {
       focussed_timer->disable();
       output_resetSequence(myKitchenTimerList,ui_focussed_timer_index);
       enter_IDLE_MODE();
       return;       
   }
   
   /* Start timer with selected time by short press of timer button or dial */
   if((input_timerButtonGotReleased(ui_focussed_timer_index) || input_dialGotReleased() )&& input_getLastPressDuration()<PRESS_DURATION_SHORT ) 
   {
     if(ui_partner_timer_index==INDEX_FOR_UNDEFINED_TIMER) {  // normal time setting
         focussed_timer->setInterval(convertControlvalueToTime(input_getEncoderValue()));
     } else {             // timesetting relative to other timer
         long relative_target_time= myKitchenTimerList[ui_partner_timer_index].getTimeLeft() - convertControlvalueToTime(input_getEncoderValue());
         if(relative_target_time>1) { // At least 1 second should be left
          focussed_timer->setInterval(relative_target_time);
         } else {  // go back to partner timer if partner timer as not enough time left
     #ifdef TRACE_MAIN_EVENTS
                  Serial.print(F("TRACE_MAIN_EVENTS>Blocking start since partner is smaller "));Serial.print(ui_focussed_timer_index);
     #endif
          output_blockedSequence(myKitchenTimerList,ui_focussed_timer_index);  
          ui_focussed_timer_index=ui_partner_timer_index;
          enter_DISPLAY_MODE();
          return;
         }
     }
     focussed_timer->startCounting();
     #ifdef TRACE_MAIN_EVENTS
                  Serial.print(F("TRACE_MAIN_EVENTS>Start Timer "));Serial.print(ui_focussed_timer_index);
                  Serial.print(F("Interval is "));Serial.println(focussed_timer->getLastStartTime());
     #endif
     output_startTimerSequence(myKitchenTimerList,ui_focussed_timer_index);
     enter_IDLE_MODE();
     return;    
   } 


   /* Button  of other timer */
  for(int other_timer_index=0;other_timer_index<TIMER_COUNT;other_timer_index++)
  {
    if(other_timer_index==ui_focussed_timer_index) continue;
    
    if(input_timerButtonGotPressed(other_timer_index)) {
      if(myKitchenTimerList[other_timer_index].hasAlert()){  // acknowledge alert
        myKitchenTimerList[other_timer_index].acknowledgeAlert(); 
      } else {
        if(myKitchenTimerList[other_timer_index].isOver() ||
           myKitchenTimerList[other_timer_index].isDisabled())  // Other time has no duty
           {
            ui_focussed_timer_index=other_timer_index;
            focussed_timer=&myKitchenTimerList[ui_focussed_timer_index];
            input_IgnoreUntilRelease();
           } else {
            output_blockedSequence(myKitchenTimerList,other_timer_index); 
           }       
       } 
     } 
   }
  

  /* Display the encoder value */
  output_renderSetScene(myKitchenTimerList,convertControlvalueToTime(input_getEncoderValue()),ui_focussed_timer_index,ui_partner_timer_index);

}



/* ******************   Helpers    *************************     
 * *********************************************************
 */

/* 
 * Translate encoder value into seconds and back. This implementes the escalating step size when intervals get higher
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
if (value<370) return 86400+(value-274)*1800; // Until 48h min Step 30 min
if (value<538) return 172800+(value-370)*3600;  // Until 168h min Step 60 min
return 604800+(value-538)*10800;  // infinity Step 180 min
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
if(totalSeconds<172800) return (totalSeconds-86400)/1800+274;
if(totalSeconds<604800) return (totalSeconds-172800)/3600+370;
return (totalSeconds-604800)/10800+538;
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
  
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);   

  sound_setup();
  output_setup();
  input_setup(); 

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
          case DISPLAY_MODE:              process_DISPLAY_MODE(); break;
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
