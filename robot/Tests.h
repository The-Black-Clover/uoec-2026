/*
 * Tests - bench checks, selected with RUN_MODE in config.h
 */
#pragma once
#include "RcChannel.h"
#include "Motor.h"

namespace Tests {
  // Prints CH1/CH2 pulse widths and stick values. Motors stay off.
  void receiver(const RcChannel& steer, const RcChannel& throttle);

  // Spins each motor forward then backward so you can check direction.
  // Lift the wheels off the table before running this!
  void motors(Motor& left, Motor& right);
}
