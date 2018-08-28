
#define LED_R_PIN 3
#define LED_G_PIN 5
#define LED_B_PIN 6

#define BUTTON_A_PIN 2
#define BUTTON_PRESSED LOW

//#define TRACE_ON

int channel_R=0;
int channel_G=0;
int channel_B=0;

void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif

  pinMode(LED_R_PIN,OUTPUT);
  pinMode(LED_G_PIN,OUTPUT);
  pinMode(LED_B_PIN,OUTPUT);
  pinMode(BUTTON_A_PIN,INPUT_PULLUP);

}

void loop() {
    
  channel_R=(millis()/4)%256;
  channel_G=(millis()/16)%256;
  channel_B=(millis()/64)%256;
  analogWrite(LED_R_PIN,channel_R);
  analogWrite(LED_G_PIN,channel_G);
  analogWrite(LED_B_PIN,channel_B);
  #ifdef TRACE_ON
  Serial.print(channel_R);Serial.print(F("|"));
  Serial.print(channel_G);Serial.print(F("|"));
  Serial.print(channel_B);Serial.println();
  #endif
  delay(100);
}
