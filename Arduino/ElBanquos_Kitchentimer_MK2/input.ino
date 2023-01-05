/* Functions to handle all input elements */

#include "mainSettings.h"

// Activate general trace output

#ifdef TRACE_ON
#define TRACE_INPUT 
#define TRACE_INPUT_BUTTONS
#define TRACE_INPUT_ENCODER
//#define TRACE_INPUT_ANALOG
//#define TRACE_INPUT_TIMING 
#endif

/* Encoder constants and variables for IDS tracking function */

#define ENCODER_CLOCK_PIN 2
#define ENCODER_DIRECTION_PIN 3
#define ENCODER_MAX_CHANGE_PER_TICK 100

volatile bool encoder_prev_clock_state=HIGH;
volatile bool encoder_direction_state_start=HIGH;
volatile int8_t encoder_change_value = 0;


/* encoder variables */
int input_encoder_value = 0;
int input_encoder_rangeMin = 0;
int input_encoder_rangeMax = 719;
int input_encoder_stepSize = 1;
int input_encoder_rangeShiftValue=input_encoder_rangeMax-input_encoder_rangeMin+1;
bool input_encoder_wrap = true;
bool input_encoder_change_event = false;


/* Button constants */ 
const byte switch_pin_list[] = {4,    //dial ( ENCODER PUSH)
                                5,     // Timer 1 // Was changed to 5 due to a bug analysis
                                2,     // Timer 2
                                3,     // Timer 3
                                4      // Timer 4 
                               };
                                
#define INPUT_PORT_COUNT sizeof(switch_pin_list)
#define INPUT_ANALOG_PORT_INDEX_OFFSET 1
#define INPUT_ANALOG_HIGH_THRESHOLD 500
#define INPUT_BUTTON_COOLDOWN 10   // Milliseconds until we accept another state change of the buttons



/* Button variables */

unsigned long buttons_last_read_time=0;  // Millis of last butto ncheck
unsigned int button_raw_state=0;         // state of buttons in last button check
unsigned int button_tick_state = 0;      // current and historized state in the actual tick



/* Generic button bit pattern (Button 0) */

#define INPUT_0_BITS 0x0003
#define INPUT_0_IS_ON_PATTERN 0x0003
#define INPUT_0_SWITCHED_ON_PATTERN 0x0001
#define INPUT_0_SWITCHED_OFF_PATTERN 0x0002


/* Element specific  button bit pattern */
/*                                         76543210 */

#define INPUT_BUTTON_A_BITS              0x0003
#define INPUT_BUTTON_A_IS_PRESSED_PATTERN   0x0003
#define INPUT_BUTTON_A_GOT_PRESSED_PATTERN   0x0001
#define INPUT_BUTTON_A_GOT_RELEASED_PATTERN  0x0002

#define INPUT_BUTTON_TIMER1_BITS                  0x000c
#define INPUT_BUTTON_TIMER1_IS_PRESSED_PATTERN    0x000c
#define INPUT_BUTTON_TIMER1_GOT_PRESSED_PATTERN   0x0004
#define INPUT_BUTTON_TIMER1_GOT_RELEASED_PATTERN  0x0008

#define INPUT_ALL_BUTTON_STATE_MASK 0xffff


/* Patterns for history handling */

#define INPUT_CURRENT_BITS 0x0155
#define INPUT_PREVIOUS_BITS 0x02aa

/* Variables for encoder tracking */

unsigned long last_press_start_time=0;
unsigned long last_press_end_time=0;
bool input_enabled=true;

/* General state variables */
volatile bool setupComplete = false;
unsigned long input_last_change_time = 0;

/* ***************************       S E T U P           ******************************
*/

void input_setup() {

  /* Initialize switch pins and raw_state_register array */
  for (byte switchIndex = 0; switchIndex < INPUT_ANALOG_PORT_INDEX_OFFSET ; switchIndex++) {
    pinMode(switch_pin_list[switchIndex], INPUT_PULLUP);
  }

  /* Initalize the encoder storage */
  input_setEncoderRange(0, 1, 1, true); /* Set Ecnoder to binary function  until we get better configuration */

  attachInterrupt(digitalPinToInterrupt(ENCODER_CLOCK_PIN),encoder_clock_change_ISR,CHANGE);
 
  setupComplete = true;
}

/* ********************************************************************************************************** */
/*               Interface functions                                                                          */

/* ------------- General Information --------------- */

int input_getSecondsSinceLastEvent() {
  unsigned long timestamp_difference = (millis() - input_last_change_time) / 1000;
  if (timestamp_difference > 255) return 255;
#ifdef TRACE_INPUT_TIMING
  Serial.print(F("TRACE_INPUT_TIMING:input last interaction:"));
  Serial.println(timestamp_difference);
#endif
  return timestamp_difference;
}

