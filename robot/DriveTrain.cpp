#include "DriveTrain.h"

DriveTrain::DriveTrain(Motor& left, Motor& right, uint8_t steerPercent,
                       uint8_t rampUpStep, uint8_t rampDownStep)
  : left_(left), right_(right), steerPercent_(steerPercent),
    rampUpStep_(rampUpStep), rampDownStep_(rampDownStep) {}

void DriveTrain::begin() {
  left_.begin();
  right_.begin();
}

void DriveTrain::arcade(int throttle, int steer, uint8_t speedPercent) {
  speedPercent = min(speedPercent, (uint8_t)100);

  // Apply steering sensitivity, then the speed-mode limit to both inputs
  // (scaling both keeps the robot's turning radius the same in every mode).
  steer    = (long)steer * steerPercent_ / 100;
  throttle = (long)throttle * speedPercent / 100;
  steer    = (long)steer    * speedPercent / 100;

  int leftTarget  = constrain(throttle + steer, -255, 255);
  int rightTarget = constrain(throttle - steer, -255, 255);

  left_.set(ramp(left_.speed(), leftTarget));
  right_.set(ramp(right_.speed(), rightTarget));
}

int DriveTrain::ramp(int current, int target) const {
  // "Speeding up" = moving away from zero in the same direction.
  // Everything else (slowing down, reversing through zero) uses the
  // faster ramp-down step, so stopping always feels responsive.
  bool speedingUp = (target > current && current >= 0) ||
                    (target < current && current <= 0);
  int step = speedingUp ? rampUpStep_ : rampDownStep_;

  int diff = constrain(target - current, -step, step);
  return current + diff;
}

void DriveTrain::stop() {
  left_.stop();
  right_.stop();
}
