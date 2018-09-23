#include "mainSettings.h"

byte state=0;

byte button_color[4]={0,0,0,0};

void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
  dev_led_rg_setup();
  output_setup();
  input_setup();
  input_setEncoderRange(0,99, 1, true) ;
}

void loop() {
    input_switches_scan_tick();
    for(int i;i<4;i++)
    {
      if(input_timerButtonGotPressed(i)) {
        Serial.print("Timer button got pressed:");
        Serial.println(i);
        if(++button_color[i]>2) button_color[i]=0;
        dev_led_rg_set_immediate(i,button_color[i]);
        output_clearDisplay();
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
    if(input_hasEncoderChangeEvent() )
    {
        Serial.print("Encoder Value:");
        Serial.println(input_getEncoderValue());
    }
    unsigned long slot3=millis()/1000+50+1439L*60;
    switch (button_color[0])
    {
      case 0:
        renderTimeCompact(0,millis()/1000+50,1);
        renderTimeCompact(2,60*59+millis()/1000+50,1);
        renderTimeCompact(3,slot3,1);
        break;
      case LEDRG_GREEN:
        renderTimeLong(60*59+millis()/1000+50);
        break;
      case LEDRG_RED:
        renderTimeLong(slot3);
        break;
    }
      
}
