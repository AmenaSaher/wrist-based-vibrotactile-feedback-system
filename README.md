
# Wrist-Based Vibrotactile Feedback System Using Linear Resonant Actuators

Master of Science in Computer Science project  
California State University, Sacramento | Fall 2026

## Overview

This project developed a four-location wrist-based vibrotactile feedback prototype using Linear Resonant Actuators (LRAs). The system was designed to investigate vibration detection and recognition of individual and combined stimulation patterns around the wrist.

The prototype integrates an Arduino Mega 2560, a TCA9548A I²C multiplexer, four DRV2605L haptic drivers, four Precision Microdrives C10-100 LRAs, and MATLAB.

Two single-subject proof-of-concept experiments were conducted:
1. Adaptive vibration detection
2. Wrist-location and vibration-pattern recognition

The project evaluates an LRA-based implementation and does not experimentally compare ERM and LRA motors.

## Hardware Components

- Arduino Mega 2560
- TCA9548A I²C Multiplexer
- 4 Adafruit DRV2605L Haptic Motor Drivers
- 4 Precision Microdrives C10-100 Linear Resonant Actuators
- Computer running MATLAB
- USB connection
- Breadboard and connecting wires
- Medical paper tape for actuator attachment

### Motor Assignments

| Motor | Multiplexer Channel | Wrist Location | Recognition RTP |
|---|---|---|---|
| A | 4 | Dorsal | 8 |
| B | 5 | Radial | 12 |
| C | 6 | Ulnar | 18 |
| D | 7 | Volar | 22 |

The LRAs were attached to the left wrist at four anatomical locations.

## System Architecture

MATLAB manages experimental trials, response collection, and data logging.

The Arduino Mega 2560 receives commands from MATLAB through USB serial communication at 115200 baud.

The TCA9548A multiplexer allows the Arduino to communicate with four DRV2605L drivers sharing the same I²C address.

Each driver controls one LRA.

The system supports:
- Independent motor activation
- Programmable RTP drive settings
- Programmable vibration duration
- Simultaneous two-motor stimulation
- Sequential two-motor stimulation
- Automated experimental data collection

RTP values represent digital drive commands and are not calibrated physical vibration measurements.

## Experiment 1: Adaptive Vibration Detection

### Objective

The first experiment investigated commanded vibration levels near the detection boundary at four wrist locations.

### Methodology

Four independent adaptive staircases were implemented using a 1-up/3-down procedure.

Each staircase:
- Started at RTP 20
- Used an adjustment step of 2
- Operated within RTP 2–40
- Increased RTP after one missed vibration
- Decreased RTP after three consecutive detected vibrations
- Stopped after six reversals or the programmed maximum of 45 stimulation trials

Each vibration pulse lasted 1000 milliseconds.

The participant pressed SPACE when a vibration was detected. No key press indicated that no vibration was detected.

Responses were collected within a nominal 1.5-second window.

No-stimulation catch trials were randomly included with a programmed probability of 12%.

### Results

The completed experiment consisted of:
- 162 total trials
- 143 motor-stimulation trials
- 19 catch trials

All four staircases reached six reversals.

| Motor | Wrist Location | Exploratory Mean RTP |
|---|---|---|
| A | Dorsal | 6.5 |
| B | Radial | 12.5 |
| C | Ulnar | 13.0 |
| D | Volar | 17.5 |

The exploratory RTP estimates were calculated by averaging the final four reversal levels.

All 19 catch trials were correctly answered as no stimulation.

These values describe commanded drive levels near the detection boundary and should not be interpreted as calibrated absolute vibration thresholds.

## Experiment 2: Wrist Location and Pattern Recognition

### Objective

The second experiment evaluated recognition of individual wrist locations, simultaneous motor pairs, and ordered sequential motor pairs.

Recognition RTP settings were selected using repeated successful detections from Experiment 1.

### Experimental Conditions

The experiment consisted of three blocks presented in a fixed order.

| Condition | Stimulation Trials | Catch Trials |
|---|---|---|
| Single motor | 20 | 5 |
| Simultaneous pairs | 30 | 5 |
| Sequential pairs | 60 | 5 |

