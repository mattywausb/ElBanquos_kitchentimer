/* Functions to handle all input elements */

#include "mainSettings.h"

// Activate general trace output

#ifdef TRACE_ON
#define TRACE_INPUT 
//#define TRACE_INPUT_HIGH
//#define TRACE_INPUT_ANALOG
//#define TRACE_INPUT_TIMING 
//#define TRACE_INPUT_ACCELERATION
#endif

/* Encoder constants and variables for IDS tracking function */

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define ENCODER_2x_TURN_INTERVAL 80
#define ENCODER_4x_TURN_INTERVAL 40

#define ENCODER_COOLDOWN_TIME 5 // millies to wait until we process another encoder change

volatile unsigned long encoder_a_prev_change_time=0;
volatile unsigned long encoder_b_prev_change_time=0;
volatile bool encoder_a_closed=false;
volatile bool encoder_b_closed=false;

enum ENCODER_STATES {
  FULL_OPEN, 
  FIRST_A_CLOSE,
  FIRST_B_CLOSE,
};

ENCODER_STATES encoder_state = FULL_OPEN;

volatile int encoder_movement = 0;


/* encoder variables */
int input_encoder_value = 0;
int input_encoder_rangeMin = 0;
int input_encoder_rangeMax = 719;
int input_encoder_stepSize = 1;
bool input_encoder_wrap = true;
bool input_encoder_change_event = false;

volatile byte input_encoder_acceleration=1;



