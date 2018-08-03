#include <TM1638.h>
#include "mainSettings.h"
#include "kitchenTimer.h"   // Class declaration

#ifdef TRACE_ON
#define TRACE_CLOCK 1
#define DEBUG_SETTING_1 1
#endif

#define DEFAULT_INTERVAL 600   // !0 Minutes
#define TIMER_MAX_INTERVAL 603800  // Seconds of 7 days
#define UI_FALLBACK_INTERVAL 10000

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

byte ui_focussed_timer=0;
long ui_control_value=96; // 10 Minutes


#define __button_of_focussed_timer (ui_focussed_timer*2)+1


TM1638 ledAndKeymodule(4, 3, 2);

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

  if( input_moduleButtonGotPressed(MOCKUP_DEMO_BUTTON))
  {
    enter_DEMO_MODE();
    return;
  }

  if(input_moduleButtonGotPressed(MOCKUP_TIMER_BUTTON)) 
  {
    if(myKitchenTimer.hasAlert()){
      myKitchenTimer.acknowledgeAlert(); 
    } else {
      enter_SET_MODE(MOCKUP_TIMER,NO_TIME_SELECTION);
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
void enter_SET_MODE(byte targetTimer,long preselected_time){
  #ifdef TRACE_CLOCK
    Serial.println(F("#SET_MODE"));
  #endif
  ui_focussed_timer=targetTimer;
  ui_control_value=convertTimeToControlvalue(preselected_time);
  if(preselected_time==NO_TIME_SELECTION
     && !myKitchenTimer.isActive()) {
            ui_control_value=convertTimeToControlvalue(DEFAULT_INTERVAL);
            }    
  ui_mode=SET_MODE;
  output_clearAllSequence ();
}

void process_SET_MODE()
{
   if(input_selectGotPressed()) {
     if(ui_control_value!=NO_TIME_SELECTION) myKitchenTimer.setInterval(convertControlvalueToTime(ui_control_value));
     myKitchenTimer.startCounting();
     enter_IDLE_MODE();
     return;
   }

  if(input_moduleButtonGotPressed(__button_of_focussed_timer) )
  {
    
    if(ui_control_value==NO_TIME_SELECTION) { //there was no selection change 
      if(myKitchenTimer.isOver()) {       
        myKitchenTimer.disable();
        enter_IDLE_MODE();
        return;
      } else {
        #ifdef TRACE_CLOCK
          Serial.println(F(">Hold switch"));
        #endif
        if(myKitchenTimer.isOnHold()) 
            myKitchenTimer.startCounting();
            else myKitchenTimer.stopCounting();
        enter_IDLE_MODE();
        return;      
      }
      enter_IDLE_MODE();
      return;
    } else {  // Time was changed
      myKitchenTimer.setInterval(convertControlvalueToTime(ui_control_value));
      myKitchenTimer.startCounting();
      enter_IDLE_MODE();
      return;
    }
  }

  if(input_moduleButtonGotPressed(5)) // Mockup other Button pressed
  {
    enter_IDLE_MODE();
    return;
  }

  if(input_moduleButtonGotPressed(MOCKUP_MINUS_BUTTON)) 
  {
    if(ui_control_value==NO_TIME_SELECTION)
      if(myKitchenTimer.hasAlert()) ui_control_value=0;
      else ui_control_value=convertTimeToControlvalue(myKitchenTimer.getTimeLeft());
      
    ui_control_value--;
    if(ui_control_value<0) ui_control_value=0;
  }  
  
  if(input_moduleButtonGotPressed(MOCKUP_PLUS_BUTTON)) 
  {
    if(ui_control_value==NO_TIME_SELECTION)
      if(myKitchenTimer.hasAlert()) ui_control_value=0;
      else ui_control_value=convertTimeToControlvalue(myKitchenTimer.getTimeLeft());
      
    ui_control_value++;
    if(ui_control_value>TIMER_MAX_INTERVAL) ui_control_value=TIMER_MAX_INTERVAL;
  }  

  
  output_renderSetScene(myKitchenTimer,convertControlvalueToTime(ui_control_value),ui_focussed_timer);
}

/* *************** DEMO_MODE ***************** */
void enter_DEMO_MODE(){
  #ifdef TRACE_CLOCK
    Serial.println(F("#DEMO"));
  #endif
  ui_mode=DEMO_MODE;
  output_clearAllSequence ();
}

void process_DEMO_MODE()
{
  if(input_moduleButtonGotPressed(1)) 
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
/* ************************************************************
 *     Helpers
 * ************************************************************
 */

long convertControlvalueToTime(long value)
{
if (value<60) return value;  // First Minute step 1 s   
if (value<96) return 60+(value-60)*15;  // Until h 10min Step 0,25 min
if (value<116) return 600+(value-96)*60;  // Until h 30min Step 1 min
if (value<128) return 1800+(value-116)*150; // Until h 60min Step 2,5 min
if (value<148) return 3600+(value-128)*900; // Until 6h min Step 15 min
if (value<184) return 21600+(value-148)*1800; // Until 24h min Step 30 min
return 86400+(value-184)*3600; //Step 60 min
}

long convertTimeToControlvalue(long totalSeconds)
{
if(totalSeconds<60) return totalSeconds;
if(totalSeconds<600) return (totalSeconds-60)/15+60;
if(totalSeconds<1800) return (totalSeconds-600)/60+96;
if(totalSeconds<3600) return (totalSeconds-1800)/150+116;
if(totalSeconds<21600) return (totalSeconds-3600)/900+128;
if(totalSeconds<86400) return (totalSeconds-21600)/1800+148;
return (totalSeconds-86400)/3600+184;
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

/* ************************************************************
 *     Main Setup
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


  #ifdef TRACE_ON 
  /* Test of stepconversion */
  long timevalue;
  long controlvalue;
  
  for(long i=0;i<215;i++) {
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

/* ************************************************************
 *     Main Loop
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