The experiment included 125 total trials.

Patterns were randomized within each block.

Each motor pulse lasted 1000 milliseconds.

For simultaneous stimulation, two motors were commanded during the same pulse interval.

For sequential stimulation, the first motor was activated for 1000 milliseconds, followed by the second motor for another 1000 milliseconds.

Responses were entered using motor labels A, B, C, and D, or N for no stimulation.

### Results

| Condition | Correct Responses | Accuracy |
|---|---|---|
| Single Motor | 13/20 | 65.0% |
| Simultaneous Pairs | 8/30 | 26.7% |
| Ordered Sequential Pairs | 8/60 | 13.3% |
| Catch Trials | 15/15 | 100.0% |

All 110 motor-stimulation commands returned DONE acknowledgments.

For sequential patterns, location-only recognition accuracy, ignoring presentation order, was 20.0% (12/60).

### Observations

Single-motor stimulation achieved the highest recognition accuracy.

Recognition was more difficult for simultaneous and sequential patterns under the tested conditions.

Some paired combinations involving Motors A and B were particularly difficult to distinguish.

These results suggest that selecting stimulation settings based on successful detection responses alone may not be sufficient for reliable recognition of combined vibration patterns.

Because the experiment involved one participant and fixed block order, the findings are descriptive and cannot establish general differences in recognition difficulty across populations.

## Repository Contents

- `arduino/LRA_Recognition_Arduino.ino` — Arduino motor-control firmware
- `matlab/LRA_Staircase_Program.m` — Adaptive detection experiment
- `matlab/LRA_Recognition.m` — Wrist-location and pattern-recognition experiment
- `data/LRA_Staircase_20261022_173121.csv` — Detection experiment data
- `data/LRA_Recognition_Blocked_20261102_121720.csv` — Recognition experiment data

## Running the Programs

### Arduino Setup

1. Connect the Arduino Mega 2560 to the TCA9548A multiplexer.
2. Connect the four DRV2605L drivers to multiplexer channels 4–7.
3. Connect each LRA to its corresponding driver.
4. Install the Adafruit DRV2605 library in Arduino IDE.
5. Upload `LRA_Recognition_Arduino.ino` to the Arduino Mega 2560.

The firmware uses `Wire.h` and `Adafruit_DRV2605.h`.

### MATLAB Setup

1. Connect the Arduino to the computer using USB.
2. Open the MATLAB experimental program.
3. Configure the serial port to match the connected Arduino. The supplied scripts use COM5.
4. Use a baud rate of 115200.
5. Run the selected experiment.
6. Follow the displayed instructions to enter detection or recognition responses.

The programs automatically save experimental results as CSV files.

### Example Serial Commands

Single-motor stimulation:

`A,8,1000`

Simultaneous stimulation:

`SIM,AB,8,12,1000`

Sequential stimulation:

`SEQ,AB,8,12,1000`

The Arduino returns DONE after completing a valid stimulation command. Invalid commands produce ERROR messages.

## Experimental Data

The repository includes the CSV files generated during the completed experiments.

The detection dataset contains motor assignments, RTP settings, detection responses, staircase information, and command status.

The recognition dataset contains stimulation patterns, motor-specific RTP settings, entered responses, recognition correctness, command status, and response-entry delays.

## Limitations

The prototype was evaluated through two single-subject proof-of-concept experiments, with one session per experiment.

Physical vibration acceleration and displacement were not independently calibrated.

Actuator contact pressure and exact placement were not standardized.

Mechanical vibration onset timing was not independently measured.

The recognition conditions were presented in a fixed block order.

These limitations restrict the generalizability of the results.

## Future Work

Future research could investigate:
- Calibrated vibration measurements
- Improved actuator mounting and wristband design
- Additional participants and repeated experimental sessions
- Different stimulation intensities and pulse timings
- Recognition during wrist movement
- Personalized stimulation settings

Potential applications include wrist-based navigation feedback for individuals who are blind or have low vision and investigation of vibration perception in individuals with altered tactile sensitivity, including carpal tunnel syndrome.

