# Mode Switching RC Car Firmware

This project is an Arduino-based RC car firmware that can work in multiple modes:

- Obstacle avoidance
- Manual driving over Bluetooth
- Line following
- Follow-me placeholder

The code is written as one main Arduino sketch plus several header files. If you are new to Arduino, do not worry. This README explains the project in simple language and matches the code that is currently in the folder.

## Project Status

Current status of the project:

- Working: obstacle avoidance with ultrasonic sensor and servo scanning
- Working: manual control commands
- Working: line follower mode
- Partially prepared only: follow-me mode
- Old file kept for reference: `obstacle_avoidance.h` is the first IR-sensor version and is not used by the main sketch now

Important:

- The active obstacle logic is in `obstacle_avoidance_v2.h`
- The main sketch starts in obstacle mode by default
- The logs in this repo show that repeated resets were caused by power/load problems, not by normal mode-switch logic

## What A Fresher Should Know First

### 1. Arduino programs always have `setup()` and `loop()`

- `setup()` runs one time when the board powers on or resets
- `loop()` runs again and again forever

In this project:

- `setup()` initializes serial, Bluetooth, motors, line sensor pins, ultrasonic sensor, and servo
- `loop()` reads Bluetooth commands, then runs the currently selected mode

### 2. All `.h` files in this folder are part of the same sketch

The Arduino IDE compiles the main file and included headers together. That means files like `motor_functions.h`, `servo.h`, and `ultrasonic.h` behave like project modules.

### 3. A "state machine" is used in obstacle mode

A state machine means the robot works step by step:

- Move forward
- Search left and right
- Turn toward a better path
- Return to moving forward

This is easier to debug than mixing all decisions in one block.

## Hardware Used

Typical hardware for this project:

- Arduino Uno or compatible board
- L298N or similar motor driver
- 2 DC motors
- HC-SR04 ultrasonic sensor
- SG90 servo motor
- Bluetooth module
- 2 line sensors
- External battery for motors
- Separate stable 5V supply for servo is strongly recommended

## Current Pin Map

The current pin mapping comes from `pin_map.h`.

| Part | Pin |
| --- | --- |
| Bluetooth RX | `D0` |
| Bluetooth TX | `D1` |
| Left motor PWM | `D5` |
| Right motor PWM | `D6` |
| Left motor IN1 | `D2` |
| Left motor IN2 | `D4` |
| Right motor IN1 | `D7` |
| Right motor IN2 | `D8` |
| Ultrasonic TRIG | `D12` |
| Ultrasonic ECHO | `D3` |
| Servo signal | `D9` |
| Line sensor left | `A0` |
| Line sensor right | `A1` |

Important note for beginners:

- `D0` and `D1` are also the normal hardware serial pins on Arduino Uno
- In the current code, Bluetooth is mapped to `D0` and `D1`
- Because of that, uploading and serial debugging can conflict with Bluetooth wiring
- If upload fails, disconnect the Bluetooth module from `D0` and `D1`, upload the sketch, then reconnect it

## Power And Reset Notes

This is the most important practical lesson from the logs:

- When motors and servo were part of the powered system, the board reset during runtime
- When only the ultrasonic sensor was powered from the Uno 5V, the reset did not happen
- That means the reset problem is caused by power drop, current spikes, or electrical noise, not by normal control flow

Recommended wiring practice:

- Do not power the servo from the Arduino 5V pin during full operation
- Do not power motors from the Arduino board
- Use a separate motor supply through the motor driver
- Use a separate stable 5V source for the servo if possible
- Connect all grounds together:
  - Arduino GND
  - Motor driver GND
  - Servo GND
  - Sensor GND
  - Bluetooth GND
- Add a `470uF` to `1000uF` capacitor near the servo power line

How to recognize a reset in logs:

- `[SYS] ready` is printed at startup
- If `[SYS] ready` appears again in the middle of a run, the board has restarted

## How The Program Works

## Startup Flow

When the board powers on:

1. `Serial.begin(9600)`
2. `BT.begin(9600)`
3. `setupMotors()`
4. `setupLine()`
5. `setupObs()`
6. Print startup logs
7. Start in obstacle mode

Current startup mode:

- `currentMode = OBS_MODE`

## Main Loop Flow

The main loop is simple:

1. Read one Bluetooth command if available
2. Check which mode is active
3. Run only that mode

High-level control flow:

```text
Power On
  -> setup()
  -> initialize modules
  -> [SYS] ready
  -> loop()
     -> readCommand()
     -> switch(currentMode)
        -> LINE_MODE    -> runLineFollower()
        -> OBS_MODE     -> runObstacleAvoidance()
        -> MANUAL_MODE  -> runManual()
        -> FOLLOW_ME    -> followMeRSSI()
```