/* ------------- Button events --------------- */

bool input_dialGotPressed()
{

  return input_enabled && ((button_tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_GOT_PRESSED_PATTERN);
}

bool input_dialIsPressed()
{
  return input_enabled && ((button_tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_IS_PRESSED_PATTERN); 
}

byte input_dialGotReleased()
{
  return input_enabled && ((button_tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_GOT_RELEASED_PATTERN); 
}

long input_getCurrentPressDuration()
{
  #ifdef TRACE_INPUT_TIMING
    Serial.print(F("TRACE_INPUT_TIMING:input CurrentPressDuration:"));
    Serial.println(millis()-last_press_start_time);
  #endif
    
  return millis()-last_press_start_time;
}

long input_getLastPressDuration()
{
  return  last_press_end_time-last_press_start_time;
}


bool input_timerButtonGotPressed(byte buttonIndex)
{
  return input_enabled && (button_tick_state & (INPUT_BUTTON_TIMER1_BITS<<buttonIndex*2)) == INPUT_BUTTON_TIMER1_GOT_PRESSED_PATTERN<<buttonIndex*2; 
}

bool input_timerButtonIsPressed(byte buttonIndex)
{
  return input_enabled && (button_tick_state & (INPUT_BUTTON_TIMER1_BITS<<buttonIndex*2)) == INPUT_BUTTON_TIMER1_IS_PRESSED_PATTERN<<buttonIndex*2;
}

bool input_timerButtonGotReleased(byte buttonIndex)
{
  return input_enabled && (button_tick_state & (INPUT_BUTTON_TIMER1_BITS<<buttonIndex*2)) == INPUT_BUTTON_TIMER1_GOT_RELEASED_PATTERN<<buttonIndex*2; 
}

/* ------------- Encoder state --------------- */

bool input_hasEncoderChangeEvent() {
  return input_encoder_change_event;
}

int input_getEncoderValue() {
  input_encoder_change_event = false;
  return input_encoder_value;
}

/* ------------- Operations ----------------- */

void input_setEncoderValue(int newValue) {
  input_encoder_value = newValue;
  if (input_encoder_value < input_encoder_rangeMin) input_encoder_value = input_encoder_rangeMin;
  if (input_encoder_value > input_encoder_rangeMax) input_encoder_value = input_encoder_rangeMax;
  input_encoder_change_event = false;
}

void input_setEncoderRange(int rangeMin, int rangeMax, int stepSize, bool wrap) {
  input_encoder_rangeMin = min(rangeMin, rangeMax);
  input_encoder_rangeMax = max(rangeMin, rangeMax);
  input_encoder_rangeShiftValue=input_encoder_rangeMax-input_encoder_rangeMin+1;
  input_encoder_wrap = wrap;
  input_encoder_stepSize = stepSize;
  #ifdef TRACE_INPUT
    Serial.print(F("TRACE_INPUT input_setEncoderRange:"));
    Serial.print(rangeMin); Serial.print(F("-"));  Serial.print(rangeMax);
    Serial.print(F(" Step ")); Serial.print(stepSize); 
    Serial.print(F(" Wrap ")); Serial.println(wrap, BIN);
  #endif
}

/* Disable input until all buttons have been released */

void input_IgnoreUntilRelease()
{
  input_enabled=false;
}

/* *************************** internal implementation *************************** */

 
/* **** Encoder movement interrupt service routines
*/

void encoder_clock_change_ISR()
{
  bool direction_state=digitalRead(ENCODER_DIRECTION_PIN);
  bool clock_state=digitalRead(ENCODER_CLOCK_PIN);  

  #ifdef TRACE_INPUT_ENCODER
    digitalWrite(LED_BUILTIN, !clock_state);
  #endif

  if(encoder_prev_clock_state && !clock_state) { //clock changes from 1 to 0 (start of cycle in PULL UP logic)
      encoder_direction_state_start=direction_state;  
      encoder_prev_clock_state=clock_state;
      return;
  }

  if(!encoder_prev_clock_state && clock_state) { //clock changes from 1 to 0 (end of cycle PULL UP logic)
      bool encoder_direction_state_end=direction_state;  
      encoder_prev_clock_state=clock_state;
      if(!encoder_direction_state_start) {
        if (encoder_direction_state_end) { // turned clockwise 
          encoder_change_value++;
        }
      } else { 
        if(!encoder_direction_state_end) { // turned counter clockwise
          encoder_change_value--;
        }
      }
      encoder_direction_state_start=encoder_direction_state_end;
  }
}   


/* ************************************* TICK ***********************************************
   translate the state of buttons into the ticks of the master loop
   Must be called by the master loop for every cycle to provide valid event states of
   all input devices.
   Also transfers state changes, tracked with the timer interrupt into a tick state
*/

void input_switches_scan_tick()
{
  bool change_happened=false;
  
  /* regular button scan  */
  if (millis() - buttons_last_read_time > INPUT_BUTTON_COOLDOWN)
  {
    byte isPressed=0;
    int analog_value=0;
    buttons_last_read_time = millis();
    
    for (int i = 0; i <INPUT_PORT_COUNT; i++) { // for all input ports configured
      if(i<INPUT_ANALOG_PORT_INDEX_OFFSET) isPressed=!digitalRead(switch_pin_list[i]);
      else {
        analog_value=analogRead(switch_pin_list[i]);
        isPressed = (analog_value>INPUT_ANALOG_HIGH_THRESHOLD);
        #ifdef TRACE_INPUT_ANALOG
          if(isPressed)
          {
            Serial.print(("TRACE_INPUT_ANALOG:analog value= "));Serial.print(analog_value);
            Serial.print((" of A"));Serial.println(switch_pin_list[i]);
          }
        #endif
      }
      bitWrite(button_raw_state,i*2,isPressed);
    }

  }

  /* copy previous tick  state to history bits  and take raw pattern as new value */
  button_tick_state = (button_tick_state & INPUT_CURRENT_BITS) << 1
               | (button_raw_state & INPUT_CURRENT_BITS);

  
  /* Track pressing time */
  for (int i =0;i<INPUT_PORT_COUNT;i++)
  {
    if((button_tick_state & (INPUT_0_BITS<<(i*2))) == INPUT_0_SWITCHED_ON_PATTERN<<(i*2)) 
    {
      change_happened=true;
      last_press_end_time =last_press_start_time=millis();
      #ifdef TRACE_INPUT_BUTTONS
        Serial.print(("TRACE_INPUT_BUTTONS:press of "));Serial.println(i);
      #endif     
    }
    if((button_tick_state & (INPUT_0_BITS<<(i*2))) == INPUT_0_SWITCHED_OFF_PATTERN<<(i*2)) 
    {
      last_press_end_time=millis();
      change_happened=true;
      #ifdef TRACE_INPUT_BUTTONS
        Serial.print(("TRACE_INPUT_BUTTONS:release of "));Serial.println(i);
      #endif     
    }
  }

  /* transfer high resolution encoder movement into tick encoder value */
  int8_t tick_encoder_change_value = encoder_change_value;  // Freeze the value for upcoming operations
  if (tick_encoder_change_value) { // there are accumulated changes
    if(input_enabled && tick_encoder_change_value<ENCODER_MAX_CHANGE_PER_TICK && tick_encoder_change_value>-ENCODER_MAX_CHANGE_PER_TICK)   {
      input_encoder_value += tick_encoder_change_value * input_encoder_stepSize;
      // Wrap or limit the encoder value 
      while (input_encoder_value > input_encoder_rangeMax) input_encoder_value = input_encoder_wrap ? input_encoder_value-input_encoder_rangeShiftValue : input_encoder_rangeMax;
      while (input_encoder_value < input_encoder_rangeMin) input_encoder_value = input_encoder_wrap ? input_encoder_value+input_encoder_rangeShiftValue : input_encoder_rangeMin;

      input_encoder_change_event = true;
      change_happened=true;
    }
    encoder_change_value -= tick_encoder_change_value; // remove the transfered value from the tracking
    #ifdef TRACE_INPUT_ENCODER
        Serial.print(F("TRACE_INPUT_ENCODER input_switches_scan_tick:"));
        Serial.print(F("\ttick_encoder_change_value=")); Serial.print(tick_encoder_change_value);
        Serial.print(F("\tinput_encoder_value=")); Serial.print(input_encoder_value);
        Serial.print(F("\tencoder_change_value left=")); Serial.println(encoder_change_value);
    #endif
  }
 
  if((button_tick_state & INPUT_ALL_BUTTON_STATE_MASK) ==0x00)  input_enabled=true; // enable input when all is released and settled
  #ifdef TRACE_INPUT_BUTTONS
    else {
      Serial.print(F("TRACE_INPUT_BUTTONS: not settled "));
      Serial.println(0x8000|button_tick_state,BIN);
    }
  #endif  
  if(change_happened)input_last_change_time = millis(); // Reset the globel age of interaction


} // void input_switches_tick()




