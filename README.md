# uoec-2026

Remote-controlled robot for the uOEC 2026 Senior Design category.

**Hardware:** Arduino Uno R3, FlySky FS-i6X transmitter + receiver (PWM output), L298N motor driver, 4 × TT geared motors, 7.2 V NiMH battery.

## Sketches

| Folder | Purpose |
|---|---|
| `rx_test/` | Checks the radio link: prints the CH1 (steering) and CH2 (throttle) pulse widths in µs, flags lost or invalid signal. |

## Receiver wiring (PWM)

| Receiver | Uno |
|---|---|
| CH1 signal | D2 |
| CH2 signal | D3 |
| + | 5V |
| − | GND |
