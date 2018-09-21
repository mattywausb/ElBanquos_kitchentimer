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



byte output_blinkCycleFlags=0;

byte output_ledPattern=0;


/*  ************************  setup  ************************************
 *  *********************************************************************
 */

void output_setup() {

}
