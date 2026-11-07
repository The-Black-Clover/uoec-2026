#include "DriveTrain.h"

DriveTrain::DriveTrain(Motor& left, Motor& right, uint8_t steerPercent)
  : left_(left), right_(right), steerPercent_(steerPercent) {}

void DriveTrain::begin() {
  left_.begin();
  right_.begin();
}

void DriveTrain::arcade(int throttle, int steer) {
  steer = (long)steer * steerPercent_ / 100;
  left_.set(constrain(throttle + steer, -255, 255));
  right_.set(constrain(throttle - steer, -255, 255));
}

void DriveTrain::stop() {
  left_.stop();
  right_.stop();
}
