
#define LED_R_PIN 5
#define LED_G_PIN 4

#define BUTTON_A_PIN 3
#define BUTTON_PRESSED LOW

#define TRACE_ON

byte state=0;

void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif

  pinMode(LED_R_PIN,OUTPUT);digitalWrite(LED_R_PIN,LOW);
  pinMode(LED_G_PIN,OUTPUT);digitalWrite(LED_G_PIN,LOW);
  pinMode(BUTTON_A_PIN,INPUT_PULLUP);

}

void loop() {
    
  if(digitalRead(BUTTON_A_PIN)==BUTTON_PRESSED) {
    if(++state>2) state=0;
    switch(state) {
      case 0: digitalWrite(LED_R_PIN,LOW);digitalWrite(LED_G_PIN,LOW);break;
      case 1: digitalWrite(LED_R_PIN,LOW);digitalWrite(LED_G_PIN,HIGH);break;
      case 2: digitalWrite(LED_R_PIN,HIGH);digitalWrite(LED_G_PIN,LOW);break;
    }
    delay(100);
    while(digitalRead(BUTTON_A_PIN)==BUTTON_PRESSED) delay(10);  // Wait until button release
    delay(100);
  }
}
