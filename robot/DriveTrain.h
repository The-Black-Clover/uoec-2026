/*
 * DriveTrain - turns throttle + steering into left/right motor speeds
 * -------------------------------------------------------------------
 * "Arcade" mixing for a two-wheel (tank-style) robot:
 *   left  = throttle + steer
 *   right = throttle - steer
 * Push the stick straight left/right and the robot spins in place.
 */
#pragma once
#include "Motor.h"

class DriveTrain {
public:
  DriveTrain(Motor& left, Motor& right, uint8_t steerPercent);

  void begin();

  // throttle and steer: -255 .. +255
  void arcade(int throttle, int steer);

  void stop();

  int leftSpeed()  const { return left_.speed(); }
  int rightSpeed() const { return right_.speed(); }

private:
  Motor& left_;
  Motor& right_;
  const uint8_t steerPercent_;
};
