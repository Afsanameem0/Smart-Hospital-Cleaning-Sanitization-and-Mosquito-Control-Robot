#include <Servo.h>

#define IN1 9
#define IN2 10
#define IN3 11
#define IN4 12

#define TRIG_LEFT 3
#define ECHO_LEFT 2
#define TRIG_MID 5
#define ECHO_MID 4
#define TRIG_RIGHT 7
#define ECHO_RIGHT 6

#define RELAY_PIN 8
#define MOTION_PIN 13


#define SERVO_SG90 A0     
#define SERVO_MG90 A1     

#define SPRAY_PRESS   40
#define SPRAY_RELEASE 90
Servo sg90;
Servo mg90;
long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 30000);
  if (duration == 0) return 300;
  return duration * 0.034 / 2;
}


void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}
void sprayOnceSafe() {
  stopMotors();
  digitalWrite(RELAY_PIN, LOW);
  delay(200);                  
  sg90.write(SPRAY_PRESS);
  delay(150);                  
  sg90.write(SPRAY_RELEASE);
  delay(350);                 
}


void sweepMG90() {

  // Left to Right
  for (int angle = 30; angle <= 150; angle++) {
    mg90.write(angle);
    delay(10);
  }

  // Right to Left
  for (int angle = 150; angle >= 30; angle--) {
    mg90.write(angle);
    delay(10);
  }
}
void setup() {

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(TRIG_LEFT, OUTPUT);  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_MID, OUTPUT);   pinMode(ECHO_MID, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(MOTION_PIN, INPUT);
  sg90.attach(SERVO_SG90);
  sg90.write(SPRAY_RELEASE);

  mg90.attach(SERVO_MG90);
  mg90.write(90);

  digitalWrite(RELAY_PIN, LOW); // Pump OFF
}


void loop() {

  // Always try to move forward
  forward();

  // Read distances
  long dLeft  = getDistance(TRIG_LEFT, ECHO_LEFT);
  long dMid   = getDistance(TRIG_MID, ECHO_MID);
  long dRight = getDistance(TRIG_RIGHT, ECHO_RIGHT);

  // Servo sweep ONLY occasionally (non-critical)
  static unsigned long lastSweep = 0;
  if (millis() - lastSweep > 2000) {
    sweepMG90();
    lastSweep = millis();
  }

  // Motion detected → spray but resume movement
  if (digitalRead(MOTION_PIN) == HIGH) {
    sprayOnceSafe();
    forward();           
  }

  // Obstacle avoidance
  if (dMid < 25) {
    backward();
    delay(200);

    if (dLeft > dRight) turnLeft();
    else turnRight();

    delay(200);
    forward();           
  }
}