## Bluetooth Commands

### Mode Selection Commands

| Command | Meaning |
| --- | --- |
| `N` | Switch to line follower mode |
| `O` | Switch to obstacle mode |
| `M` | Switch to manual mode |
| `W` | Switch to follow-me mode |

### Manual Motion Commands

These commands are stored by `handleManualCommand()` and take effect while manual mode is active.

| Command | Meaning |
| --- | --- |
| `F` | Forward |
| `B` | Backward |
| `L` | Turn left |
| `R` | Turn right |
| `S` | Stop |
| `G` | Forward left |
| `I` | Forward right |
| `H` | Backward left |
| `J` | Backward right |

### Speed Commands

Current code behavior:

- If the received Bluetooth character is between `0` and `9`, the code sends it to `setSpeedValue()`
- Obstacle mode also uses internal presets with `setSpeedValue(4)` and `setSpeedValue(9)`

Important for freshers:

- The current sketch does not use long packets like `V120`
- It currently reacts to single digit characters only
- So the README should match the code, not older notes

## Mode Details

## 1. Obstacle Avoidance Mode

This is the default mode at startup.

Main file used:

- `obstacle_avoidance_v2.h`

### Obstacle mode constants

| Name | Value | Meaning |
| --- | --- | --- |
| `BREAK_DIST` | `50` | Start obstacle handling if object is near |
| `STOP_DIST` | `10` | Immediate emergency reaction distance |
| `HIGHEST_SPEED` | `180` | Forward target speed |
| `LOW_SPEED` | `20` | Reduced speed during search stage |
| `CENTER_ANGLE` | `90` | Servo center |
| `LEFT_SCAN_ANGLE` | `170` | Servo scan left |
| `RIGHT_SCAN_ANGLE` | `10` | Servo scan right |

### Obstacle mode state values

| State number in logs | State name | Meaning |
| --- | --- | --- |
| `0` | `MOVE` | Normal forward travel |
| `1` | `SEARCH` | Scan left and right |
| `2` | `TURN` | Turn toward a better path |
| `3` | `ESCAPE` | Defined but not used in current logic |
| `4` | `STOP` | Defined but not used in current logic |

### Real obstacle flow in current code

#### Step A: Emergency front check

At the start of `runObstacleAvoidance()`:

- If the current state is not `TURN`
- And the measured distance is less than or equal to `STOP_DIST`
- The car immediately calls `backward()`
- Then it sets `currentSpeed = 0`
- Then it returns from the function

This is a safety reaction before normal decision flow.

#### Step B: `MOVE` state

In `MOVE` state:

- The code checks the front distance against `BREAK_DIST`
- If the path is clear:
  - target speed becomes `HIGHEST_SPEED`
  - obstacle count is cleared
- If an obstacle is near:
  - state changes to `SEARCH`
  - search starts from left
  - target speed becomes `LOW_SPEED`

#### Step C: `SEARCH` state

In `SEARCH` state:

- The servo moves left step by step until it reaches `170`
- The code waits `100 ms`
- It reads `leftDist`
- Then the servo moves right step by step until it reaches `10`
- The code waits `100 ms`
- It reads `rightDist`
- Then the state changes to `TURN`

#### Step D: `TURN` state

In `TURN` state:

- The code sets a speed preset with `setSpeedValue(4)`
- If both sides are still close:
  - it chooses a backward diagonal turn
- Otherwise:
  - it chooses a forward diagonal turn
- At the same time, the servo is moved back toward center
- Once the servo reaches center:
  - state returns to `MOVE`
  - target speed becomes high again
  - speed preset `9` is applied

### Beginner explanation of servo scan

The servo does not jump instantly from `90` to `170` or `10`.

Instead:

- Every loop it moves by `2` degrees
- The current angle is stored in `servoCurrentAngle`
- This makes the scan easier to watch and easier to debug in logs

### Logs you will see in obstacle mode

Typical log pieces:

- `State:0` means move state
- `State:1` means search state
- `State:2` means turn state
- `dist 31.06 minDist 50` means measured distance was compared against a limit
- `direction 170 servoCurrentAngle 92` means the servo target is 170 and current angle moved to 92

## 2. Manual Mode

Main file used:

- `manual.h`

How it works:

- Bluetooth commands like `F`, `B`, `L`, `R`, `S`, `G`, `I`, `H`, `J` are saved as `manualCmd`
- `runManual()` checks `manualCmd`
- It calls the matching motor function
- If the command is unknown, the car stops

Good beginner note:

- Sending a motion command does not automatically switch to manual mode
- You must first send `M`
- After that, send manual commands

## 3. Line Follower Mode

Main file used:

- `line_follower.h`

How it works:

