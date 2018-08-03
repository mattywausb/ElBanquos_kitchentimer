#include <TM1638.h>
#include "mainSettings.h"
#include "kitchenTimer.h"   // Class declaration

#ifdef TRACE_ON
#define TRACE_CLOCK 1
#define DEBUG_SETTING_1 1
#endif

#define DEFAULT_INTERVAL 600
#define TIMER_MAX_INTERVAL 603800  // Seconds of 7 days

enum UI_MODES {
  IDLE_MODE, // show time, check for alarms, check age of rds time 
  PRESELECT_MODE,
  SET_MODE,
  DEMO_MODE
};
UI_MODES ui_mode = DEMO_MODE; 

byte ui_focussed_timer=0;
long ui_selected_time=300;


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

 if( input_moduleButtonIsPressed(2))
 {
  enter_DEMO_MODE();
  return;
 }


  if(input_moduleButtonIsPressed(7)) 
  {
   
    enter_SET_MODE(3);
    return;
  }

  if(input_moduleButtonIsPressed(7)) 
  {
    myKitchenTimer.acknowledgeAlert(); 
  }  

 if( input_moduleButtonIsPressed(0)&input_moduleButtonIsPressed(7))
 {
  enter_DEMO_MODE();
  return;
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
void enter_SET_MODE(byte targetTimer){
  #ifdef TRACE_CLOCK
    Serial.println(F("#SET_MODE"));
  #endif
  ui_focussed_timer=targetTimer;

  if(myKitchenTimer.isActive()) {
      if(myKitchenTimer.isOver()) ui_selected_time=0;
      else {
        ui_selected_time=NO_TIME_SELECTION;
      }
  } else {
    ui_selected_time=DEFAULT_INTERVAL;
  }
    
  ui_mode=SET_MODE;
}

void process_SET_MODE()
{
   if(input_selectGotPressed()) {
     if(ui_selected_time!=NO_TIME_SELECTION) myKitchenTimer.setInterval(ui_selected_time);
     myKitchenTimer.startCounting();
     enter_IDLE_MODE();
   }

  if(input_moduleButtonIsPressed(__button_of_focussed_timer) )
  {
    
    if(ui_selected_time!=NO_TIME_SELECTION) { // already had a time selection change 
       ui_selected_time=NO_TIME_SELECTION; // switch back to follow the time
    } else {     
      if(myKitchenTimer.isOver()) myKitchenTimer.disable();
      else  if(myKitchenTimer.isOnHold()) myKitchenTimer.startCounting();
            else myKitchenTimer.stopCounting();
    }
  }

  if(input_moduleButtonIsPressed(5)) 
  {
    enter_IDLE_MODE();
  }

  if(input_moduleButtonIsPressed(3)) 
  {
    if(ui_selected_time==NO_TIME_SELECTION) ui_selected_time=myKitchenTimer.getTimeLeft();
    ui_selected_time -= 15;
    if(ui_selected_time<0) ui_selected_time=0;
  }  
  
  if(input_moduleButtonIsPressed(4)) 
  {
    if(ui_selected_time==NO_TIME_SELECTION) ui_selected_time=myKitchenTimer.getTimeLeft();
    ui_selected_time += 15;
    if(ui_selected_time>TIMER_MAX_INTERVAL) ui_selected_time=TIMER_MAX_INTERVAL;
  }  

  
  output_renderSetScene(myKitchenTimer,ui_selected_time,ui_focussed_timer);
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
  if(input_moduleButtonIsPressed(1)) 
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
 *     Main Setup
 * ************************************************************
 */

void setup() {
   #ifdef TRACE_ON 
 Serial.begin(9600);
 Serial.println(F("--------->Kitchen Timer Start<------------"));
 #endif

  output_setup(&ledAndKeymodule);
  input_setup(&ledAndKeymodule); /* Encoder Range 24 hoursis,stepping quater hours */



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
