/*
 * DriveTrain - turns throttle + steering into left/right motor speeds
 * -------------------------------------------------------------------
 * "Arcade" mixing for a tank-style robot:
 *   left  = throttle + steer
 *   right = throttle - steer
 * Push the stick straight left/right and the robot spins in place.
 *
 * Speeds are ramped: each call to arcade() moves the wheels only part of
 * the way toward the stick's target, so the robot never lurches.
 * Call arcade() at a fixed rate (DriveConfig::CONTROL_PERIOD_MS).
 */
#pragma once
#include "Motor.h"

class DriveTrain {
public:
  DriveTrain(Motor& left, Motor& right, uint8_t steerPercent,
             uint8_t rampUpStep, uint8_t rampDownStep);

  void begin();

  // throttle and steer: -255 .. +255
  // speedPercent: top speed limit for the current mode (0-100)
  void arcade(int throttle, int steer, uint8_t speedPercent);

  // Immediate stop, no ramp (used for signal loss and disarming).
  void stop();

  int leftSpeed()  const { return left_.speed(); }
  int rightSpeed() const { return right_.speed(); }

private:
  // Move `current` toward `target` by at most one ramp step.
  int ramp(int current, int target) const;

  Motor& left_;
  Motor& right_;
  const uint8_t steerPercent_;
  const uint8_t rampUpStep_;
  const uint8_t rampDownStep_;
};