/* Button constants */ 
const byte switch_pin_list[] = {4,    //normally 4 BUTTON A SELECT ( ENCODER PUSH)
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

bool input_selectGotPressed()
{

  return input_enabled && ((button_tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_GOT_PRESSED_PATTERN);
}

bool input_selectIsPressed()
{
  return input_enabled && ((button_tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_IS_PRESSED_PATTERN); 
}

byte input_selectGotReleased()
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
  input_encoder_wrap = wrap;
  input_encoder_stepSize = stepSize;
  #ifdef TRACE_INPUT
    Serial.print(F("TRACE_INPUT input_setEncoderRange:"));
    Serial.print(rangeMin); Serial.print(F("-"));
    Serial.print(rangeMax); Serial.print(F(" Step "));
    Serial.print(stepSize); Serial.print(F(" Wrap "));
    Serial.println(wrap, BIN);
  #endif
}

/* Disable input until all buttons have been released */

void input_pauseUntilRelease()
{
  input_enabled=false;
}

/* *************************** internal implementation *************************** */

 
/* **** Encoder movement interrupt service routines
*/

void encoder_pin_a_change_ISR()
{
  byte isClosed=!digitalRead(ENCODER_PIN_A);
  unsigned long turn_event_interval=millis()-encoder_a_prev_change_time;
  
  if(millis()-encoder_a_prev_change_time<ENCODER_COOLDOWN_TIME) return; // we are inside debounce cooldown so get out here
  encoder_a_prev_change_time=millis();
  encoder_a_closed=isClosed;
  
  switch(encoder_state) {
    case FULL_OPEN:   
                      if(encoder_a_closed) 
                      {
                        encoder_state=FIRST_A_CLOSE; // This initiating a turn
                      }
                      break;
    case FIRST_A_CLOSE:
                      if(!encoder_b_closed&&!encoder_a_closed)  encoder_state=FULL_OPEN;  // Turned Back
                      break;
    case FIRST_B_CLOSE:
                      if(!encoder_b_closed && !encoder_a_closed)      // finally turn is complete
                      {
                        encoder_state=FULL_OPEN;
                        encoder_movement+=1;                      
                      }
                      break;
  }
}


void encoder_pin_b_change_ISR()
{
  byte isClosed=!digitalRead(ENCODER_PIN_B);
  unsigned long turn_event_interval=millis()-encoder_b_prev_change_time;
  
  if(turn_event_interval<ENCODER_COOLDOWN_TIME) return; // we are inside debounce cooldown so get out here
  encoder_b_prev_change_time=millis();
  encoder_b_closed=isClosed;
  
  switch(encoder_state) {
    case FULL_OPEN:   
                      if(encoder_b_closed) 
                      {
                        encoder_state=FIRST_B_CLOSE; // This initiating a turn
                      }
                      break;
    case FIRST_B_CLOSE:
                      if(!encoder_b_closed&&!encoder_a_closed)  encoder_state=FULL_OPEN;  // Turned Back
                      break;
    case FIRST_A_CLOSE:
                      if(!encoder_b_closed && !encoder_a_closed)      // finally turn is complete
                      {
                        encoder_state=FULL_OPEN;
                        encoder_movement-=1; 
                      }
                      break;
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
  
  /* regular button scan and encoder crosscheck */
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

    if(encoder_state != FULL_OPEN && digitalRead(ENCODER_PIN_A) && digitalRead(ENCODER_PIN_B)) {   // Both encoder contacts are open but state is not set properly
      if(millis()-encoder_a_prev_change_time> ENCODER_COOLDOWN_TIME
      && millis()-encoder_b_prev_change_time>ENCODER_COOLDOWN_TIME ) // And encoder change time is out of cooldowns      
      {
        encoder_state = FULL_OPEN;
        encoder_a_closed=false;
        encoder_b_closed=false;
      }
      
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
      #ifdef TRACE_INPUT_HIGH
        Serial.print(("TRACE_INPUT_HIGH:press of "));Serial.println(i);
      #endif     
    }
    if((button_tick_state & (INPUT_0_BITS<<(i*2))) == INPUT_0_SWITCHED_OFF_PATTERN<<(i*2)) 
    {
      last_press_end_time=millis();
      change_happened=true;
      #ifdef TRACE_INPUT_HIGH
        Serial.print(("TRACE_INPUT_HIGH:release of "));Serial.println(i);
      #endif     
    }
  }

  /* transfer high resolution encoder movement into tick encoder value */
  int tick_encoder_movement = encoder_movement;  // Freeze the value for upcoming operations
  if (tick_encoder_movement) {
    if(input_enabled && !input_selectIsPressed())
    {
      unsigned long change_interval=millis()-input_last_change_time;
      if(change_interval<ENCODER_4x_TURN_INTERVAL) input_encoder_acceleration=4;
      else if(change_interval<ENCODER_2x_TURN_INTERVAL) input_encoder_acceleration=2;
      else input_encoder_acceleration=1;
      #ifdef TRACE_INPUT_ACCELERATION 
        Serial.print(F("TRACE_INPUT_ACCELERATION "));
        Serial.print(change_interval);Serial.print(F("("));
        Serial.print(input_encoder_acceleration);Serial.println(F(") "));  
      #endif     
      change_happened=true;
      input_encoder_value += tick_encoder_movement * input_encoder_stepSize*input_encoder_acceleration;
      input_encoder_change_event = true;
    }
    encoder_movement -= tick_encoder_movement; // remove the transfered value from the tracking
  }

  /* Wrap or limit the encoder value */
  if (input_encoder_value > input_encoder_rangeMax)      input_encoder_value = input_encoder_wrap ? input_encoder_rangeMin : input_encoder_rangeMax;
  else if (input_encoder_value < input_encoder_rangeMin) input_encoder_value = input_encoder_wrap ? input_encoder_rangeMax : input_encoder_rangeMin;

  if((button_tick_state & INPUT_ALL_BUTTON_STATE_MASK) ==0x00)  input_enabled=true; // enable input when all is released and settled
  #ifdef TRACE_INPUT_HIGH
    else {
      Serial.print(F("TRACE_INPUT_HIGH: not settled "));
      Serial.println(0x8000|button_tick_state,BIN);
    }
  #endif  
  if(change_happened)input_last_change_time = millis(); // Reset the globel age of interaction


} // void input_switches_tick()




/* ***************************       S E T U P           ******************************
*/

void input_setup() {

  /* Initialize switch pins and raw_state_register array */
  for (byte switchIndex = 0; switchIndex < INPUT_ANALOG_PORT_INDEX_OFFSET ; switchIndex++) {
    pinMode(switch_pin_list[switchIndex], INPUT_PULLUP);
  }

  /* Initalize the encoder storage */
  input_setEncoderRange(0, 1, 1, 1); /* Set Ecnoder to binary function  until we get better configuration */

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A),encoder_pin_a_change_ISR,CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B),encoder_pin_b_change_ISR,CHANGE);
 
  setupComplete = true;
}
