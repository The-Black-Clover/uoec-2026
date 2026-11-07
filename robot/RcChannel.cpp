#include "RcChannel.h"
#include "config.h"

// ---------------- Interrupt bookkeeping ----------------
// Which RcChannel owns each interrupt source.
static RcChannel* s_extInstances[2] = { nullptr, nullptr };   // INT0 (D2), INT1 (D3)
static RcChannel* s_portCInstances[6] = { nullptr };          // A0..A5 (PCINT8..13)
static volatile uint8_t s_lastPortC = 0;                      // port C levels at last change

static void isrInt0() { if (s_extInstances[0]) s_extInstances[0]->onEdge(digitalRead(2) == HIGH); }
static void isrInt1() { if (s_extInstances[1]) s_extInstances[1]->onEdge(digitalRead(3) == HIGH); }

// One interrupt fires for ANY change on A0..A5. Compare the port with its
// previous state to find which pins changed, then notify each owner.
ISR(PCINT1_vect) {
  uint8_t now     = PINC;
  uint8_t changed = now ^ s_lastPortC;
  s_lastPortC = now;

  for (uint8_t bit = 0; bit < 6; bit++) {
    if ((changed & (1 << bit)) && s_portCInstances[bit]) {
      s_portCInstances[bit]->onEdge(now & (1 << bit));
    }
  }
}

// ---------------- RcChannel ----------------
RcChannel::RcChannel(uint8_t pin) : pin_(pin) {}

bool RcChannel::begin() {
  pinMode(pin_, INPUT);

  // D2 / D3: dedicated external interrupts.
  int irq = digitalPinToInterrupt(pin_);
  if (irq == 0 || irq == 1) {
    s_extInstances[irq] = this;
    attachInterrupt(irq, irq == 0 ? isrInt0 : isrInt1, CHANGE);
    return true;
  }

  // A0..A5: pin-change interrupt on port C.
  if (pin_ >= A0 && pin_ <= A5) {
    uint8_t bit = pin_ - A0;
    noInterrupts();
    s_portCInstances[bit] = this;
    s_lastPortC = PINC;          // start from the current levels
    PCMSK1 |= (1 << bit);        // watch this pin
    PCIFR  |= (1 << PCIF1);      // clear any stale pending interrupt
    PCICR  |= (1 << PCIE1);      // enable the port C pin-change interrupt
    interrupts();
    return true;
  }

  return false;  // pin can't be read with an interrupt
}

// Runs inside an interrupt: keep it short.
void RcChannel::onEdge(bool pinIsHigh) {
  unsigned long now = micros();
  if (pinIsHigh) {
    riseUs_ = now;                         // pulse started
  } else {
    unsigned long width = now - riseUs_;   // pulse ended
    if (width >= RcConfig::PULSE_MIN_VALID && width <= RcConfig::PULSE_MAX_VALID) {
      widthUs_     = (uint16_t)width;
      lastPulseMs_ = millis();
    }
    // Otherwise: noise, or a falling edge we caught without its rise. Ignore it.
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

int RcChannel::switchPosition() const {
  int us = pulseUs();
  if (us < 0) return -1;
  if (us < (int)RcConfig::SWITCH_LOW_MAX)  return 0;
  if (us > (int)RcConfig::SWITCH_HIGH_MIN) return 2;
  return 1;
}
