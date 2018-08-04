
#include <TM1638.h>

#define TRACE_INPUT 1


TM1638 ledAndKeymodule(4, 3, 2);

enum DEMO_MODE { 
    MODE_LED_CHASE
  , MODE_DIGIT_CHASE
  , MODE_STRING_EXPERIMENT
  };
DEMO_MODE demo_mode=MODE_STRING_EXPERIMENT;

#define MODE_CHANGE_BUTTON 7
#define DISPLAY_INTENSITY 3

void setup() {
  Serial.begin(9600);
  Serial.println(F("--------->Start<------------"));

  // put your setup code here, to run once:
  ledAndKeymodule.setupDisplay(true, DISPLAY_INTENSITY);
  ledAndKeymodule.clearDisplay();
}

void loop() {
   unsigned long milliFixed = millis();
   byte cycle=(milliFixed*2/100)%8; // cycle from 0 to 8, stepping in 2/10s
   byte prevCycle=(cycle-1)%8;
   byte buttons = ledAndKeymodule.getButtons(); // get input (not debounced)

#ifdef TRACE_INPUT
    Serial.println(buttons,BIN);
    #endif

   switch(demo_mode) {
    case MODE_LED_CHASE:
        if(bitRead(buttons,MODE_CHANGE_BUTTON)) {
            demo_mode=MODE_DIGIT_CHASE;
            delay(500);
            break;
        }

        ledAndKeymodule.setLEDs(B10000000>>cycle);
        break;
        
    case MODE_DIGIT_CHASE:
        if(bitRead(buttons,MODE_CHANGE_BUTTON)) {
            demo_mode=MODE_STRING_EXPERIMENT;
            delay(500);
            break;
        }
        if(prevCycle!=cycle) {
            ledAndKeymodule.clearDisplayDigit(prevCycle,false);
            ledAndKeymodule.setDisplayDigit(0,cycle,false); 
        }

    case MODE_STRING_EXPERIMENT:
        if(bitRead(buttons,MODE_CHANGE_BUTTON)) {
            demo_mode=MODE_LED_CHASE;
            delay(500);
            break;
        }  

        ledAndKeymodule.clearDisplay();
        ledAndKeymodule.setDisplayToString("________",B10000001,0);
        delay(2000);
        ledAndKeymodule.setDisplayToString("00",B10000000,3);
        delay(2000);
           
   }
   
      
   
}
