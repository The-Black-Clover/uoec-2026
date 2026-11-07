/*
 * Motor - one DC motor on one channel of the L298N
 * ------------------------------------------------
 * enPin  : ENA or ENB (must be a PWM pin) - controls speed
 * inA/inB: IN1/IN2 or IN3/IN4             - control direction
 */
#pragma once
#include <Arduino.h>

class Motor {
public:
  Motor(uint8_t enPin, uint8_t inA, uint8_t inB,
        bool inverted, uint8_t minPwm, uint8_t maxPwm);

  void begin();

  // speed: -255 (full reverse) .. 0 (stop) .. +255 (full forward)
  void set(int speed);

  // Cut power and let the motor coast to a stop.
  void stop();

  int speed() const { return speed_; }

private:
  const uint8_t enPin_, inA_, inB_;
  const bool    inverted_;
  const uint8_t minPwm_, maxPwm_;
  int speed_ = 0;
};
