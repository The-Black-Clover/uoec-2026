/*
 * robot.ino - uOEC 2026 remote-controlled robot
 * ----------------------------------------------
 * FlySky FS-i6X -> FS-iA6B receiver -> Arduino Uno -> L298N -> drive motors
 *
 * Controls (transmitter):
 *   Right stick  up/down    (CH2) : forward / backward
 *   Right stick  left/right (CH1) : turn (spins in place at full deflection)
 *   Switch SwA              (CH5) : ARM / DISARM the motors (remote kill switch)
 *   Switch SwC              (CH6) : speed mode PRECISION / NORMAL / TURBO
 *
 * Files in this sketch:
 *   config.h        pins, run mode and tuning values  <- edit this one
 *   RcChannel.*     reads one receiver channel (stick or switch)
 *   Motor.*         drives one L298N channel
 *   DriveTrain.*    mixes throttle + steering, ramps the speeds
 *   Tests.*         receiver and motor bench tests
 *
 * Choose what runs with RUN_MODE in config.h.
 * Serial Monitor: 115200 baud.
 */
#include "config.h"
#include "RcChannel.h"
#include "Motor.h"
#include "DriveTrain.h"
#include "Tests.h"

// ---------------- Components ----------------
RcChannel rcSteer(Pins::RC_STEER);
RcChannel rcThrottle(Pins::RC_THROTTLE);
RcChannel rcArm(Pins::RC_ARM);
RcChannel rcSpeed(Pins::RC_SPEED);

Motor leftMotor (Pins::LEFT_EN,  Pins::LEFT_IN1,  Pins::LEFT_IN2,
                 DriveConfig::INVERT_LEFT,  DriveConfig::MIN_PWM, DriveConfig::MAX_PWM);
Motor rightMotor(Pins::RIGHT_EN, Pins::RIGHT_IN3, Pins::RIGHT_IN4,
                 DriveConfig::INVERT_RIGHT, DriveConfig::MIN_PWM, DriveConfig::MAX_PWM);

DriveTrain drive(leftMotor, rightMotor, DriveConfig::STEER_PERCENT,
                 DriveConfig::RAMP_UP_STEP, DriveConfig::RAMP_DOWN_STEP);

// ---------------- Drive safety states ----------------
// NoSignal         : sticks not received          -> motors off
// Disarmed         : arm switch off (or missing)  -> motors off
// WaitingForCenter : armed, but both sticks must be centred before driving,
//                    so the robot never takes off on its own after power-up,
//                    reconnecting or arming
// Driving          : normal control
enum class DriveState : uint8_t { NoSignal, Disarmed, WaitingForCenter, Driving };
DriveState state = DriveState::NoSignal;

void setState(DriveState next) {
  if (next == state) return;
  state = next;
  switch (state) {
    case DriveState::NoSignal:         Serial.println(F("[state] NO SIGNAL - motors off")); break;
    case DriveState::Disarmed:         Serial.println(F("[state] DISARMED - flip SwA to arm")); break;
    case DriveState::WaitingForCenter: Serial.println(F("[state] Centre the right stick to start")); break;
    case DriveState::Driving:          Serial.println(F("[state] DRIVING")); break;
  }
}

// ---------------- Switch helpers ----------------
bool isArmed() {
  if (!SwitchConfig::USE_ARM_SWITCH) return true;
  // No signal on CH5 returns -1, which never matches: missing = disarmed.
  return rcArm.switchPosition() == SwitchConfig::ARMED_POSITION;
}

// Speed-mode index from switch SwC: 0 = PRECISION, 1 = NORMAL, 2 = TURBO.
uint8_t speedMode() {
  int pos = rcSpeed.switchPosition();
  return (pos < 0) ? 0 : (uint8_t)pos;   // no signal -> PRECISION (safest)
}

uint8_t speedPercent(uint8_t mode) {
  if (!SwitchConfig::USE_SPEED_SWITCH) return SwitchConfig::SPEED_WITHOUT_SWITCH;
  return SwitchConfig::SPEED_PERCENT[mode];
}

void announceSpeedMode(uint8_t mode) {
  static int lastMode = -1;
  if (!SwitchConfig::USE_SPEED_SWITCH || mode == lastMode) return;
  lastMode = mode;

  static const char* const NAMES[3] = { "PRECISION", "NORMAL", "TURBO" };
  Serial.print(F("[speed] "));
  Serial.print(NAMES[mode]);
  Serial.print(F(" ("));
  Serial.print(SwitchConfig::SPEED_PERCENT[mode]);
  Serial.println(F("%)"));
}

void printDriveDebug(int throttle, int steer) {
  static unsigned long lastPrint = 0;
  if (!DEBUG_PRINT || millis() - lastPrint < 200) return;
  lastPrint = millis();

  Serial.print(F("thr="));   Serial.print(throttle);
  Serial.print(F(" str="));  Serial.print(steer);
  Serial.print(F("  L="));   Serial.print(drive.leftSpeed());
  Serial.print(F(" R="));    Serial.println(drive.rightSpeed());
}

// ---------------- Main drive logic ----------------
void driveLoop() {
  // Run at a fixed rate so the speed ramp behaves predictably.
  static unsigned long lastTick = 0;
  if (millis() - lastTick < DriveConfig::CONTROL_PERIOD_MS) return;
  lastTick = millis();

  // 1. Lost radio? Stop immediately (no ramp).
  if (!rcSteer.hasSignal() || !rcThrottle.hasSignal()) {
    drive.stop();
    setState(DriveState::NoSignal);
    return;
  }

  // 2. Kill switch off? Stop immediately (no ramp).
  if (!isArmed()) {
    drive.stop();
    setState(DriveState::Disarmed);
    return;
  }

  int throttle = rcThrottle.value();
  int steer    = rcSteer.value();

  // 3. Coming from any other state: wait until BOTH stick axes are centred,
  //    otherwise a pushed stick would make the robot drive or spin at once.
  if (state != DriveState::Driving) {
    drive.stop();
    bool centred = (throttle == 0 && steer == 0);
    setState(centred ? DriveState::Driving : DriveState::WaitingForCenter);
    return;
  }

  // 4. Drive, limited by the current speed mode.
  uint8_t mode = speedMode();
  announceSpeedMode(mode);
  drive.arcade(throttle, steer, speedPercent(mode));
  printDriveDebug(throttle, steer);
}

// ---------------- Arduino entry points ----------------
void setup() {
  Serial.begin(SERIAL_BAUD);
  drive.begin();   // motors off before anything else

  if (!rcSteer.begin() || !rcThrottle.begin()) {
    Serial.println(F("ERROR: receiver CH1/CH2 must be on pins D2 and D3."));
    while (true) {}   // stop here; motors stay off
  }
  if (!rcArm.begin() || !rcSpeed.begin()) {
    Serial.println(F("ERROR: receiver CH5/CH6 must be on pins A0-A5."));
    while (true) {}
  }

  switch (RUN_MODE) {
    case RunMode::ReceiverTest: Serial.println(F("Mode: RECEIVER TEST - move the sticks and switches")); break;
    case RunMode::MotorTest:    Serial.println(F("Mode: MOTOR TEST")); break;
    case RunMode::Drive:        Serial.println(F("Mode: DRIVE")); break;
  }
}

void loop() {
  switch (RUN_MODE) {
    case RunMode::ReceiverTest: Tests::receiver(rcSteer, rcThrottle, rcArm, rcSpeed); break;
    case RunMode::MotorTest:    Tests::motors(leftMotor, rightMotor); break;
    case RunMode::Drive:        driveLoop(); break;
  }
}
