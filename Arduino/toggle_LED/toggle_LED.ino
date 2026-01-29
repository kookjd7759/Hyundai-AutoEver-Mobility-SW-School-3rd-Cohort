void setup() {
    pinMode(13 , OUTPUT);
    pinMode(2 , INPUT_PULLUP);
    digitalWrite(13, LOW);
}

void loop() {
    int key_val= 0;
    while(1) {
	    key_val= digitalRead(2);
	    if(key_val==LOW) { digitalWrite(13, HIGH); }
	    else { digitalWrite(13, LOW); }	    
    }
}