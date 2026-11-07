/*
 * robot.ino - uOEC 2026 remote-controlled robot
 * ----------------------------------------------
 * FlySky FS-i6X -> FS-iA6B receiver -> Arduino Uno -> L298N -> 2 drive motors
 *
 * Files in this sketch:
 *   config.h        pins, run mode and tuning values  <- edit this one
 *   RcChannel.*     reads one receiver channel (stick)
 *   Motor.*         drives one motor through the L298N
 *   DriveTrain.*    mixes throttle + steering into left/right speeds
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

Motor leftMotor (Pins::LEFT_EN,  Pins::LEFT_IN1,  Pins::LEFT_IN2,
                 DriveConfig::INVERT_LEFT,  DriveConfig::MIN_PWM, DriveConfig::MAX_PWM);
Motor rightMotor(Pins::RIGHT_EN, Pins::RIGHT_IN3, Pins::RIGHT_IN4,
                 DriveConfig::INVERT_RIGHT, DriveConfig::MIN_PWM, DriveConfig::MAX_PWM);

DriveTrain drive(leftMotor, rightMotor, DriveConfig::STEER_PERCENT);

// ---------------- Drive safety states ----------------
// NoSignal         : no radio -> motors off
// WaitingForCenter : radio OK, but throttle must be centred before driving
// Driving          : normal control
enum class DriveState : uint8_t { NoSignal, WaitingForCenter, Driving };
DriveState state = DriveState::NoSignal;

void setState(DriveState next) {
  if (next == state) return;
  state = next;
  switch (state) {
    case DriveState::NoSignal:         Serial.println(F("[state] NO SIGNAL - motors off")); break;
    case DriveState::WaitingForCenter: Serial.println(F("[state] Centre the right stick to start")); break;
    case DriveState::Driving:          Serial.println(F("[state] DRIVING")); break;
  }
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

void driveLoop() {
  // 1. Lost radio? Stop immediately and require re-centring afterwards.
  if (!rcSteer.hasSignal() || !rcThrottle.hasSignal()) {
    drive.stop();
    setState(DriveState::NoSignal);
    return;
  }

  int throttle = rcThrottle.value();
  int steer    = rcSteer.value();

  // 2. Don't take off on power-up or reconnect if the stick is already pushed.
  if (state != DriveState::Driving) {
    drive.stop();
    setState(throttle == 0 ? DriveState::Driving : DriveState::WaitingForCenter);
    return;
  }

  // 3. Drive.
  drive.arcade(throttle, steer);
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

  switch (RUN_MODE) {
    case RunMode::ReceiverTest: Serial.println(F("Mode: RECEIVER TEST - move the right stick")); break;
    case RunMode::MotorTest:    Serial.println(F("Mode: MOTOR TEST")); break;
    case RunMode::Drive:        Serial.println(F("Mode: DRIVE")); break;
  }
}

void loop() {
  switch (RUN_MODE) {
    case RunMode::ReceiverTest: Tests::receiver(rcSteer, rcThrottle); break;
    case RunMode::MotorTest:    Tests::motors(leftMotor, rightMotor); break;
    case RunMode::Drive:        driveLoop(); break;
  }
}
