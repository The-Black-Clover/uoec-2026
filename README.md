# uoec-2026

Remote-controlled robot for the uOEC 2026 Senior Design category.

**Hardware:** Arduino Uno R3 (Elegoo), FlySky FS-i6X transmitter + FS-iA6B receiver (PWM output), L298N motor driver, 2 × TT geared drive motors, rear caster, fixed front pusher, 7.2 V NiMH battery.

## Controls

| Transmitter | Channel | Does |
|---|---|---|
| Right stick up/down | CH2 | Forward / backward |
| Right stick left/right | CH1 | Turn (spins in place at full deflection) |
| Switch **SwA** | CH5 | **Arm / disarm** the motors (remote kill switch) |
| Switch **SwC** (3 positions) | CH6 | Speed mode: **PRECISION 40 %** / **NORMAL 70 %** / **TURBO 100 %** |

Safety behaviour:
- Sticks lost → motors cut instantly.
- Disarmed, or CH5 missing → motors cut instantly.
- After power-up, reconnecting or arming, the robot waits until **both** stick axes are centred.
- CH6 missing → PRECISION speed.
- Speed changes are ramped (no lurching); stops from the safety checks skip the ramp.

## Code layout

Everything lives in one sketch, `robot/`. Open `robot/robot.ino` in the Arduino IDE and every file shows up as a tab.

| File | What it does |
|---|---|
| `robot.ino` | Main program: creates the components, runs the selected mode, handles drive safety (signal loss, arm switch, centre-stick-to-start) and speed modes. |
| `config.h` | **All pin numbers, the run mode and tuning values.** Usually the only file you edit. |
| `RcChannel.h/.cpp` | Reads one receiver channel with interrupts (non-blocking): external interrupts on D2/D3, pin-change interrupts on A0–A5. Gives pulse width in µs, a −255…255 stick value, or a switch position. |
| `Motor.h/.cpp` | Drives one L298N channel (speed + direction). |
| `DriveTrain.h/.cpp` | Mixes throttle + steering into left/right speeds (arcade / tank steering) and ramps them. |
| `Tests.h/.cpp` | Bench tests: receiver readout (sticks + switches) and motor direction check. |

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
| CH5 signal | A0 |
| CH6 signal | A1 |
| + | 5V |
| − | GND |

CH1 and CH2 must stay on D2 and D3: they are the Uno's only external-interrupt pins. CH5 and CH6 must be on A0–A5 (pin-change interrupt).

If the two motors on one side fight each other (one mounted the other way round), swap that motor's two wires at the L298N. If a whole side runs backwards, flip `INVERT_LEFT` / `INVERT_RIGHT` in `config.h`.

## Transmitter setup

Set the receiver **failsafe** on the FS-i6X so CH1 and CH2 go to centre when the signal is lost. By default the receiver keeps sending the last stick position, so the robot would keep driving.

1. **Assign the switches:** hold OK → Functions setup → **Aux. channels** → Channel 5 = **SwA**, Channel 6 = **SwC**.
2. **Failsafe:** Functions setup → **Failsafe** → CH1 and CH2 at centre, **CH5 at the disarmed position** (SwA up).
3. **Wrong direction?** Functions setup → **Reverse** → reverse CH2 (forward/back) or CH1 (turning).
4. Check everything in `ReceiverTest` mode: SwA should read pos 0 / 2, SwC pos 0 / 1 / 2. If SwA reads the opposite way, change `ARMED_POSITION` in `config.h`.

No switches wired yet? Set `USE_ARM_SWITCH` and `USE_SPEED_SWITCH` to `false` in `config.h`.
