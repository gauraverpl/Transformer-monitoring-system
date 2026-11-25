# Transformer Monitoring System

An ESP32-based real-time transformer monitoring and protection system. The project combines temperature, oil/liquid level, ultrasonic distance, and toggle-switch inputs to help monitor transformer conditions and provide short-circuit protection alerts through the Blynk mobile application.

## Features

- Real-time monitoring of transformer temperature
- Oil or liquid level monitoring
- Ultrasonic distance measurement
- Toggle-switch input for protection and circuit control
- Short-circuit protection alarm or warning logic
- Remote monitoring through the Blynk application
- ESP32 Wi-Fi connectivity
- Suitable for a basic laboratory or prototype monitoring setup

## Hardware

- ESP32 development board
- Temperature sensor, such as DS18B20 or a compatible analog sensor
- Liquid or oil level sensor
- HC-SR04 ultrasonic sensor
- Toggle switch
- Relay or switching circuit for protection use, if required
- Breadboard or PCB
- Jumper wires
- USB cable or external power supply

> **Safety warning:** Transformers can operate at dangerous voltage levels. This project should not be connected directly to high-voltage transformer terminals. Use appropriately rated sensors, isolation, relays, fuses, and enclosures, and consult a qualified electrical engineer before using it in a real installation.

## Project Overview

The ESP32 reads the connected sensors and switch state, processes threshold values, and sends monitoring data to Blynk. Sensor readings can be viewed in the Blynk dashboard, while abnormal temperature, low oil level, or a protection-triggering event can activate an alarm or safety relay.

## Suggested Pin Connections

Update these pins according to the final ESP32 sketch and wiring:

| Component | ESP32 Pin | Notes |
|---|---:|---|
| Temperature sensor signal | GPIO 4 | Change for the selected sensor |
| Liquid sensor signal | GPIO 34 | Prefer ADC input |
| Ultrasonic TRIG | GPIO 5 | Digital output |
| Ultrasonic ECHO | GPIO 18 | Digital input |
| Protection toggle switch | GPIO 19 | Use an appropriate pull-up/pull-down configuration |
| Relay/contactor control | GPIO 23 | Use with a transistor or relay driver module |

The exact pin assignments may change when firmware is added. The liquid sensor and ultrasonic echo pin should be 3.3 V compatible.

## Blynk Setup

1. Create a project or template in the Blynk IoT console.
2. Add virtual widgets for:
   - Temperature
   - Oil/liquid level
   - Ultrasonic distance
   - Protection status
   - Alert indicator
3. Create a device and copy its authentication details.
4. Configure the ESP32 Wi-Fi SSID and password.
5. Add the Blynk authentication token to the ESP32 firmware.
6. Upload the firmware and open the Blynk dashboard.

## Firmware Configuration

Before uploading firmware, define the following values:

```cpp
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Transformer Monitor"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const float TEMPERATURE_ALERT_C = 70.0;
const float MIN_OIL_LEVEL_PERCENT = 30.0;
const float OBSTACLE_DISTANCE_CM = 10.0;
```

Recommended Blynk virtual pins:

| Measurement | Virtual Pin |
|---|---|
| Temperature | V0 |
| Oil/liquid level | V1 |
| Ultrasonic distance | V2 |
| Protection status | V3 |
| Alert indicator | V4 |

## Operation

1. Wire the sensor and protection circuits with the transformer isolated and safely powered down.
2. Connect the ESP32 and verify its 3.3 V/GND connections.
3. Upload the ESP32 firmware.
4. Check that the ESP32 connects to Wi-Fi.
5. Confirm that all sensor readings appear in Blynk.
6. Test the protection toggle and relay or alarm output at low voltage before field use.
7. Calibrate temperature, oil level, and distance thresholds for the specific hardware.

## Monitoring Parameters

Adjust alert thresholds based on the transformer specification and environment:

- **Temperature:** identify an acceptable operating range and trigger warning/cutoff limits.
- **Oil level:** calibrate sensor values for full, warning, and low-level conditions.
- **Ultrasonic distance:** use it only for the intended mechanical or container measurement.
- **Toggle switch:** use a normally open/normally closed configuration with an appropriate pull resistor.

## Safety and Protection

The toggle switch is a prototype-level protection input and should not replace certified transformer protection equipment. A real short-circuit protection system should use properly rated current sensors, isolation, protective relays, fuses, breakers, and certified control hardware.

## Notes

- ESP32 ADC readings may vary with supply voltage and require calibration.
- Temperature sensors, liquid sensors, ultrasonic modules, and relays may require different wiring and libraries.
- Wi-Fi outages can interrupt Blynk updates; log local or fail-safe protection logic independently when necessary.
- This repository currently does not include a firmware file or license. Add the sketch, wiring diagram, bill of materials, and calibration instructions before claiming production readiness.
