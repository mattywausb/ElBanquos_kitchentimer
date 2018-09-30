/*
  Melody

  Plays a melody

  circuit:
  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Tone
*/

#include "mainSettings.h"


#define PIEZO_PIN 5
#define TRIGGER_BUTTON_PIN 4

#include "pitches.h"

// notes in the melody:
int notes[] = {
  NOTE_B6, 
   NOTE_A6,
   NOTE_CS7,
   NOTE_B6
};

#define NOTE_COUNT sizeof(notes)/sizeof(notes[0])

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  8,8,16,16,8, 4, 4
};

#define DURATION_COUNT sizeof(noteDurations)/sizeof(noteDurations[0])

#define DURATION_FULL_NOTE 1000



void setup() {
   #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
   pinMode(TRIGGER_BUTTON_PIN, INPUT_PULLUP);

 input_setup();
   
}

void loop() {
 input_switches_scan_tick();
 if(input_timerButtonGotPressed(0))
 {
   #ifdef TRACE_ON 
    Serial.println("--- Playing ---"); 
  #endif
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < NOTE_COUNT; thisNote++) {
      for(int thisDuration = 0; thisDuration<DURATION_COUNT;thisDuration++) {  
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = DURATION_FULL_NOTE / noteDurations[thisDuration];
        tone(5, notes[thisNote], noteDuration);
    
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(5);
      }
    }

  #ifdef TRACE_ON 
    Serial.println("--- Melody Complete ---"); 
  #endif

 }
}
