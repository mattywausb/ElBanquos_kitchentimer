#include "mainSettings.h"

byte state=0;

void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
  output_setup();
  input_setup();
}

void loop() {

    for(int i;i<4;i++)
    {
      if(input_timerButtonGotPressed(i)) {
        Serial.print("Timer button got pressed:");
        Serial.println(i);
      }
      if(input_timerButtonGotReleased(i)) {
        Serial.print("Timer button got released:");
        Serial.print(i);
        Serial.print(" press duration was:");
        Serial.println(input_getLastPressDuration());
      }
    }
    if(input_selectGotPressed()) {
        Serial.println("Select button got pressed:");
     }
    if(input_selectGotReleased()) {
        Serial.println("Select button got released");
     }
      
}
