/* Functions to handle all input elements */

#include "mainSettings.h"

// Activate general trace output

#ifdef TRACE_ON
#define TRACE_INPUT 1
//#define TRACE_INPUT_HIGH 1
#define TRACE_INPUT_traceValue_acceleration
#endif




/* Port constants --> check the IDS Function */

#define PORT_MAIN_SWITCH 8

const byte switch_pin_list[] = {11,  // ENCODER A
                                12,    // ENCODER B
                                10    // BUTTON A SELECT ( ENCODER PUSH)

                               };

const byte button_for_timer[] = {1,3,5,7}; // Mapping of timers to buttons on the board


  
                                

const unsigned int debounce_mask[] = { /* every bit is 2 ms */
  0x0007,    // ENCODER A
  0x0007,    // ENCODER B
  0x0007,    // BUTTON A SELECT ( ENCODER PUSH)
};
#define INPUT_PORT_COUNT sizeof(switch_pin_list)


TM1638 *buttonModule;
byte buttons_current_state = 0;
byte buttons_last_state = 0;
byte buttons_gotPressed_flag = 0;
byte buttons_gotReleased_flag = 0;
unsigned long buttons_last_read_time = 0;
#define BUTTON_MODULE_COOLDOWN 10

volatile bool setupComplete = false;

volatile unsigned int raw_state_register[INPUT_PORT_COUNT];
volatile unsigned int raw_state;
volatile unsigned int debounced_state = 0; /* Debounced state with history to last cycle managed by the ISR */
volatile unsigned long input_last_change_time = 0;


unsigned int tick_state = 0;              /* State provided in the actual tick, with change indication to last tick */

#ifdef TRACE_INPUT_traceValue_acceleration
volatile byte traceValue_acceleration=0;
volatile int traceValue_turn_interval=0;
#endif

#define INPUT_BITIDX_ENCODER_A 0
#define INPUT_BITIDX_ENCODER_B 2
#define INPUT_BITIDX_BUTTON_A 4
#define INPUT_BITIDX_BUTTON_B 6
/*                                         76543210 */
#define INPUT_ENCODER_A_BITS              0x0003
#define INPUT_ENCODER_A_CLOSED_PATTERN    0x0001
#define INPUT_ENCODER_A_OPENED_PATTERN    0x0002

#define INPUT_ENCODER_B_BITS             0x000c
#define INPUT_ENCODER_B_CLOSED_PATTERN   0x0004
#define INPUT_ENCODER_B_OPENED_PATTERN   0x0008

#define INPUT_ENCODER_AB_BITS            0x000f

#define INPUT_BUTTON_A_BITS              0x0030
#define INPUT_BUTTON_A_IS_PRESSED_PATTERN   0x0030
#define INPUT_BUTTON_A_GOT_PRESSED_PATTERN   0x0010
#define INPUT_BUTTON_A_GOT_RELEASED_PATTERN  0x0020



/* Variables for debounce handling */

#define INPUT_CURRENT_BITS 0x5555
#define INPUT_PREVIOUS_BITS 0xaaaa

/* Variables for encoder tracking */

#define ENCODER_IDLE_STATE 0
#define ENCODER_A_FIRST_STATE INPUT_ENCODER_A_CLOSED_PATTERN
#define ENCODER_B_FIRST_STATE INPUT_ENCODER_B_CLOSED_PATTERN

#define ENCODER_2x_TURN_INTERVAL 150
#define ENCODER_4x_TURN_INTERVAL 70


volatile byte encoder_transition_state = 0;
volatile int encoder_movement = 0;

int input_encoder_value = 0;
int input_encoder_rangeMin = 0;
int input_encoder_rangeMax = 719;
int input_encoder_stepSize = 1;
bool input_encoder_wrap = true;
bool input_encoder_change_event = false;

unsigned long last_press_start_time=0;
unsigned long last_press_end_time=0;
bool input_enabled=true;






/* ********************************************************************************************************** */
/*               Interface functions                                                                          */

/* ------------- General Information --------------- */

int input_getSecondsSinceLastEvent() {
  unsigned long timestamp_difference = (millis() - input_last_change_time) / 1000;
  if (timestamp_difference > 255) return 255;
#ifdef TRACE_INPUT_HIGH
  Serial.print(F("input last interaction:"));
  Serial.println(timestamp_difference);
#endif
  return timestamp_difference;
}

/* ------------- Button events --------------- */

