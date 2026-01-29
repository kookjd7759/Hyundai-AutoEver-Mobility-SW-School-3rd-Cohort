#define VR_PIN A0

int tmp_cnt= 0;
int adc_val= 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    adc_val= analogRead(VR_PIN);
    Serial.println("["+String(++tmp_cnt)+"]"+String(adc_val));
    delay(100);
}