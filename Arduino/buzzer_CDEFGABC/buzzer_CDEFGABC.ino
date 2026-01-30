#define BUZ_PIN (5)
#define INTERVAL 10000
#define DELAY_TIME 500

#define KEY_PIN (3)
bool buz_enable= false;

#define C4_PERIOD   (1911)
#define D4_PERIOD   (1703)
#define E4_PERIOD   (1517)
#define F4_PERIOD   (1432)
#define G4_PERIOD   (1276)
#define A4_PERIOD   (1136)
#define B4_PERIOD   (1012)
#define C5_PERIOD   (956)

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
      for(int i= 0; i<200; i++) {
        digitalWrite(BUZ_PIN, HIGH); delayMicroseconds(C4_PERIOD); 
        digitalWrite(BUZ_PIN, LOW); delayMicroseconds(C4_PERIOD);
      }

      for(int i= 0; i<200; i++) {
        digitalWrite(BUZ_PIN, HIGH); delayMicroseconds(D4_PERIOD);
        digitalWrite(BUZ_PIN, LOW); delayMicroseconds(D4_PERIOD);
      }

      for(int i= 0; i<200; i++) {
        digitalWrite(BUZ_PIN, HIGH); delayMicroseconds(E4_PERIOD);
        digitalWrite(BUZ_PIN, LOW); delayMicroseconds(E4_PERIOD);
      }            
      
      for(int i= 0; i<200; i++) {
        digitalWrite(BUZ_PIN, HIGH); delayMicroseconds(F4_PERIOD);
        digitalWrite(BUZ_PIN, LOW); delayMicroseconds(F4_PERIOD);
      }

      for(int i= 0; i<200; i++) {
        digitalWrite(BUZ_PIN, HIGH); delayMicroseconds(G4_PERIOD);
        digitalWrite(BUZ_PIN, LOW); delayMicroseconds(G4_PERIOD);
      }
      
      for(int i= 0; i<200; i++) {
        digitalWrite(BUZ_PIN, HIGH); delayMicroseconds(A4_PERIOD);
        digitalWrite(BUZ_PIN, LOW); delayMicroseconds(A4_PERIOD);
      }
      
      for(int i= 0; i<200; i++) {
        digitalWrite(BUZ_PIN, HIGH); delayMicroseconds(B4_PERIOD);
        digitalWrite(BUZ_PIN, LOW); delayMicroseconds(B4_PERIOD);
      }
    }        
  }
}