/*
 * rx_test.ino - FlySky receiver check (PWM mode)
 * -----------------------------------------------
 * Purpose : Confirm the FS-i6X -> receiver -> Uno link works BEFORE
 *           any motor code is added. Prints each channel's pulse width.
 *
 * Wiring  : Receiver CH1 signal -> Uno D2   (right stick left/right = steering)
 *           Receiver CH2 signal -> Uno D3   (right stick up/down    = throttle)
 *           Receiver +  (VCC)   -> Uno 5V
 *           Receiver -  (GND)   -> Uno GND
 *
 * Expected: ~1000 us (stick one way), ~1500 us (centre), ~2000 us (other way).
 *           "NO SIGNAL" means the transmitter is off, unbound, or a wire is loose.
 *
 * Open Serial Monitor at 115200 baud.
 */

// ---------------- Pin assignments ----------------
const uint8_t PIN_CH1_STEER    = 2;
const uint8_t PIN_CH2_THROTTLE = 3;

// ---------------- Signal limits ----------------
// FlySky sends a pulse every ~20 ms, so 25 ms is enough to catch one.
// If none arrives, pulseIn() returns 0 and we treat it as signal loss.
const unsigned long PULSE_TIMEOUT_US = 25000;
const int PULSE_MIN_VALID = 900;   // anything outside 900-2100 us is noise
const int PULSE_MAX_VALID = 2100;

/*
 * readChannel()
 * Reads one PWM pulse from the receiver.
 * Returns the pulse width in microseconds, or -1 if the signal is
 * missing or out of range (edge case: lost link / loose wire / noise).
 */
int readChannel(uint8_t pin) {
  unsigned long width = pulseIn(pin, HIGH, PULSE_TIMEOUT_US);
  if (width < PULSE_MIN_VALID || width > PULSE_MAX_VALID) {
    return -1;
  }
  return (int)width;
}

void printChannel(const char* name, int value) {
  Serial.print(name);
  if (value < 0) {
    Serial.print("NO SIGNAL");
  } else {
    Serial.print(value);
    Serial.print(" us");
  }
  Serial.print("   ");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_CH1_STEER, INPUT);
  pinMode(PIN_CH2_THROTTLE, INPUT);
  Serial.println(F("FlySky receiver test - move the right stick"));
}

void loop() {
  printChannel("CH1 steer: ",    readChannel(PIN_CH1_STEER));
  printChannel("CH2 throttle: ", readChannel(PIN_CH2_THROTTLE));
  Serial.println();
  delay(100);   // slow the printout so it's readable
}
