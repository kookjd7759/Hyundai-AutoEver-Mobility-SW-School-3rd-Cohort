int size, idx;
void setup() {
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  idx = 0;
  size = 4;
}

void loop() {
  digitalWrite(idx+10, HIGH);
  delay(30);
  digitalWrite(idx+10, LOW);
  delay(30);
  idx = (idx + 1) % size;
}
