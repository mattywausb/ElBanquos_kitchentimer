/* device driver for the LED Red Green strip 
 * Using a 74HC595 shift register with following setting
 * Output 0: Resistpr to LED 1 Green Cathode, Red Anode
 * Output 1:             LED 1 Green Anode, Red Cathode
 * Output 2: Resistpr to LED 2 Green Cathode, Red Anode
 * Output 3:             LED 3 Green Anode, Red Cathode
 * ....
 */

#include "mainSettings.h"

#ifdef TRACE_ON
//#define TRACE_DEV_LED_RG
#endif


#define LEDRG_DATA_PIN 8
#define LEDRG_LATCH_PIN 7
#define LEDRG_CLOCK_PIN 6


byte current_pattern=0x55;


void dev_led_rg_setup() {
  pinMode(LEDRG_DATA_PIN,OUTPUT);digitalWrite(LEDRG_DATA_PIN,LOW);
  pinMode(LEDRG_CLOCK_PIN,OUTPUT);digitalWrite(LEDRG_CLOCK_PIN,LOW);
  pinMode(LEDRG_LATCH_PIN,OUTPUT);digitalWrite(LEDRG_LATCH_PIN,LOW);
  dev_led_rg_show();
  #ifdef TRACE_ON
    Serial.println(F("TRACE dev_led_rg: setup complete"));
  #endif
}


/* Write current pattern to the led register
 *  
 */
void dev_led_rg_show()
{
   digitalWrite(LEDRG_LATCH_PIN,LOW);
   digitalWrite(LEDRG_CLOCK_PIN,LOW);
   shiftOut(LEDRG_DATA_PIN,LEDRG_CLOCK_PIN,MSBFIRST,current_pattern);
   digitalWrite(LEDRG_LATCH_PIN,HIGH);  
   #ifdef TRACE_DEV_LED_RG
    Serial.print(F("TRACE dev_led_rg: showing="));
    Serial.println(0x100|current_pattern,BIN);
   #endif
}

/* set state of one led to
 *  0 = off
 *  1 = red
 *  2 = green
 *  >2 = off
 */
void dev_led_rg_set(byte led_index,byte new_color) 
{
  #ifdef TRACE_DEV_LED_RG
    Serial.print(F("TRACE dev_led_rg: index="));
    Serial.print(led_index);
    Serial.print(F(" new_color="));
    Serial.print(new_color);
  #endif
  if(led_index>3) return; // dont use invalid input
  new_color&=0x03;

  led_index<<=1; // multiply index by 2 for the upcoming shifts
  byte bitmask=0x03<<led_index;
  bitmask=~bitmask;
  current_pattern=(new_color<<led_index|(current_pattern&bitmask));
  #ifdef TRACE_DEV_LED_RG
    Serial.print(F(" bitmask="));
    Serial.print(0x100|bitmask,BIN);
    Serial.print(F(" current_pattern="));
    Serial.println(0x100|current_pattern,BIN);
  #endif
}

void dev_led_rg_set_immediate(byte led_index,byte color) 
{
  dev_led_rg_set(led_index,color);
  dev_led_rg_show();
}
