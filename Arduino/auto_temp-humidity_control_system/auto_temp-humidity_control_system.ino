#define LED_PIN 13
#define VR_PIN A0

#define MOTOR_DIR_PIN (12)
#define MOTOR_PWM_PIN (3)
#define MOTOR_BRK_PIN (9)

#define CW LOW
#define CCW HIGH

#define BRAKE_ON HIGH
#define BRAKE_OFF LOW

int temperature = 0;
int upper = 20;
int lower = 18;

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_BRK_PIN, OUTPUT);

  digitalWrite(MOTOR_BRK_PIN, BRAKE_ON);
  analogWrite(MOTOR_PWM_PIN, 0);
}

void read_temperature() {
  int adc_val = analogRead(VR_PIN);
  temperature = map(adc_val, 0, 1023, -40, 80);
  Serial.println("(adc,pwm)=(" + String(adc_val) + "," + String(temperature) + ")");
  analogWrite(LED_PIN, adc_val / 4);
}

void loop() {
  read_temperature();

  if (lower <= temperature && temperature <= upper) {
    digitalWrite(MOTOR_BRK_PIN, BRAKE_ON);
    analogWrite(MOTOR_PWM_PIN, 0);

    digitalWrite(9, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    return;
  }

  digitalWrite(MOTOR_BRK_PIN, BRAKE_OFF);
  digitalWrite(9, LOW);

  int center = 19;
  int dist = abs(temperature - center);
  int speed = constrain(dist * 10, 0, 255);

  if (temperature < lower) {
    digitalWrite(MOTOR_DIR_PIN, CW);
    analogWrite(MOTOR_PWM_PIN, speed);

    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
  } 
  else { // temperature > upper
    digitalWrite(MOTOR_DIR_PIN, CCW);
    analogWrite(MOTOR_PWM_PIN, speed);

    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
  }
}
