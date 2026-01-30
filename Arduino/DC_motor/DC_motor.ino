// define Motor Driver Pins
#define MOTOR_DIR_PIN (12)
#define MOTOR_PWM_PIN (3)
#define MOTOR_BRK_PIN (9)

// define Motor Direction
#define CW LOW
#define CCW HIGH

// define Brake On/Off
#define BRAKE_ON HIGH
#define BRAKE_OFF LOW

void setup() {
    Serial.begin(9600);
    pinMode(MOTOR_DIR_PIN, OUTPUT); // 모터 제어 핀 설정
    pinMode(MOTOR_PWM_PIN, OUTPUT);
    pinMode(MOTOR_BRK_PIN, OUTPUT);
    
    digitalWrite(MOTOR_DIR_PIN, HIGH); // 초기에는 모터 정지
    digitalWrite(MOTOR_PWM_PIN, 0);
    digitalWrite(MOTOR_BRK_PIN, HIGH);
    delay(1000);
}

void loop() {
    digitalWrite(MOTOR_DIR_PIN, CW); // 시계 방향 회전
    digitalWrite(MOTOR_BRK_PIN, BRAKE_OFF); //release breaks
    analogWrite(MOTOR_PWM_PIN, 255); //set work duty for the motor
    Serial.println("motor cw rotate");
    delay(2000);
    
    digitalWrite(MOTOR_BRK_PIN, HIGH);//activate breaks
    Serial.println("motor brake on");
    delay(500);

    digitalWrite(MOTOR_DIR_PIN, CCW); // 반시계 방향 회전
    digitalWrite(MOTOR_BRK_PIN, BRAKE_OFF); //release breaks
    analogWrite(MOTOR_PWM_PIN, 255); //set work duty for the motor to 0 (off)
    Serial.println("motor ccw rotate");
    delay(2000);

    digitalWrite(MOTOR_BRK_PIN, BRAKE_OFF);//activate breaks
    Serial.println("motor brake on");
    delay(500);    
}