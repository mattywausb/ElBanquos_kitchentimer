#include "mainSettings.h"

#ifdef TRACE_ON
//#define TRACE_SOUND
#endif

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978





#define SOUND_OUT_PIN 5
#define TRACE_MONITOR_LED_PIN LED_BUILTIN

// notes in the melody:
int notes[] = {
  NOTE_B6, 
   NOTE_A6,
   NOTE_CS7
}; 

const byte sound_pattern [] {  // TODO: compress this into 2 bit per note or put it to code 
            1,1,1,1,1,0,0,0,
            1,1,1,1,1,0,0,0,
            2,2,2,2,2,0,0,0,
            2,2,2,2,2,0,0,0,
            3,3,3,3,3,0,0,0,
            3,3,3,3,3,0,0,0,
            1,1,1,1,1,0,0,0,
            1,1,1,1,1,0,0,0
};

#define SOUND_PATTERN_COUNT sizeof(sound_pattern)/sizeof(sound_pattern[0])
#define NOTE_COUNT sizeof(notes)/sizeof(notes[0])

#define NOTE_DURATION 125
#define PAUSE_DURATION NOTE_DURATION*1.30

byte sound_active_flags=0;
unsigned long sound_last_tick_time=0;

byte sound_current_frame=0;

/*  ************************  interface  ********************************
 *  *********************************************************************
 */

void sound_playAlarmForTimer(byte timer_index)
{

  if(bitRead(sound_active_flags,timer_index)==0) // Sound for this alarm is just triggered
  {
      sound_current_frame=0;
      sound_last_tick_time=0;
      bitSet(sound_active_flags,timer_index);
  }
}

void sound_stopAlarmForTimer(byte timer_index)
{
  bitClear(sound_active_flags,timer_index);
  if(sound_active_flags==0) // this was the last alarm running
  {
    noTone(SOUND_OUT_PIN);
    digitalWrite(TRACE_MONITOR_LED_PIN,LOW);
  }
}

void sound_stopAll(byte timer_index)
{
  sound_active_flags=0;
  noTone(SOUND_OUT_PIN);
  digitalWrite(TRACE_MONITOR_LED_PIN,LOW);
}

/* *************************** TICK *************************************
   Must be called every tick of the core loop
   Manages playing the melody in the background 
*/

void sound_tick()
{
  if(sound_active_flags==0) return;

  if(millis()-sound_last_tick_time>PAUSE_DURATION) 
  {
    sound_last_tick_time=millis();
    if(notes[sound_pattern[sound_current_frame]]>0)
    {
      if(sound_pattern[sound_current_frame]>0)
          tone(SOUND_OUT_PIN, notes[sound_pattern[sound_current_frame]-1], NOTE_DURATION);
          digitalWrite(TRACE_MONITOR_LED_PIN,HIGH);
    } 
    else {
          noTone(SOUND_OUT_PIN);
          digitalWrite(TRACE_MONITOR_LED_PIN,LOW);
    }
    if(++sound_current_frame>=SOUND_PATTERN_COUNT) sound_current_frame=0;
  }
}

/*  ************************  setup  ************************************
 *  *********************************************************************
 */

void sound_setup() {
  pinMode(SOUND_OUT_PIN,OUTPUT);
  digitalWrite(SOUND_OUT_PIN,LOW);   
  #ifdef TRACE_SOUND
  pinMode(TRACE_MONITOR_LED_PIN,OUTPUT);
  digitalWrite(TRACE_MONITOR_LED_PIN,LOW);    
  #endif
}
