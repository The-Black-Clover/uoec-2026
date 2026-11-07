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
}

// ======================= Driving =======================
namespace DriveConfig {
  // If a wheel spins backwards in MotorTest mode, flip its setting here.
  constexpr bool INVERT_LEFT  = false;
  constexpr bool INVERT_RIGHT = false;

  // TT motors only hum below about this PWM, so the slowest "moving"
  // speed starts here instead of at 0.
  constexpr uint8_t MIN_PWM = 70;
  constexpr uint8_t MAX_PWM = 255;          // lower (e.g. 180) to limit top speed

  // How much the steering stick affects the wheels, in percent.
  // Lower it (e.g. 60) if turning is too twitchy.
  constexpr uint8_t STEER_PERCENT = 100;
}
