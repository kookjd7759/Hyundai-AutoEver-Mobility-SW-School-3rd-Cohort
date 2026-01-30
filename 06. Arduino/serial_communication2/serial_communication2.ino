void setup() {
  Serial.begin(9600);
}

void loop() {
  char c = 0;
  if (Serial.available()) c = Serial.read();
  if (c == 'a') Serial.println("bts");
  if (c == 'b') Serial.println("exo");
  delay(1000);
}
