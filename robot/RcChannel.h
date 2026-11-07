/*
 * RcChannel - reads one PWM channel from the FlySky receiver
 * ----------------------------------------------------------
 * Times each pulse in the background with an interrupt, so reading a
 * channel never blocks the program (unlike pulseIn()).
 *
 * Two kinds of interrupt are used, depending on the pin:
 *   D2, D3   : external interrupts INT0 / INT1 (attachInterrupt)
 *   A0 - A5  : pin-change interrupt PCINT1 (one shared interrupt for
 *              the whole analog port; the ISR works out which pin moved)
 * Any other pin is rejected by begin().
 */
#pragma once
#include <Arduino.h>

class RcChannel {
public:
  explicit RcChannel(uint8_t pin);

  // Start listening. Returns false if the pin can't be used.
  bool begin();

  // True if a valid pulse arrived recently.
  bool hasSignal() const;

  // Last pulse width in microseconds (~1000-2000), or -1 if no signal.
  int pulseUs() const;

  // Stick position from -255 (full one way) to +255 (full other way).
  // 0 inside the deadband or when there is no signal.
  int value() const;

  // For switches: 0 = low (~1000 us), 1 = middle (~1500 us),
  // 2 = high (~2000 us), or -1 if there is no signal.
  // A 2-position switch only ever returns 0 or 2.
  int switchPosition() const;

  // Called from the interrupt handlers (not for normal use).
  void onEdge(bool pinIsHigh);

private:
  const uint8_t pin_;
  volatile unsigned long riseUs_      = 0;
  volatile uint16_t      widthUs_     = 0;
  volatile unsigned long lastPulseMs_ = 0;
};
