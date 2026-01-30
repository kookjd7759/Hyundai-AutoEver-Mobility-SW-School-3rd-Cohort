#define BUZ_PIN (5)
#define INTERVAL 1000
#define DELAY_TIME 500

#define KEY_PIN (3)
bool buz_enable= false;

void setup() {
    pinMode(BUZ_PIN, OUTPUT);
    pinMode(KEY_PIN, INPUT);
}

void loop() {
  int key_val= 0;
  
  while(1) {
    key_val= digitalRead(KEY_PIN);
    if(key_val==LOW) { buz_enable=true; }
    else { buz_enable=false; }

    if(buz_enable==true) {
      digitalWrite(BUZ_PIN, HIGH);
      delayMicroseconds(INTERVAL);
      digitalWrite(BUZ_PIN, LOW);
      delayMicroseconds(INTERVAL);
    }        
  }
}