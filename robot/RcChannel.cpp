#include "RcChannel.h"
#include "config.h"

RcChannel* RcChannel::s_instances[2] = { nullptr, nullptr };

RcChannel::RcChannel(uint8_t pin) : pin_(pin) {}

bool RcChannel::begin() {
  int irq = digitalPinToInterrupt(pin_);
  if (irq != 0 && irq != 1) {
    return false;  // not D2 or D3
  }
  pinMode(pin_, INPUT);
  s_instances[irq] = this;
  attachInterrupt(irq, irq == 0 ? isrInt0 : isrInt1, CHANGE);
  return true;
}

void RcChannel::isrInt0() { if (s_instances[0]) s_instances[0]->onEdge(); }
void RcChannel::isrInt1() { if (s_instances[1]) s_instances[1]->onEdge(); }

// Runs inside the interrupt: keep it short.
void RcChannel::onEdge() {
  unsigned long now = micros();
  if (digitalRead(pin_) == HIGH) {
    riseUs_ = now;                         // pulse started
  } else {
    unsigned long width = now - riseUs_;   // pulse ended
    if (width >= RcConfig::PULSE_MIN_VALID && width <= RcConfig::PULSE_MAX_VALID) {
      widthUs_     = (uint16_t)width;
      lastPulseMs_ = millis();
    }
  }
}

int RcChannel::pulseUs() const {
  // Copy the shared values with interrupts paused so they can't change mid-read.
  noInterrupts();
  uint16_t width      = widthUs_;
  unsigned long last  = lastPulseMs_;
  interrupts();

  if (width == 0 || millis() - last > RcConfig::SIGNAL_TIMEOUT_MS) {
    return -1;
  }
  return width;
}

bool RcChannel::hasSignal() const {
  return pulseUs() >= 0;
}

int RcChannel::value() const {
  int us = pulseUs();
  if (us < 0) return 0;

  int offset = us - (int)RcConfig::PULSE_CENTER;
  if (abs(offset) < (int)RcConfig::DEADBAND_US) return 0;

  const long halfRange = (RcConfig::PULSE_MAX - RcConfig::PULSE_MIN) / 2;  // 500 us
  long v = (long)offset * 255 / halfRange;
  return (int)constrain(v, -255L, 255L);
}
