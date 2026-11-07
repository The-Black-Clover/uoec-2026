#include "Motor.h"

Motor::Motor(uint8_t enPin, uint8_t inA, uint8_t inB,
             bool inverted, uint8_t minPwm, uint8_t maxPwm)
  : enPin_(enPin), inA_(inA), inB_(inB),
    inverted_(inverted), minPwm_(minPwm), maxPwm_(maxPwm) {}

void Motor::begin() {
  pinMode(enPin_, OUTPUT);
  pinMode(inA_, OUTPUT);
  pinMode(inB_, OUTPUT);
  stop();
}

void Motor::set(int speed) {
  speed = constrain(speed, -255, 255);
  speed_ = speed;
  if (inverted_) speed = -speed;

  if (speed == 0) {
    stop();
    return;
  }

  // Direction
  digitalWrite(inA_, speed > 0 ? HIGH : LOW);
  digitalWrite(inB_, speed > 0 ? LOW  : HIGH);

  // Speed: scale 1..255 onto minPwm..maxPwm so small stick moves still turn the wheel
  int pwm = map(abs(speed), 1, 255, minPwm_, maxPwm_);
  analogWrite(enPin_, pwm);
}

void Motor::stop() {
  speed_ = 0;
  analogWrite(enPin_, 0);
  digitalWrite(inA_, LOW);
  digitalWrite(inB_, LOW);
}
