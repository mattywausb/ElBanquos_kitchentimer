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

#define MOCKUP_SELECT_BUTTON 6
#define MOCKUP_PLUS_BUTTON 4
#define MOCKUP_MINUS_BUTTON 2
#define MOCKUP_DEMO_BUTTON 0
#define MOCKUP_TIMER_BUTTON 7
#define MOCKUP_TIMER 3


enum UI_MODES {
  IDLE_MODE, // show time, check for alarms, check age of rds time 
  PRESELECT_MODE,
  SET_MODE,
  DEMO_MODE
};
UI_MODES ui_mode = DEMO_MODE; 

byte ui_focussed_timer_index=3;  // until we have multi timer support we test everything with 3
bool ui_value_changed=false;

TM1638 ledAndKeymodule(4, 3, 2);  // the led+keys module is input and output, so core must own it

KitchenTimer myKitchenTimer;

/* *************** IDLE_MODE ***************** */
void enter_IDLE_MODE(){
  #ifdef TRACE_CLOCK
    Serial.println(F("#IDLE"));
  #endif
  ui_mode=IDLE_MODE;
  output_clearAllSequence ();
}

void process_IDLE_MODE()
{

  if( input_demoButtonGotPressed())
  {
    enter_DEMO_MODE();
    return;
  }

  if(input_timerButtonGotPressed(MOCKUP_TIMER) )
  {
    if(myKitchenTimer.hasAlert()){
      myKitchenTimer.acknowledgeAlert(); 
    } else {
      ui_value_changed=false;
      enter_SET_MODE(MOCKUP_TIMER);
      return;      
    }
  } 

  output_renderIdleScene(myKitchenTimer);
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
void enter_SET_MODE(byte targetTimer)
{
  #ifdef TRACE_CLOCK
    Serial.println(F("#SET_MODE"));
  #endif
  ui_focussed_timer_index=targetTimer;
  if(!ui_value_changed)
  {
    input_setEncoderRange(-200,200,1,false);   // set encoder to track a generic movement 
    input_setEncoderValue(0);  
  }
  ui_mode=SET_MODE;
  output_clearAllSequence ();
}

void process_SET_MODE()
{

   /* UI Fallback, after brief time of no interaction */
   if(input_getSecondsSinceLastEvent()>UI_FALLBACK_INTERVAL) 
   {
     enter_IDLE_MODE();
     return;    
   }

   /* Button of the timer */
   if(input_timerButtonGotPressed(ui_focussed_timer_index)) {
     if(ui_value_changed) {   
       myKitchenTimer.setInterval(convertControlvalueToTime(input_getEncoderValue()));
       myKitchenTimer.startCounting();
     }
     enter_IDLE_MODE();
     return;    
   } 

  /* Select Button */
  if( input_selectGotPressed())
  {
    if(ui_value_changed)  
    { 
          myKitchenTimer.setInterval(convertControlvalueToTime(input_getEncoderValue()));
          myKitchenTimer.startCounting();
          enter_IDLE_MODE();
          return;
    }
    
    if(!myKitchenTimer.isOver())  // This timer is hot
    {       
        #ifdef TRACE_CLOCK
          Serial.println(F("process_SET_MODE >Hold switch"));
        #endif
        if(myKitchenTimer.isOnHold()) 
        {
          myKitchenTimer.startCounting();      
          output_resumeScequence(myKitchenTimer,ui_focussed_timer_index);   
        } else {
          myKitchenTimer.stopCounting();
          output_holdSequence(myKitchenTimer,ui_focussed_timer_index);
        }
            
        enter_IDLE_MODE();
        return;      
     } else { // Timer is over */
        myKitchenTimer.disable();
     }
  }

   /* Button  of other timer */
  if(input_timerButtonGotPressed(2)) // Mockup other Button pressed
  {
    enter_IDLE_MODE();
    return;
  }

  /* React to encoder changes  */
  int initial_encoderValue=0;
  
  if(input_hasEncoderChangeEvent()) {
    input_getEncoderValue(); // acknowledge the processing of the change to the input module
    if(!ui_value_changed) 
    {
      if(myKitchenTimer.isRunning() || myKitchenTimer.isOnHold()) 
      {
        initial_encoderValue=convertTimeToControlvalue(myKitchenTimer.getTimeLeft());                  
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
  if(ui_value_changed) output_renderSetScene(myKitchenTimer,convertControlvalueToTime(input_getEncoderValue()),ui_focussed_timer_index);
  else output_renderSetScene(myKitchenTimer,myKitchenTimer.getTimeLeft(),ui_focussed_timer_index);
}

/* *************** DEMO_MODE ***************** */
void enter_DEMO_MODE()
{
  #ifdef TRACE_CLOCK
    Serial.println(F("#DEMO"));
  #endif
  ui_mode=DEMO_MODE;
  output_clearAllSequence ();
}

void process_DEMO_MODE()
{
  if(input_demoButtonGotPressed()) 
  {
    #ifdef DEBUG_SETTING_1
    myKitchenTimer.disable(); // Resets state completely
    myKitchenTimer.setInterval(72);
    myKitchenTimer.startCounting();
    #endif
  
    enter_IDLE_MODE();
    return;
  }
  
  output_renderDemoScene (input_get_buttonModulePattern()) ;
}
/* ******************   Helpers    *************************     
 * *********************************************************
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

   /* Automated actions */
   if(myKitchenTimer.hasAlert() && myKitchenTimer.getTimeLeft()<-15) myKitchenTimer.acknowledgeAlert(); 


    /* UI logic */
    switch (ui_mode) {
          case IDLE_MODE:                 process_IDLE_MODE(); break;
          case PRESELECT_MODE:            process_PRESELECT_MODE(); break;
          case SET_MODE:                  process_SET_MODE(); break;
          case DEMO_MODE:                 process_DEMO_MODE(); break;
    }
  
}