- Read left line sensor from `A0`
- Read right line sensor from `A1`
- Use simple digital decisions

Current behavior:

| Left sensor | Right sensor | Action |
| --- | --- | --- |
| `0` | `0` | Move forward |
| `1` | `0` | Turn left |
| `0` | `1` | Turn right |
| other | other | Stop |

This is a very simple line follower and is easy for beginners to understand.

## 4. Follow-Me Mode

Main file used:

- `follow_me.h`

Current status:

- `followMeRSSI()` exists as a placeholder only
- No real follow-me logic is implemented yet

This means:

- Sending `W` switches the mode
- But the actual follow-me behavior is not finished yet

## Motor Control Summary

Main file used:

- `motor_functions.h`

This file contains all low-level movement functions:

- `forward()`
- `backward()`
- `left()`
- `right()`
- `forwardLeft()`
- `forwardRight()`
- `backwardLeft()`
- `backwardRight()`
- `stopCar()`

There are also direct-speed functions used by line follower and other logic:

- `moveForward(spd)`
- `moveBackward(spd)`
- `turnLeft(spd)`
- `turnRight(spd)`
- `setMotor(left, right)`

Important beginner note:

- `setMotor(left, right)` currently always drives the car in forward direction and only changes PWM values
- Direction changes are handled by the dedicated movement functions above

## File Guide

| File | Purpose |
| --- | --- |
| `mode_switching.ino` | Main sketch, setup, loop, command reading, mode switching |
| `pin_map.h` | All pin definitions in one place |
| `motor_functions.h` | Low-level motor control functions |
| `manual.h` | Manual mode command execution |
| `line_follower.h` | Simple line follower logic |
| `ultrasonic.h` | Distance reading and obstacle comparison |
| `servo.h` | Servo scan movement and angle helpers |
| `obstacle_avoidance_v2.h` | Active obstacle avoidance logic |
| `obstacle_avoidance.h` | Old unused IR-based version |
| `follow_me.h` | Follow-me placeholder |
| `logs.txt` | Earlier log showing resets during powered servo/motor testing |
| `logs2.text` | Log captured with only ultrasonic powered from Uno 5V |

## How To Read The Two Log Files

### `logs.txt`

What it showed:

- `[SYS] ready` appeared multiple times
- That means the board restarted during runtime
- There were also corrupted characters before restart text
- This strongly points to power drop or electrical noise

### `logs2.text`

What it showed:

- `[SYS] ready` appeared only once at the beginning
- No repeated startup banner was found
- This test was done without powering motors and servo from the running setup
- So the program flow continued normally

Conclusion from both logs:

- Reset issue is hardware power related
- The core obstacle logic is not the main reset cause

## Upload And Run Guide

1. Open the project in Arduino IDE
2. Select the correct board and COM port
3. If Bluetooth is connected on `D0/D1`, disconnect it before uploading
4. Upload the sketch
5. Reconnect Bluetooth if needed
6. Power motors and servo from proper external supplies
7. Open Bluetooth app and send mode commands

Recommended first test order:

1. Test obstacle mode with only ultrasonic and logs
2. Test manual mode with motors
3. Add servo power with a separate 5V source
4. Finally test the full car

## Troubleshooting

### Problem: Board resets while running

Check:

- Is the servo powered from Arduino 5V
- Are motors sharing the same weak supply
- Are all grounds connected together
- Is there a capacitor near servo supply

Quick reset clue:

- Repeated `[SYS] ready` means reset happened

### Problem: No distance is detected correctly

Check:

- TRIG and ECHO wiring
- Sensor power and GND
- Object surface and angle
- `300.00` often means no echo or timeout

### Problem: Manual commands do nothing

Check:

- Did you send `M` first
- Is Bluetooth connected
- Are TX/RX pins wired correctly

### Problem: Serial monitor and Bluetooth interfere

Check:

- Current Bluetooth pins are `D0/D1`
- Those pins overlap with normal serial communication on Uno

### Problem: Follow-me mode does nothing

Reason:

- It is not implemented yet in the current code

## Suggested Learning Order For Freshers

If you want to understand this project quickly, study files in this order:

1. `mode_switching.ino`
2. `pin_map.h`
3. `motor_functions.h`
4. `manual.h`
5. `line_follower.h`
6. `ultrasonic.h`
7. `servo.h`
8. `obstacle_avoidance_v2.h`

This order goes from easiest to more advanced.

## Flow Diagram URL
[View Flow Diagram]()

## Final Summary

This project is a multi-mode Arduino RC car firmware with a working obstacle avoider, manual driving, and line follower. The main logic is organized around a simple mode switch and an obstacle state machine. The recent debug logs show that runtime resets came from power/load issues when motors and servo were powered, not from normal obstacle logic flow.
