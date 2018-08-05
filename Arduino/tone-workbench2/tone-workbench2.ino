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

#define PIEZO_PIN 5
#define TRIGGER_BUTTON_BIN 8

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


#define DURATION_COUNT sizeof(noteDurations)/sizeof(noteDurations[0])

#define NOTE_DURATION 125
#define NOTE_PAUSE NOTE_DURATION * 0.30


void setup() {
   pinMode(TRIGGER_BUTTON_BIN, INPUT_PULLUP);
   pinMode(LED_BUILTIN,OUTPUT);
}

void loop() {

 if(!digitalRead(TRIGGER_BUTTON_BIN))
 {
  
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < NOTE_COUNT; thisNote++)
    {
      for(byte repeat =0;repeat<2;repeat++)
      {
        for(int thisDuration = 0; thisDuration<5;thisDuration++) {  
          digitalWrite(LED_BUILTIN,HIGH);
          tone(5, notes[thisNote], NOTE_DURATION);
      
          // to distinguish the notes, set a minimum time between them.
          // the note's duration + 30% seems to work well:
          delay(NOTE_DURATION);
          digitalWrite(LED_BUILTIN,LOW);
          delay(NOTE_PAUSE);
          // stop the tone playing:
          noTone(5);
        }
         delay(NOTE_PAUSE+NOTE_DURATION);
      }
    }
 }


}
