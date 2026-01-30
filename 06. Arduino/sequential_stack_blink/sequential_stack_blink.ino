int target, idx, last;
void setup() {
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  target = 7;
  idx = 0;
  last = 3;
}

void loop() {
  if (last){
    last--;
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
    delay(300);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    delay(300);
    return;
  }
  digitalWrite(idx+6, HIGH);
  delay(70);
  if (idx == target){
    idx = 0;
    target--;
    if (target == -1){
      last = 3;
      target = 7;
    }
  }
  else{
    digitalWrite(idx+6, LOW);
    delay(70);
    idx++;
  }
}
