# uoec-2026

Remote-controlled robot for the uOEC 2026 Senior Design category.

**Hardware:** Arduino Uno R3 (Elegoo), FlySky FS-i6X transmitter + FS-iA6B receiver (PWM output), L298N motor driver, 2 × TT geared drive motors, rear caster, fixed front pusher, 7.2 V NiMH battery.

## Code layout

Everything lives in one sketch, `robot/`. Open `robot/robot.ino` in the Arduino IDE and every file shows up as a tab.

| File | What it does |
|---|---|
| `robot.ino` | Main program: creates the components, runs the selected mode, handles drive safety (signal loss, centre-stick-to-start). |
| `config.h` | **All pin numbers, the run mode and tuning values.** Usually the only file you edit. |
| `RcChannel.h/.cpp` | Reads one receiver channel using a hardware interrupt (non-blocking). Gives pulse width in µs or a −255…255 stick value. |
| `Motor.h/.cpp` | Drives one motor through one L298N channel (speed + direction). |
| `DriveTrain.h/.cpp` | Mixes throttle + steering into left/right motor speeds (arcade / tank steering). |
| `Tests.h/.cpp` | Bench tests: receiver readout and motor direction check. |

## Run modes

Set `RUN_MODE` in `config.h`, then upload. Serial Monitor at **115200 baud**.

| Mode | Use it to |
|---|---|
| `RunMode::ReceiverTest` | Check the radio link. Prints CH1/CH2 in µs (≈1000 / 1500 / 2000). Motors stay off. |
| `RunMode::MotorTest` | Check motor wiring. Spins left then right motor forward and backward. **Lift the wheels first.** If a wheel turns the wrong way on a "forward" step, flip `INVERT_LEFT` / `INVERT_RIGHT`. |
| `RunMode::Drive` | Normal driving with the right stick: up/down = forward/back, left/right = turn. |

Recommended order: ReceiverTest → MotorTest → Drive.

## Wiring

### Power
| From | To |
|---|---|
| Battery red (+) | L298N **+12V** terminal |
| Battery black (−) | L298N **GND** terminal |
| L298N +12V terminal | Uno **VIN** |
| L298N GND terminal | Uno **GND** (shared ground — required) |

Leave the 5V-regulator jumper on the L298N in place (battery is below 12 V).

### Motors
| Motor | L298N |
|---|---|
| Left red / black | OUT1 / OUT2 |
| Right red / black | OUT3 / OUT4 |

### L298N control → Uno
Remove the ENA and ENB jumper caps first.

| L298N | Uno |
|---|---|
| ENA | D5 |
| IN1 | D7 |
| IN2 | D8 |
| IN3 | D10 |
| IN4 | D11 |
| ENB | D6 |

### Receiver (PWM) → Uno
| Receiver | Uno |
|---|---|
| CH1 signal | D2 |
| CH2 signal | D3 |
| + | 5V |
| − | GND |

CH1 and CH2 must stay on D2 and D3: they are the Uno's only external-interrupt pins.

## Transmitter setup

Set the receiver **failsafe** on the FS-i6X so CH1 and CH2 go to centre when the signal is lost. By default the receiver keeps sending the last stick position, so the robot would keep driving.
