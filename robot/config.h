/*
 * config.h - every pin number and tuning value in one place
 * ----------------------------------------------------------
 * If you rewire something or want to tune how the robot drives,
 * this is the only file you should need to change.
 */
#pragma once
#include <Arduino.h>

// ======================= Run mode =======================
// Pick what the sketch does when you upload it:
//   RunMode::ReceiverTest - print stick values, motors stay OFF (start here)
//   RunMode::MotorTest    - spin each motor forward/backward to check direction
//                           (lift the wheels off the table first!)
//   RunMode::Drive        - normal remote-control driving
enum class RunMode : uint8_t { ReceiverTest, MotorTest, Drive };
constexpr RunMode RUN_MODE = RunMode::ReceiverTest;

constexpr unsigned long SERIAL_BAUD = 115200;   // Serial Monitor speed
constexpr bool DEBUG_PRINT = true;              // print values while driving

// ======================= Pins =======================
namespace Pins {
  // FlySky receiver (PWM). These MUST be D2 and D3: they are the Uno's
  // only hardware-interrupt pins, which lets us read the sticks without
  // pausing the rest of the program.
  constexpr uint8_t RC_STEER    = 2;   // CH1 - right stick left/right
  constexpr uint8_t RC_THROTTLE = 3;   // CH2 - right stick up/down

  // Switch channels. These MUST be on A0-A5: they use the Uno's
  // pin-change interrupt, since D2/D3 are already taken by the sticks.
  constexpr uint8_t RC_ARM   = A0;     // CH5 - switch SwA: arm / disarm motors
  constexpr uint8_t RC_SPEED = A1;     // CH6 - switch SwC: 3 speed modes

  // L298N, channel A -> left motor (OUT1/OUT2)
  constexpr uint8_t LEFT_EN  = 5;      // ENA (PWM pin, jumper removed)
  constexpr uint8_t LEFT_IN1 = 7;
  constexpr uint8_t LEFT_IN2 = 8;

  // L298N, channel B -> right motor (OUT3/OUT4)
  constexpr uint8_t RIGHT_EN  = 6;     // ENB (PWM pin, jumper removed)
  constexpr uint8_t RIGHT_IN3 = 10;
  constexpr uint8_t RIGHT_IN4 = 11;
}

// ======================= Receiver =======================
namespace RcConfig {
  constexpr uint16_t PULSE_MIN    = 1000;  // stick fully one way (us)
  constexpr uint16_t PULSE_CENTER = 1500;  // stick centred
  constexpr uint16_t PULSE_MAX    = 2000;  // stick fully the other way

  // Pulses outside this window are treated as noise and ignored.
  constexpr uint16_t PULSE_MIN_VALID = 900;
  constexpr uint16_t PULSE_MAX_VALID = 2100;

  // No valid pulse for this long = signal lost (receiver sends one every ~20 ms).
  constexpr unsigned long SIGNAL_TIMEOUT_MS = 100;

  // Ignore stick movement this close to centre (us), so the robot sits still.
  constexpr uint16_t DEADBAND_US = 25;

  // Switch positions: below LOW_MAX = position 0, above HIGH_MIN = position 2,
  // in between = position 1 (middle of a 3-position switch).
  constexpr uint16_t SWITCH_LOW_MAX  = 1250;
  constexpr uint16_t SWITCH_HIGH_MIN = 1750;
}

// ======================= Switches =======================
namespace SwitchConfig {
  // Arm switch (CH5). Motors can only run while it is in ARMED_POSITION.
  // If CH5 has no signal (wire loose, channel not assigned) the robot
  // stays disarmed: failing safe.
  // Set USE_ARM_SWITCH = false to drive without it (e.g. before it is wired).
  constexpr bool USE_ARM_SWITCH = true;
  constexpr int  ARMED_POSITION = 2;       // 2 = switch down (~2000 us) on SwA

  // Speed switch (CH6). Top speed in percent for each position of SwC.
  // If CH6 has no signal, PRECISION is used (slowest = safest).
  constexpr bool    USE_SPEED_SWITCH = true;
  constexpr uint8_t SPEED_PERCENT[3] = {
    40,    // position 0: PRECISION - docking, lining up on the box
    70,    // position 1: NORMAL    - weaving past obstacles
    100    // position 2: TURBO     - crossing open ground
  };
  constexpr uint8_t SPEED_WITHOUT_SWITCH = 100;   // used when USE_SPEED_SWITCH = false
}

// ======================= Driving =======================
namespace DriveConfig {
  // If a whole SIDE spins backwards in MotorTest mode, flip its setting here.
  // If only ONE motor of a side fights the other (mounted the other way
  // round), swap that motor's two wires at the L298N instead.
  constexpr bool INVERT_LEFT  = false;
  constexpr bool INVERT_RIGHT = false;

  // TT motors only hum below about this PWM, so the slowest "moving"
  // speed starts here instead of at 0.
  constexpr uint8_t MIN_PWM = 70;
  constexpr uint8_t MAX_PWM = 255;          // lower (e.g. 180) to limit top speed

  // How much the steering stick affects the wheels, in percent.
  // Lower it (e.g. 60) if turning is too twitchy.
  constexpr uint8_t STEER_PERCENT = 100;

  // The drive loop runs at a fixed rate so ramping behaves the same
  // no matter how fast the Uno loops.
  constexpr unsigned long CONTROL_PERIOD_MS = 10;   // 100 updates per second

  // Ramping: the most a wheel's speed (-255..255) may change per update.
  // Speeding up gently stops wheelspin and keeps the box from sliding off;
  // slowing down is allowed to be quicker so the robot still stops crisply.
  //   RAMP_UP_STEP   8 -> 0 to full speed in ~0.3 s
  //   RAMP_DOWN_STEP 20 -> full speed to 0 in ~0.13 s
  // Signal loss and disarming skip the ramp and cut the motors at once.
  constexpr uint8_t RAMP_UP_STEP   = 8;
  constexpr uint8_t RAMP_DOWN_STEP = 20;
}