bool input_selectGotPressed()
{

  return input_enabled && ((tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_GOT_PRESSED_PATTERN);
}

bool input_selectIsPressed()
{
  return input_enabled && ((tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_IS_PRESSED_PATTERN); 
}

byte input_selectGotReleased()
{
  return input_enabled && ((tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_GOT_RELEASED_PATTERN); 
}

long input_getCurrentPressDuration()
{
  #ifdef TRACE_INPUT
    Serial.print(F("input CurrentPressDuration:"));
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
  return input_enabled && bitRead(buttons_gotPressed_flag, button_for_timer[buttonIndex]); 
}

bool input_timerButtonIsPressed(byte buttonIndex)
{
  return input_enabled && bitRead(buttons_current_state, button_for_timer[buttonIndex]);
}

bool input_timerButtonGotReleased(byte buttonIndex)
{
  return input_enabled && bitRead(buttons_gotReleased_flag, button_for_timer[buttonIndex]); 
}


/* trace function */
byte input_get_buttonModulePattern()
{
  return buttons_current_state;
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
    Serial.print(F("input_setEncoderRange:"));
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

 
/* **** Timer 1 interrupt function to track the state changes direct attached buttons and switches  
*/

ISR(TIMER1_COMPA_vect)
{
  static unsigned long last_turn_event=0;
  unsigned long turn_event_interval=0;
  TCNT1 = 0;             // reset the counter register

  if (!setupComplete) return; 

  /* copy debounce of state of last cycle  to history bits */
  debounced_state = (debounced_state & INPUT_CURRENT_BITS) << 1
                    | (debounced_state & INPUT_CURRENT_BITS);

  /* Get state of all switches */
  for (byte i = 0; i < INPUT_PORT_COUNT; i++) { // for all input ports configured

    // Push  actual reading into the raw state registers
    raw_state_register[i] <<= 1;
    raw_state_register[i] |= !digitalRead(switch_pin_list[i]);
    bitWrite(raw_state, i * 2, raw_state_register[i] & 0x0001); /* and the current status bits */

    // if raw state is stable  copy it to debounced state
    if ((raw_state_register[i] & 0x001f) == 0x0000) bitClear(debounced_state, i << 1);
    else if ((raw_state_register[i]&debounce_mask[i]) == debounce_mask[i]) bitSet(debounced_state, i << 1);
  }

  /* now track the encoder */
  switch (encoder_transition_state) {
    case ENCODER_IDLE_STATE:
      if ((debounced_state & INPUT_ENCODER_AB_BITS)
          == INPUT_ENCODER_A_CLOSED_PATTERN ||
          ((debounced_state & INPUT_ENCODER_AB_BITS)
           == INPUT_ENCODER_B_CLOSED_PATTERN)) {
        encoder_transition_state = debounced_state & INPUT_ENCODER_AB_BITS;
      };
      break;

    case ENCODER_A_FIRST_STATE:
      if (bitRead(debounced_state, INPUT_BITIDX_ENCODER_A) == 0 // A is open
          && ((debounced_state & INPUT_ENCODER_B_BITS) == INPUT_ENCODER_B_OPENED_PATTERN)) { // B Pin just got opened
        encoder_movement++; 
        turn_event_interval=millis()-last_turn_event;
        last_turn_event=millis();    
        if(turn_event_interval<ENCODER_2x_TURN_INTERVAL)
        {
          encoder_movement+=1;
          #ifdef TRACE_INPUT_traceValue_acceleration 
            traceValue_turn_interval=turn_event_interval;
            traceValue_acceleration=2;
          #endif
          if(turn_event_interval<ENCODER_4x_TURN_INTERVAL)
          {
            encoder_movement+=2;                 
            #ifdef TRACE_INPUT_traceValue_acceleration 
              traceValue_acceleration=4;
            #endif
          }
        }
      }
      break;

    case ENCODER_B_FIRST_STATE:
      if (bitRead(debounced_state, INPUT_BITIDX_ENCODER_B) == 0 // B is open
          && ((debounced_state & INPUT_ENCODER_A_BITS) == INPUT_ENCODER_A_OPENED_PATTERN)) { // A Pin just got opened
        encoder_movement--;
        turn_event_interval=millis()-last_turn_event;  
        last_turn_event=millis();  
        if(turn_event_interval<ENCODER_2x_TURN_INTERVAL)
        {
          encoder_movement-=1;
          #ifdef TRACE_INPUT_traceValue_acceleration 
            traceValue_turn_interval=turn_event_interval;
            traceValue_acceleration=2;
          #endif
          if(turn_event_interval<ENCODER_4x_TURN_INTERVAL)
          {
            encoder_movement-=2;                 
            #ifdef TRACE_INPUT_traceValue_acceleration 
              traceValue_acceleration=4;
            #endif
          }
        }
      }
      break;
  };

  /* Reset encoder  transition state, when all debounced states of the encoder contacts are low */
  if ((debounced_state &
       INPUT_ENCODER_AB_BITS &
       INPUT_CURRENT_BITS) == 0) {
    encoder_transition_state = ENCODER_IDLE_STATE;
  }

  /* Reset last change timer if anything has changed */
  if ((debounced_state & INPUT_CURRENT_BITS) != (debounced_state & INPUT_PREVIOUS_BITS) >> 1) input_last_change_time = millis();

}


/* ************************************* TICK ***********************************************
   translate the state of buttons into the ticks of the master loop
   Must be called by the master loop for every cycle to provide valid event states of
   all input devices.
   Also transfers state changes, tracked with the timer interrupt into a tick state
*/

void input_switches_scan_tick() {

  #ifdef TRACE_INPUT_traceValue_acceleration 
   if( traceValue_acceleration) 
   {
    Serial.print(traceValue_turn_interval);Serial.print(F("("));
    Serial.print(traceValue_acceleration);Serial.print(F(") "));
    traceValue_acceleration=0;    
   }
  #endif

  /* copy processed tick  state to history bits  and take debounced as new value */
  tick_state = (tick_state & INPUT_CURRENT_BITS) << 1
               | (debounced_state & INPUT_CURRENT_BITS);

  /* capture buttonsModule states, prevent bouncing with simple cooldown  */
  buttons_last_state = buttons_current_state;
  if (millis() - buttons_last_read_time > BUTTON_MODULE_COOLDOWN)
  {
    buttons_current_state = buttonModule->getButtons();
    buttons_last_read_time = millis();
  }

  /* derive state change information for the button module  */
  buttons_gotPressed_flag = (buttons_last_state ^ buttons_current_state)&buttons_current_state;
  buttons_gotReleased_flag = (buttons_last_state ^ buttons_current_state) & ~buttons_current_state;
  


  if ((buttons_last_state ^ buttons_current_state)) // at least one button changed state
  {
    input_last_change_time = millis(); // Reset the globel age of interaction
    #ifdef TRACE_INPUT
      Serial.print(buttons_current_state, BIN); Serial.print(F("\t "));
      Serial.print(buttons_gotPressed_flag, BIN); Serial.print(F("\t "));
      Serial.println(buttons_gotReleased_flag, BIN);
    #endif
  }

  /* Track pressing time */
  if(buttons_gotPressed_flag || input_selectGotPressed()) last_press_end_time =last_press_start_time=millis();
  if(buttons_gotReleased_flag || input_selectGotReleased()) last_press_end_time=millis();


  /* transfor high resulution encoder movement into encoder value */
  int tick_encoder_movement = encoder_movement;  // Freeze the value for upcoming operations
  if (tick_encoder_movement) {
    if(input_enabled && !input_selectIsPressed())
    {
      input_encoder_value += tick_encoder_movement * input_encoder_stepSize;
      input_encoder_change_event = true;
    }
    encoder_movement -= tick_encoder_movement; // remove the transfered value from the tracking
  }

  /* Wrap or limit the encoder value */
  if (input_encoder_value > input_encoder_rangeMax)      input_encoder_value = input_encoder_wrap ? input_encoder_rangeMin : input_encoder_rangeMax;
  else if (input_encoder_value < input_encoder_rangeMin) input_encoder_value = input_encoder_wrap ? input_encoder_rangeMax : input_encoder_rangeMin;

  if(buttons_current_state==0&& buttons_last_state==0)  input_enabled=true; // enable input when all is released and settled

} // void input_switches_tick()




/* ***************************       S E T U P           ******************************
*/

void input_setup(TM1638 *buttonModuleToUse) {

  /* Register button Module we need to ask */
  buttonModule = buttonModuleToUse;

  /* Initialize switch pins and raw_state_register array */
  for (byte switchIndex = 0; switchIndex < INPUT_PORT_COUNT ; switchIndex++) {
    pinMode(switch_pin_list[switchIndex], INPUT_PULLUP);
    raw_state_register[switchIndex] = 0;
  }
  pinMode(PORT_MAIN_SWITCH, INPUT_PULLUP);

  /* Initalize the encoder storage */
  input_setEncoderRange(0, 1, 1, 1); /* Set Ecnoder to binary function  until we get better configuration */

  // Establish timer1 for regular input scanning

  noInterrupts();           // Alle Interrupts temporär abschalten
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;                // Register mit 0 initialisieren
  OCR1A = 62  ;             // Output Compare für call alle 1 ms https://timer-interrupt-calculator.simsso.de/
  OCR1A = 124  ;             // Output Compare für call alle 2 ms https://timer-interrupt-calculator.simsso.de/
  OCR1A = 186  ;             // Output Compare für call alle 3 ms https://timer-interrupt-calculator.simsso.de/
  TCCR1B |= (1 << CS12);    // 256 als Prescale-Wert spezifizieren
  TIMSK1 |= (1 << OCIE1A);  // Timer Compare Interrupt aktivieren
  interrupts();             // alle Interrupts scharf schalten

  setupComplete = true;
}
