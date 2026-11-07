#include "Tests.h"

namespace {
  void printChannel(const __FlashStringHelper* name, const RcChannel& ch) {
    Serial.print(name);
    int us = ch.pulseUs();
    if (us < 0) {
      Serial.print(F("NO SIGNAL     "));
    } else {
      Serial.print(us);
      Serial.print(F(" us ("));
      Serial.print(ch.value());
      Serial.print(F(")   "));
    }
  }

  void printSwitch(const __FlashStringHelper* name, const RcChannel& ch) {
    Serial.print(name);
    int pos = ch.switchPosition();
    if (pos < 0) {
      Serial.print(F("NO SIGNAL   "));
    } else {
      Serial.print(F("pos "));
      Serial.print(pos);
      Serial.print(F(" ("));
      Serial.print(ch.pulseUs());
      Serial.print(F(" us)   "));
    }
  }

  void runStep(Motor& motor, int speed, const __FlashStringHelper* label) {
    Serial.println(label);
    motor.set(speed);
    delay(1500);
    motor.stop();
    delay(500);
  }
}

void Tests::receiver(const RcChannel& steer, const RcChannel& throttle,
                     const RcChannel& arm, const RcChannel& speed) {
  // Sticks: expect ~1000 / ~1500 / ~2000 us.
  // Switches: SwA should give pos 0 / 2, SwC pos 0 / 1 / 2.
  // "NO SIGNAL" = transmitter off, receiver not bound, or a loose wire.
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint < 100) return;
  lastPrint = millis();

  printChannel(F("CH1 steer: "), steer);
  printChannel(F("CH2 throttle: "), throttle);
  printSwitch(F("CH5 arm: "), arm);
  printSwitch(F("CH6 speed: "), speed);
  Serial.println();
}

void Tests::motors(Motor& left, Motor& right) {
  // Each wheel should roll so the robot would move FORWARD on the
  // "forward" steps. If not, flip INVERT_LEFT / INVERT_RIGHT in config.h.
  Serial.println(F("\nMotor test starting in 3 s - wheels off the table!"));
  delay(3000);

  runStep(left,   180, F("LEFT  forward"));
  runStep(left,  -180, F("LEFT  backward"));
  runStep(right,  180, F("RIGHT forward"));
  runStep(right, -180, F("RIGHT backward"));

  Serial.println(F("Done. Repeating in 5 s..."));
  delay(5000);
}
