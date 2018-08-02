#include <TM1638.h>
#include "mainSettings.h"

#ifdef TRACE_ON
#define TRACE_TIMER 1
#endif

enum UI_MODES {
  IDLE_MODE, // show time, check for alarms, check age of rds time 
  PRESELECT_MODE,
  SET_MODE,
  DEMO_MODE
};

UI_MODES ui_mode = DEMO_MODE; 


TM1638 ledAndKeymodule(4, 3, 2);

/* *************** IDLE_MODE ***************** */
void enter_IDLE_MODE(){
  #ifdef TRACE_CLOCK
    Serial.println(F("#IDLE"));
  #endif
  ui_mode=IDLE_MODE;
}

void process_IDLE_MODE()
{

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
void enter_SET_MODE(){
  #ifdef TRACE_CLOCK
    Serial.println(F("#SET_MODE"));
  #endif
  ui_mode=SET_MODE;
}

void process_SET_MODE()
{

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

   /* Timer tracking */
   // TBD

    /* UI logic */
    switch (ui_mode) {
          case IDLE_MODE:                 process_IDLE_MODE(); break;
          case PRESELECT_MODE:            process_PRESELECT_MODE(); break;
          case DEMO_MODE:                 process_DEMO_MODE(); break;
    }
  
}
