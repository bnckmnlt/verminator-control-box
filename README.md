# Automated Vermicomposting Control Box

## Overview
This project is an automated vermicomposting control system designed to monitor and maintain optimal conditions for composting using kitchen waste. It integrates multiple sensors and actuators to automate processes such as soil aeration, moisture control, compost weighing, and vermitea production.

The system is built on **Arduino Mega 2560** and **Arduino Uno**, providing reliable real-time data acquisition and equipment control.

## Features
- **Environmental Monitoring**
  - **BME280**: Measures ambient temperature and humidity.
  - **Capacitive Soil Moisture Sensor**: Tracks soil moisture levels.
  - **NPK Sensor**: Monitors soil nutrient composition.

- **Weight Measurement**
  - **Load Cells (x3)**:
    - Compost mass measurement
    - Reservoir water level
    - Vermitea container volume

- **Actuation**
  - **Peristaltic Pump**: Distributes vermitea.
  - **Misting Systems**:
    - Conveyor misting
    - Soil misting
  - **Fans**:
    - Soil aeration fan
    - Ambient aeration fan
  - **Servo Motor**: Controls conveyor gate mechanism.
  - **NEMA17 Stepper Motors (x3) with TB6600 Drivers**:
    - Conveyor belt movement
    - Rake operation
    - Soil sifter

## System Architecture

| Component           | Controller    | Function                                         |
|---------------------|---------------|--------------------------------------------------|
| BME280              | Arduino Mega  | Temperature & humidity sensing                   |
| Capacitive Soil V2  | Arduino Mega  | Soil moisture monitoring                         |
| NPK Sensor          | Arduino Mega  | Soil nutrient analysis                           |
| 3× Load Cells       | Arduino Mega  | Weight monitoring (compost, reservoir, vermitea) |
| Peristaltic Pump    | Arduino Mega  | Vermitea distribution                            |
| Misting Systems     | Arduino Mega  | Conveyor and soil moisture control               |
| Fans (4×)           | Arduino Mega  | Soil and ambient aeration                        |
| Servo Motor         | Arduino Uno   | Conveyor gate                                    |
| 3× NEMA17 + TB6600  | Arduino Uno   | Conveyor, rake, soil sifter motion               |

## Hardware Requirements
- Arduino Mega 2560
- Arduino Uno
- BME280 temperature & humidity sensor
- Capacitive soil moisture sensor v2
- NPK sensor
- 3× Load Cells with HX711 or similar amplifier
- 3x Peristaltic pump
- 2× 12V fans and 2x 24V fans
- Servo motor
- 3× NEMA17 stepper motors
- 3× TB6600 stepper drivers
- 5V/12V/24V power supply (depending on motor requirements)
- Miscellaneous components: relays, MOSFETs, wiring, power regulation modules, DC Booster

## Software Requirements
- **Arduino IDE** (or PlatformIO)
- Libraries:
  - `AccelStepper`
  - `Adafruit_BME280_Library`
  - `Adafruit_BusIO`
  - `Adafruit_NeoPixel`
  - `Adafruit_TiCoServo`
  - `Adafruit_Unified_Sensor`
  - `Adafruit_TiCoServo`
  - `ArduinoJson`
  - `HX711` 
  - `HX711_ADC` for load cells

## Setup Instructions
1. **Hardware Assembly**
   - Connect all sensors and actuators to the Arduino Mega according to the pin mapping defined in the source code.
   - Use the Arduino Uno for additional I/O if required.

2. **Software Configuration**
   - Install the required libraries in Arduino IDE.
   - Adjust configuration files (e.g., pin definitions, thresholds) as needed.
   - Upload the provided firmware to the Arduino Mega and Uno.

3. **Power Management**
   - Ensure proper power rating for stepper motors and fans.
   - Use separate power rails where necessary to avoid voltage drops.

## Operation
Once powered and programmed, the system:
- Continuously monitors environmental conditions (temperature, humidity, soil moisture, and NPK levels).
- Controls misting pumps and fans to maintain optimal composting conditions.
- Weighs compost, reservoir water, and vermitea in real time.
- Automates the movement of the conveyor, rake, and soil sifter via stepper motors.
- Dispenses vermitea automatically through the peristaltic pump.
- Use either serial communication or the MQTT protocol to connect the Raspberry Pi and store data in the database.

## License
This project is released under the MIT License. See [LICENSE](LICENSE) for details.

## Author
Developed and maintained by Think I/O.
