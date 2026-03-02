# Event Horizon GNC: Hardware-in-the-Loop (HITL) Flight Software

[![F´ Framework](https://img.shields.io/badge/NASA-F%C2%B4%20(F%20Prime)-blue?logo=nasa)](https://nasa.github.io/fprime/)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Hardware](https://img.shields.io/badge/Hardware-ESP32--S3-orange.svg)]()

**Event Horizon** is a parallel prototype for a Guidance, Navigation, and Control (GNC) system modeling spacecraft proximity operations. Built using **NASA's open-source F´ (F Prime) flight software framework**, this repository represents **Phase 2** of the project: full Hardware-in-the-Loop (HITL) integration.

Watch the live hardware F´ FSW demonstration here: **[Event Horizon GNC HITL Demo (YouTube)](https://youtu.be/jNDhC6qe1r4)**

---

## 🚀 Systems Architecture Overview

Unlike early software simulators, this repository operates on live physical hardware data. Abstract math and simulated rate groups have been replaced with a real-time serial telemetry pipeline.

### 1. The Hardware Interface (ESP32-S3)
A custom-built capacitive sensor array acts as the spatial disruption environment.
* **Physics & Filtering:** The microcontroller captures raw capacitance field data and applies a local Exponential Moving Average (EMA) filter to handle the analog noise floor.
* **Telemetry Streaming:** Processed data is packaged into `$GNC,Z_Dist,Sig,X_Err,Y_Err` UART byte streams and pushed over a serial connection at a strict 5Hz timing interval to prevent flight software buffer overflows.
* **Calibration Sequence:** The hardware initiates a 5-second "hands-off" electrical flush upon boot to capture a clean environmental baseline.

### 2. The Flight Software (F´ ProximityDriver)
An active custom F´ component (`ProximityDriver`) natively interfaces with the hardware stream.
* **Deframing:** Catching raw UART bytes via `Drv.ByteStreamData`, the driver allocates and deallocates buffers asynchronously, reconstructing and parsing the `$GNC` strings using `sscanf`.
* **GNC Translation Logic:** X and Y spatial error vectors are evaluated against a programmed `DEADZONE` (20.0f) and a tuned sensitivity ceiling (`MAX_ERROR = 80.0f`). 
* **Actuation:** The component translates spatial interruptions into proportional **0-100% thrust commands** across a 4-axis RCS model (Top, Bottom, Left, Right).

### 3. The Ground Data System (GDS)
Real-time visualization of the deep-space Z-Axis math, sensor signal strength, and live thruster firing using the standard F´ localized ground station.

---

## 🛠️ Hardware & Pinout Setup

* **Microcontroller:** ESP32-S3 Development Board
* **Sensors:** 4x Capacitive Probes mapped to a central spatial zone.
* **Pin Mapping:**
  * `PIN 4` : Top Sensor 
  * `PIN 5` : Bottom Sensor 
  * `PIN 6` : Left Sensor 
  * `PIN 7` : Right Sensor 

---

## 💻 Build and Deployment Instructions

### Prerequisites
* [F´ (F Prime) Requirements](https://nasa.github.io/fprime/UsersGuide/user/install.html) (Python virtual environment, CMake, Ninja)
* macOS (Darwin) or Linux build environment.

### 1. Build the Flight Software
Navigate to the root directory of the repository and utilize the `fprime-util` build system:
```bash
fprime-util generate
fprime-util build -j 4
