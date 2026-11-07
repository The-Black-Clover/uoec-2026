/*
 * RcChannel - reads one PWM channel from the FlySky receiver
 * ----------------------------------------------------------
 * Uses a hardware interrupt to time each pulse in the background,
 * so reading a channel never blocks the program (unlike pulseIn()).
 *
 * Only works on pins D2 and D3 of an Uno (its interrupt pins).
 */
#pragma once
#include <Arduino.h>

class RcChannel {
public:
  explicit RcChannel(uint8_t pin);

  // Start listening. Returns false if the pin can't do interrupts.
  bool begin();

  // True if a valid pulse arrived recently.
  bool hasSignal() const;

  // Last pulse width in microseconds (~1000-2000), or -1 if no signal.
  int pulseUs() const;

  // Stick position from -255 (full one way) to +255 (full other way).
  // 0 inside the deadband or when there is no signal.
  int value() const;

private:
  void onEdge();               // called on every rising/falling edge
  static void isrInt0();
  static void isrInt1();
  static RcChannel* s_instances[2];

  const uint8_t pin_;
  volatile unsigned long riseUs_      = 0;
  volatile uint16_t      widthUs_     = 0;
  volatile unsigned long lastPulseMs_ = 0;
};
